#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <regex.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "select_header.h"

#define max(a, b)   ((a) > (b) ? (a) : (b))

/********************************************************/
void gestore(int signo)
{
    int stato;
    printf("esecuzione gestore di SIGCHLD\n");
    wait(&stato);
}

int main(int argc, char **argv)
{
    struct sockaddr_in cliaddr, servaddr;
    struct hostent *hostTcp, *hostUdp;
    int port, listen_sd, conn_sd, udp_sd, nread, maxfdp1, len;
    const int on = 1;
    fd_set rset;

    // TCP variabili
    char nome_file[STRING_LENGTH], parola[STRING_LENGTH];
    int nome_file_length, parola_length, file_fd;
    char c;
    int parola_ind;
    int file_ind;
    int word_found;
    int total_word_found;

    // UDP variabili
    UDPReq req;
    char chr;
    int ris, word_ind, found;

    /* CONTROLLO ARGOMENTI ---------------------------------- */
    if (argc != 2)
    {
        printf("Error: %s port \n", argv[0]);
        exit(1);
    }
    else
    {
        nread = 0;
        while (argv[1][nread] != '\0')
        {
            if ((argv[1][nread] < '0') || (argv[1][nread] > '9'))
            {
                printf("Secondo argomento non intero\n");
                exit(2);
            }
            nread++;
        }
        port = atoi(argv[1]);
        if (port < 1024 || port > 65535)
        {
            printf("Porta scorretta...");
            exit(2);
        }
    }

    /* INIZIALIZZAZIONE INDIRIZZO SERVER ----------------------------------------- */
    memset((char *)&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    /* CREAZIONE E SETTAGGI SOCKET TCP --------------------------------------- */
    listen_sd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sd < 0)
    {
        perror("creazione socket ");
        exit(3);
    }
    printf("Server: creata la socket d'ascolto per le richieste di ordinamento, fd=%d\n",
           listen_sd);

    if (setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    {
        perror("set opzioni socket d'ascolto");
        exit(3);
    }
    printf("Server: set opzioni socket d'ascolto ok\n");

    if (bind(listen_sd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind socket d'ascolto");
        exit(3);
    }
    printf("Server: bind socket d'ascolto ok\n");

    if (listen(listen_sd, 5) < 0)
    {
        perror("listen");
        exit(3);
    }
    printf("Server: listen ok\n");

    /* CREAZIONE E SETTAGGI SOCKET UDP --------------------------------------- */
    udp_sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sd < 0)
    {
        perror("apertura socket UDP");
        exit(4);
    }
    printf("Creata la socket UDP, fd=%d\n", udp_sd);

    if (setsockopt(udp_sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    {
        perror("set opzioni socket UDP");
        exit(4);
    }
    printf("Set opzioni socket UDP ok\n");

    if (bind(udp_sd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind socket UDP");
        exit(4);
    }
    printf("Bind socket UDP ok\n");

    signal(SIGCHLD, gestore);

    /* PULIZIA E SETTAGGIO MASCHERA DEI FILE DESCRIPTOR ------------------------- */
    FD_ZERO(&rset);
    maxfdp1 = max(listen_sd, udp_sd) + 1;

    /* CICLO DI RICEZIONE RICHIESTE --------------------------------------------- */
    for (;;)
    {
        FD_SET(listen_sd, &rset);
        FD_SET(udp_sd, &rset);

        if ((nread = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                perror("select");
                exit(5);
            }
        }
        /* GESTIONE RICHIESTE UDP  ----------------------------- */
        if (FD_ISSET(udp_sd, &rset))
        {
            printf("Ricevuta richiesta di UDP\n");
            len = sizeof(struct sockaddr_in);


            // RICEVO REQUEST UDP
            if (recvfrom(udp_sd, &req, sizeof(req), 0, (struct sockaddr *)&cliaddr, &len) < 0)
            {
                perror("recvfrom ");
                continue;
            }


            hostUdp = gethostbyaddr((char *)&cliaddr.sin_addr, sizeof(cliaddr.sin_addr), AF_INET);
            if (hostUdp == NULL)
            {
                printf("client host information not found\n");
            }
            else
            {
                printf("Operazione richiesta da: %s %i\n", hostUdp->h_name, (unsigned)ntohs(cliaddr.sin_port));
            }

            // APRO FILE
            file_fd = open(req.nomefile, O_RDONLY);
            if ( file_fd < 0 ) {
                perror("open");
                continue;
            }

            word_ind = 0;
            ris = -1;
            while (nread = read(file_fd,&chr,sizeof(chr)) > 0) {
                if ( chr == '\n' ) {
                    word_ind = 0;
                }
                if ( req.parola[word_ind] == chr ) {
                    word_ind++;
                    if ( word_ind == strlen(req.parola) ) {
                        ris = 0;
                        break;
                    }
                } else {
                    word_ind = 0;
                }
            }

            close(file_fd);

            if ( nread < 0 ) {
                perror("nread");
                continue;
            }

            // INVIO RISULTATO AL CLIENT
            ris = htonl(ris);
            if (sendto(udp_sd, &ris, sizeof(int), 0, (struct sockaddr *)&cliaddr, len) < 0)
            {
                perror("sendto ");
                continue;
            }
        }
        /* GESTIONE RICHIESTE TCP  ----------------------------- */
        if (FD_ISSET(listen_sd, &rset))
        {
            printf("Ricevuta richiesta TCP\n");
            len = sizeof(cliaddr);
            if ((conn_sd = accept(listen_sd, (struct sockaddr *)&cliaddr, &len)) < 0)
            {
                if (errno == EINTR)
                {
                    perror("Forzo la continuazione della accept");
                    continue;
                }
                else
                    exit(6);
            }
            if (fork() == 0)
            {
                // GESTIONE FIGLIO TCP
                close(listen_sd);
                hostTcp = gethostbyaddr((char *)&cliaddr.sin_addr, sizeof(cliaddr.sin_addr), AF_INET);
                if (hostTcp == NULL)
                {
                    printf("client host information not found\n");
                    close(conn_sd);
                    exit(6);
                }
                else
                {
                    printf("Server (figlio): host client e' %s \n", hostTcp->h_name);
                }

                // LEGGO LUNGHEZZA NOME FILE
                while ((nread = read(conn_sd, &nome_file_length, sizeof(nome_file_length)) > 0) ) {
                    nome_file_length = ntohl(nome_file_length);

                    // LEGGO NOME FILE
                    if ( read(conn_sd, nome_file, nome_file_length) < 0 ) {
                        perror("read");
                        close(conn_sd);
                        exit(6);
                    }

                    // LEGGO LUNGHEZZA PAROLA
                    if ( read(conn_sd, &parola_length, sizeof(parola_length)) < 0 ) {
                        perror("read");
                        close(conn_sd);
                        exit(6);
                    }

                    parola_length = ntohl(parola_length);

                    // LEGGO PAROLA
                    if ( read(conn_sd, parola, parola_length) < 0 ) {
                        perror("read");
                        close(conn_sd);
                        exit(6);
                    }

                    printf("Server (figlio): file_richiesto: %s\n", nome_file);

                    file_fd = open(nome_file,O_RDONLY);

                    if ( file_fd < 0 ) {
                        perror("open");
                        close(conn_sd);
                        exit(6);
                    }

                    parola_ind = 0;
                    file_ind = 0;
                    word_found = 0;
                    total_word_found = 0;

                    // CONTO PAROLA TROVATE
                    while ( read(file_fd, &c, sizeof(char)) > 0 ) {

                        if ( c == '\n' ) {
                            parola_ind = 0;
                            word_found = 0;
                        }

                        if ( c == parola[parola_ind] && word_found == 0) {
                            parola_ind++;

                            // PAROLA INTERA TROVATA
                            if ( parola_ind == strlen(parola) ) {
                                word_found = 1;
                                total_word_found++;
                            }

                        } else {
                            parola_ind = 0;   
                        }
                    }

                    printf("%i\n", total_word_found);

                    // INVIO PAROLE TROVATE
                    total_word_found = htonl(total_word_found);
                    if ( write(conn_sd, &total_word_found, sizeof(total_word_found)) < 0 ) {
                        perror("write");
                        close(conn_sd);
                        exit(6);
                    }

                    lseek(file_fd,0,SEEK_SET);

                    parola_ind = 0;
                    file_ind = 0;
                    word_found = 0;
                    // INVIO PAROLE E LINEE
                    while ( read(file_fd, &c, sizeof(char)) > 0 ) {

                        file_ind++;

                        if ( c == '\n' ) {
                            parola_ind = 0;
                            file_ind = 0;
                            word_found = 0;
                        }

                        if ( c == parola[parola_ind] && word_found == 0) {
                            parola_ind++;

                            // PAROLA INTERA TROVATA
                            if ( parola_ind == strlen(parola) ) {
                                word_found = 1;

                                printf("ind: %i\n",file_ind);
                                lseek(file_fd, -file_ind, SEEK_CUR);
                                
                                file_ind = 0;

                                do {
                                    nread = read(file_fd, &c, sizeof(c));
                                    file_ind++;
                                } while ( c != '\n' && nread > 0) ;

                                file_ind -= 1;

                                lseek(file_fd, -(file_ind + 1), SEEK_CUR);         
                                file_ind = htonl(file_ind);

                                // INVIO LUNGHEZZA DELLA RIGA
                                if ( write(conn_sd, &file_ind, sizeof(file_ind)) < 0 ) {
                                    perror("write");
                                    close(conn_sd);
                                    exit(6);
                                }

                                file_ind = ntohl(file_ind);

                                // INVIO RIGA
                                for ( int i = 0; i < file_ind; i++ ) {
                                    read(file_fd, &c, sizeof(c));
                                    write(conn_sd, &c, sizeof(c));
                                }

                                parola_ind = 0;
                                file_ind = 0;
                                word_found = 0;
                            }
                        } else {
                            parola_ind = 0;
                        }
                    }
                }
                
                

                close(file_fd);

                // Libero risorse
                printf("Figlio TCP terminato, libero risorse e chiudo. \n");
                close(conn_sd);
                exit(0);
            }
            close(conn_sd);
        }
    }
}