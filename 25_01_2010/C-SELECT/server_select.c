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

#define max(a, b) ((a) > (b) ? (a) : (b))

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
    int N, gg, mm, aa, found;
    char risp;
    int title_length;

    // UDP variabili
    UDPReq req;
    int ris;

    Prenotazione prenotazioni[MAX_PREN];
    Prenotazione p;
    for (int i = 0; i < MAX_PREN; i++)
    {
        strcpy(prenotazioni[i].titolo, "L");
        for (int j = 0; j < M; j++)
        {
            prenotazioni[i].copie[j][0] = -1;
            prenotazioni[i].copie[j][1] = -1;
            prenotazioni[i].copie[j][2] = -1;
        }

        prenotazioni[i].copieTotali = -1;
        prenotazioni[i].copieDisp = -1;
    }

    strcpy(prenotazioni[0].titolo, "VANGOGH");
    prenotazioni[0].copie[0][0] = 27;
    prenotazioni[0].copie[0][1] = 12;
    prenotazioni[0].copie[0][2] = 2000;

    prenotazioni[0].copieTotali = 1;
    prenotazioni[0].copieDisp = 0;


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
                if (strcmp(req.titolo, prenotazioni[i].titolo) == 0 ) {
                    for ( int j = 0; j < prenotazioni[i].copieTotali - prenotazioni[i].copieDisp; j++ ) {
                        if ( req.gg == prenotazioni[i].copie[j][0] && req.mm == prenotazioni[i].copie[j][1] && req.aa == prenotazioni[i].copie[j][2] ) {
                            ris = 0;
                            prenotazioni[i].copie[j][0] = -1;
                            prenotazioni[i].copie[j][1] = -1;
                            prenotazioni[i].copie[j][2] = -1;
                            prenotazioni[i].copieDisp--;
                            break;
                        }
                    }
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
                    close(udp_sd);
                    exit(1);
                }
                else
                {
                    printf("Server (figlio): host client e' %s \n", hostTcp->h_name);
                }

                // LEGGO N
                while (read(conn_sd, &N, sizeof(N)) > 0)
                {

                    N = ntohl(N);
                    // CONTROLLO SE N E' VALIDO
                    risp = 'S';
                    if (N > 30 || N < 1)
                    {
                        risp = 'N';
                    }

                    // SCRIVO RISPO
                    if (write(conn_sd, &risp, sizeof(risp)) < 0)
                    {
                        perror("write");
                        close(conn_sd);
                        close(udp_sd);
                        exit(2);
                    }

                    if ( risp == 'N' ) {
                        close(conn_sd);
                        exit(0);
                    }

                    // LEGGO GIORNO
                    if (read(conn_sd, &gg, sizeof(gg)) < 0)
                    {
                        perror("read");
                        close(conn_sd);
                        close(udp_sd);
                        exit(3);
                    }
                    gg = ntohl(gg);

                    // LEGGO MESE
                    if (read(conn_sd, &mm, sizeof(mm)) < 0)
                    {
                        perror("read");
                        close(conn_sd);
                        close(udp_sd);
                        exit(4);
                    }
                    mm = ntohl(mm);

                    // LEGGO ANNO
                    if (read(conn_sd, &aa, sizeof(aa)) < 0)
                    {
                        perror("read");
                        close(conn_sd);
                        close(udp_sd);
                        exit(5);
                    }
                    aa = ntohl(aa);

                    // TOLGO N GIORNI DALLA DATA
                    gg -= N;

                    if (gg < 1)
                    {
                        gg += 30;
                        mm--;
                        if (mm < 1)
                        {
                            mm += 12;
                            aa--;
                        }
                    }

                    printf("%i,%i,%i\n",gg,mm,aa);

                    found = 0;

                    for (int i = 0; i < MAX_PREN; i++)
                    {
                        if (strcmp(prenotazioni[i].titolo, "L") != 0)
                        {
                            for (int j = 0; j < prenotazioni[i].copieTotali - prenotazioni[i].copieDisp; j++)
                            {
                                if (prenotazioni[i].copie[j][2] < aa) {
                                    found++;
                                    break;
                                } else if ( prenotazioni[i].copie[j][2] == aa && prenotazioni[i].copie[j][1] < mm ) {
                                    found++;
                                    break;
                                } else if ( prenotazioni[i].copie[j][2] == aa && prenotazioni[i].copie[j][1] == mm 
                                && prenotazioni[i].copie[j][0] <= gg  )
                                {
                                    found++;
                                    break;
                                }
                            }
                        }
                    }

                    printf("%i\n", found);

                    found = htonl(found);

                    // SCRIVO NUMERO DI TITOLI TROVATI
                    if (write(conn_sd, &found, sizeof(found)) < 0)
                    {
                        perror("write");
                        close(conn_sd);
                        close(udp_sd);
                        exit(2);
                    }

                    for (int i = 0; i < MAX_PREN; i++)
                    {
                        if (strcmp(prenotazioni[i].titolo, "L") != 0)
                        {
                            for (int j = 0; j < prenotazioni[i].copieTotali - prenotazioni[i].copieDisp; j++)
                            {
                                if ((prenotazioni[i].copie[j][2] < aa) || 
                                    ( prenotazioni[i].copie[j][2] == aa && prenotazioni[i].copie[j][1] < mm ) ||
                                    ( prenotazioni[i].copie[j][2] == aa && prenotazioni[i].copie[j][1] == mm 
                                    && prenotazioni[i].copie[j][0] <= gg  ))
                                {

                                    // SCRIVO LUNGHEZZA TITOLO
                                    title_length = htonl(strlen(prenotazioni[i].titolo) + 1);
                                    if (write(conn_sd, &title_length, sizeof(title_length)) < 0)
                                    {
                                        perror("write");
                                        close(conn_sd);
                                        close(udp_sd);
                                        exit(6);
                                    }

                                    // SCRIVO TITOLO
                                    if (write(conn_sd, prenotazioni[i].titolo, strlen(prenotazioni[i].titolo) + 1) < 0)
                                    {
                                        perror("write");
                                        close(conn_sd);
                                        close(udp_sd);
                                        exit(7);
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }

                // Libero risorse
                printf("Figlio TCP terminato, libero risorse e chiudo. \n");
                close(conn_sd);
                close(udp_sd);
                exit(0);
            }
            close(conn_sd);
        }
    }
}