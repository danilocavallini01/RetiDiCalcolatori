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
    char tipo_prenotazione[TPREN_LENGTH], fileName[IMMAG_LENGTH];
    char buff[STRING_LENGTH];
    int tipo_prenotazione_length, file_name_length;

    int found, image_fd, file_length;

    // UDP variabili
    UDPReq req;
    int ris;

    Prenotazione prenotazioni[MAX_PREN];
    Prenotazione p;
    for (int i = 0; i < MAX_PREN; i++)
    {
        strcpy(p.id,"L");
        strcpy(p.n_persone,"L");
        strcpy(p.tipo_prenotazione,"L");
        strcpy(p.tipo_veicolo,"L");
        strcpy(p.targa,"L");
        strcpy(p.immagine,"L");
        prenotazioni[i] = p;
    }

    strcpy(prenotazioni[0].id,"HGFD89");
    strcpy(prenotazioni[0].n_persone,"3");
    strcpy(prenotazioni[0].tipo_prenotazione,"mezza piazzola");
    strcpy(prenotazioni[0].tipo_veicolo,"niente");
    strcpy(prenotazioni[0].targa,"L");
    strcpy(prenotazioni[0].immagine,"./fra/ciao");

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

            // AGGIORNAMENTO PRENOTAZIONE
            ris = -1;
            for ( int i = 0; i < MAX_PREN; i++ ) {
                if ( strcmp(prenotazioni[i].id,req.id) == 0 ) {
                    strcpy(prenotazioni[i].n_persone,req.n_persone);
                    ris = 0;
                    break;
                } 
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
            printf("Ricevuta richiesta TCP: richiesta di download dei file\n");
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

                // LEGGO LUNGHEZZA TIPO PRENOTAZIONE
                if ( read(conn_sd, &tipo_prenotazione_length, sizeof(tipo_prenotazione_length)) < 0 ) {
                    perror("read");
                    close(conn_sd);
                    exit(6);
                }

                tipo_prenotazione_length = ntohl(tipo_prenotazione_length);

                // LEGGO TIPO PRENOTAZIONE
                if ( read(conn_sd, tipo_prenotazione, tipo_prenotazione_length) < 0 ) {
                    perror("read");
                    close(conn_sd);
                    exit(6);
                }

                printf("Server (figlio): prenotazione richiesta: %s\n", tipo_prenotazione);

                found = 0;

                for ( int i = 0; i < MAX_PREN; i++ ) {
                    if ( strcmp(prenotazioni[i].tipo_prenotazione,tipo_prenotazione) == 0 ) {
                        found++;
                    }
                }


                // INVIO NUMERO DI PRENOTAZIONI TROVATE
                found = htonl(found);
                if ( write(conn_sd, &found, sizeof(found)) < 0) {
                    perror("write");
                    close(conn_sd);
                    exit(6);
                }

                // INVIO FILES
                for ( int i = 0; i < MAX_PREN; i++ ) {
                    if ( strcmp(prenotazioni[i].tipo_prenotazione,tipo_prenotazione) == 0 ) {

                        strcpy(fileName,prenotazioni[i].immagine);

                        // INVIO LUNGHEZZA NOME FILE
                        file_name_length = htonl(strlen(fileName) + 1);
                        if (write(conn_sd, &file_name_length, sizeof(file_name_length) ) < 0)
                        {
                            perror("Errore nell'invio del nome file\n");
                            close(conn_sd);
                            exit(6);
                        }
                        
                        // INVIO NOME FILE
                        if (write(conn_sd, fileName, (strlen(fileName) + 1)) < 0)
                        {
                            perror("Errore nell'invio del nome file\n");
                            close(conn_sd);
                            exit(6);
                        }

                        // CALCOLO LUNGHEZZA FILE
                        printf("path: %s\n", fileName);
                        image_fd = open(fileName, O_RDONLY);
                        if ( image_fd < 0 ) {
                            perror("open");
                            close(conn_sd);
                            exit(6);
                        }

                        file_length = lseek(image_fd, 0, SEEK_END);
                        lseek(image_fd, 0, SEEK_SET);

                        printf("Invio lunghezza file: %i\n", file_length);
                        file_length = htonl(file_length);

                        // INVIO LUNGHEZZA FILE
                        if (write(conn_sd, &file_length, sizeof(file_length)) < 0)
                        {
                            perror("Errore nell'invio della lunghezza\n");
                            close(conn_sd);
                            close(image_fd);
                            exit(6);
                        }

                        // INVIO FILE
                        while ((nread = read(image_fd, buff, sizeof(buff))) > 0)
                        {
                            if ( write(conn_sd, buff, nread) < 0 ) {
                                perror("nwrite");
                                close(conn_sd);
                                close(image_fd);
                                exit(6);
                            }
                        }

                        if ( nread < 0 ) {
                            perror("nread");
                            close(conn_sd);
                            close(image_fd);
                            exit(6);
                        }

                        close(image_fd);
                    }
                }

               

                // Libero risorse
                printf("Figlio TCP terminato, libero risorse e chiudo. \n");
                close(conn_sd);
                exit(0);
            }
            close(conn_sd);
        }
    }
}