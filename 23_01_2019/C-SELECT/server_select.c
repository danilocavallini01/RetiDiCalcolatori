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

#define MAX_PREN 10
#define TARGA_LENGTH 8
#define NAME_LENGTH 255
#define PATENTE_LENGTH 6
#define max(a, b)   ((a) > (b) ? (a) : (b))

/********************************************************/
void gestore(int signo)
{
    int stato;
    printf("esecuzione gestore di SIGCHLD\n");
    wait(&stato);
}
/********************************************************/

typedef struct
{
    char targa[TARGA_LENGTH];
    char patente[PATENTE_LENGTH];
    char tipoVeicolo[7];
    char immagini[TARGA_LENGTH + 5];
} Prenotazione;

typedef struct
{
    char targa[TARGA_LENGTH];
    char patente[PATENTE_LENGTH];
} UDPReq;

int main(int argc, char **argv)
{
    struct sockaddr_in cliaddr, servaddr;
    struct hostent *hostTcp, *hostUdp;
    int port, listen_sd, conn_sd, udp_sd, nread, maxfdp1, len;
    const int on = 1;
    fd_set rset;

    // TCP variabili
    char targa[NAME_LENGTH], fileName[NAME_LENGTH], buff[NAME_LENGTH];
    DIR *dir1;
    struct dirent *dd1;
    int image_fd, file_length, file_count, file_name_length;

    // UDP variabili
    UDPReq req;
    int ris;

    Prenotazione prenotazioni[MAX_PREN];
    for (int i = 0; i < MAX_PREN; i++)
    {
        Prenotazione p;
        strcpy(p.targa,"L");
        strcpy(p.patente,"L");
        strcpy(p.tipoVeicolo,"L");
        strcpy(p.immagini,"L");
        prenotazioni[i] = p;
    }

    strcpy(prenotazioni[0].targa,"AA111AA");
    strcpy(prenotazioni[0].patente,"00100");
    strcpy(prenotazioni[0].tipoVeicolo,"auto");
    strcpy(prenotazioni[0].immagini,"AA111AA_img/");

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
            printf("Ricevuta richiesta di UDP: eliminazione di una parola\n");
            len = sizeof(struct sockaddr_in);

            if (recvfrom(udp_sd, &req, sizeof(req), 0, (struct sockaddr *)&cliaddr, &len) < 0)
            {
                perror("recvfrom ");
                continue;
            }

            printf("Operazione richiesta sulla targa: %s con la patente: %s\n", req.targa,
                   req.patente);

            hostUdp = gethostbyaddr((char *)&cliaddr.sin_addr, sizeof(cliaddr.sin_addr), AF_INET);
            if (hostUdp == NULL)
            {
                printf("client host information not found\n");
            }
            else
            {
                printf("Operazione richiesta da: %s %i\n", hostUdp->h_name, (unsigned)ntohs(cliaddr.sin_port));
            }

            ris = -1;
            for ( int i = 0; i < MAX_PREN; i++ ) {
                if ( strcmp(prenotazioni[i].targa,req.targa) == 0 ) {
                    strcpy(prenotazioni[i].patente,req.patente);
                    ris = 0;
                    break;
                } 
            }

            // Send result to the client
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

                // Leggo la targa
                if ( read(conn_sd, targa, TARGA_LENGTH) < 0 ) {
                    perror("read");
                    continue;
                }

                printf("Server (figlio): targa richiesta: %s\n", targa);

                char risp;
                strcat(targa, "_img/");

                if ((dir1 = opendir(targa)) != NULL)
                {
                    risp = 'S';
                    printf("Invio risposta affermativa al client\n");
                    if ( write(conn_sd, &risp, sizeof(char)) < 0) {
                        perror("write");
                        continue;
                    }

                    file_count = 0;
                    while ((dd1 = readdir(dir1)) != NULL)
                    {
                        file_count++;
                    }

                    file_count -= 2;

                    printf("Invio numero dei file\n");
                    if ( write(conn_sd, &file_count, sizeof(file_count)) < 0) {
                        perror("write");
                        continue;
                    }

                    dir1 = opendir(targa);

                    while ((dd1 = readdir(dir1)) != NULL)
                    {

                        if (strcmp(dd1->d_name, ".") != 0 && strcmp(dd1->d_name, "..") != 0)
                        {
                            fileName[0] = '\0';

                            strcpy(fileName, targa);
                            strcat(dd1->d_name, "\0");
                            strcat(fileName, dd1->d_name);  

                            printf("Invio nome fileName: %s\n", dd1->d_name);

                            // invio lunghezza nome file
                            file_name_length = htonl(strlen(dd1->d_name) + 1);
                            if (write(conn_sd, &file_name_length, sizeof(int) ) < 0)
                            {
                                perror("Errore nell'invio del nome file\n");
                                continue;
                            }
                            
                            // invio nome file
                            if (write(conn_sd, dd1->d_name, (strlen(dd1->d_name) + 1)) < 0)
                            {
                                perror("Errore nell'invio del nome file\n");
                                continue;
                            }

                            printf("path: %s\n", fileName);
                            image_fd = open(fileName, O_RDONLY);
                            file_length = lseek(image_fd, 0, SEEK_END);
                            lseek(image_fd, 0, SEEK_SET);

                            printf("Invio lunghezza file: %i\n", file_length);
                            file_length = htonl(file_length);

                            // invio lunghezza file
                            if (write(conn_sd, &file_length, sizeof(file_length)) < 0)
                            {
                                perror("Errore nell'invio della lunghezza\n");
                                continue;
                            }

                            // invio file
                            while ((nread = read(image_fd, buff, sizeof(buff))) > 0)
                            {
                                printf("%s\nnread: %i\n", buff, nread);
                                write(conn_sd, buff, nread);
                            }

                            close(image_fd);
                        }
                    }
                }
                else
                { // err apertura dir
                    risp = 'N';
                    printf("Invio risposta negativa al client per dir %s \n", targa);
                    write(conn_sd, &risp, sizeof(char));
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