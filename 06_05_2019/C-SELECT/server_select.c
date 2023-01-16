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

#define STRING_LENGTH 50
#define MAX_FILES 255
#define max(a, b) ((a) > (b) ? (a) : (b))

/********************************************************/
void gestore(int signo)
{
    int stato;
    printf("esecuzione gestore di SIGCHLD\n");
    wait(&stato);
}
/********************************************************/

int main(int argc, char **argv)
{
    struct sockaddr_in cliaddr, servaddr;
    struct hostent *hostTcp, *hostUdp;
    int port, listen_sd, conn_sd, udp_sd, nread, maxfdp1, len;
    const int on = 1;
    fd_set rset;

    // TCP variabili
    char dir_name[STRING_LENGTH], files[MAX_FILES][STRING_LENGTH], fileName[STRING_LENGTH], path[STRING_LENGTH * 2], buff[STRING_LENGTH];
    char risp, c;
    DIR *dir;
    struct dirent *file;
    int dir_name_length, file_count, consonanti, vocali, ind, 
    file_name_length, i, fd, file_length;

    // UDP variabili
    char file_name_udp[STRING_LENGTH],tmp_file[STRING_LENGTH],chr;
    int ris, file_fd, tmp_fd;

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
            printf("Ricevuta richiesta di UDP: eliminazione vocali da un file\n");
            len = sizeof(struct sockaddr_in);

            if (recvfrom(udp_sd, file_name_udp, sizeof(file_name_udp), 0, (struct sockaddr *)&cliaddr, &len) < 0)
            {
                perror("recvfrom ");
                continue;
            }

            printf("Operazione richiesta su file: %s\n", file_name_udp);

            hostUdp = gethostbyaddr((char *)&cliaddr.sin_addr, sizeof(cliaddr.sin_addr), AF_INET);
            if (hostUdp == NULL)
            {
                printf("client host information not found\n");
            }
            else
            {
                printf("Operazione richiesta da: %s %i\n", hostUdp->h_name, (unsigned)ntohs(cliaddr.sin_port));
            }

            file_fd = open(file_name_udp, O_RDONLY);
            strcpy(tmp_file,file_name_udp);
            strcat(tmp_file, "__TMP_.tmp");
            tmp_fd = open(tmp_file, O_WRONLY | O_CREAT, 0777);

            if (file_fd < 0 || tmp_fd < 0)
            {
                perror("open");
                // INVIO RISULTATO AL CLIENT
                ris = -1;
                ris = htonl(ris);
                if (sendto(udp_sd, &ris, sizeof(int), 0, (struct sockaddr *)&cliaddr, len) < 0)
                {
                    perror("sendto ");
                    continue;
                }

                continue;
            }

            ris = 0;

            // LEGGO E SCRIVO CARATTERE
            while ((nread = read(file_fd, &chr, 1)) > 0)
            {
                if (nread < 0)
                {
                    perror("read");
                    // INVIO RISULTATO AL CLIENT
                    ris = -1;
                    ris = htonl(ris);
                    close(file_fd);
                    close(tmp_fd);
                    if (sendto(udp_sd, &ris, sizeof(int), 0, (struct sockaddr *)&cliaddr, len) < 0)
                    {
                        perror("sendto ");
                        continue;
                    }

                    continue;
                }

                if (chr != 'A' && chr != 'E' && chr != 'I' && chr != 'O' && chr != 'U' && chr != 'a' && chr != 'e' && chr != 'i' && chr != 'o' && chr != 'u')
                {

                    if (write(tmp_fd, &chr, sizeof(char)) < 0)
                    {
                        perror("write");
                        // INVIO RISULTATO AL CLIENT
                        ris = -1;
                        ris = htonl(ris);
                        close(file_fd);
                        close(tmp_fd);  
                        if (sendto(udp_sd, &ris, sizeof(int), 0, (struct sockaddr *)&cliaddr, len) < 0)
                        {
                            perror("sendto ");
                            continue;
                        }

                        continue;
                    }
                }
                else
                {
                    ris++;
                }
            }

            close(file_fd);
            close(tmp_fd);

            rename(tmp_file,file_name_udp);

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
            { // GESTIONE FIGLIO -------------------------------
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

                // LEGGO LUNGHEZZA NOME DEL DIRETTORIO
                if (read(conn_sd, &dir_name_length, sizeof(dir_name_length)) < 0)
                {
                    perror("read");
                    close(conn_sd);
                    exit(6);
                }

                dir_name_length = ntohl(dir_name_length);

                // LEGGO NOME DEL DIRETTORIO
                if (read(conn_sd, dir_name, dir_name_length) < 0)
                {
                    perror("read");
                    close(conn_sd);
                    exit(6);
                }

                printf("Server (figlio): Direttorio richiesto: %s\n", dir_name);

                if ((dir = opendir(dir_name)) != NULL)
                {
                    risp = 'S';
                    printf("Invio risposta affermativa al client\n");
                    if (write(conn_sd, &risp, sizeof(char)) < 0)
                    {
                        perror("write");
                        close(conn_sd);
                        exit(6);
                    }

                    file_count = 0, i = 0;
                    while ((file = readdir(dir)) != NULL)
                    {
                        strcat(file->d_name, "\0");
                        printf("%s\n",file->d_name);
                        ind = 0, consonanti = 0, vocali = 0;

                        while ((c = file->d_name[ind]) != '\0')
                        {
                            if (c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U' || c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u')
                            {
                                vocali++;
                            }
                            else
                            {
                                consonanti++;
                            }

                            ind++;

                            if (consonanti >= 1 && vocali >= 1)
                            {
                                strcpy(files[i++],file->d_name);
                                file_count++;
                                break;
                            }
                        }
                    }

                    // INVIO NUMERO FILE
                    printf("Invio numero dei file\n");
                    file_count = htonl(file_count);
                    if (write(conn_sd, &file_count, sizeof(file_count)) < 0)
                    {
                        perror("write");
                        close(conn_sd);
                        exit(6);
                    }

                    file_count = ntohl(file_count);

                    // LOOP OVER VALID FILES
                    for (i = 0; i < file_count; i++)
                    {

                        strcpy(fileName, files[i]);

                        printf("Invio nome fileName: %s\n", fileName);

                        // INVIO LUNGHEZZA NOME FILE
                        file_name_length = htonl(strlen(fileName) + 1);
                        if (write(conn_sd, &file_name_length, sizeof(int)) < 0)
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

                        // SCRIVO PATH
                        strcpy(path, dir_name);
                        strcat(path, "/");
                        strcat(path, fileName);

                        fd = open(path, O_RDONLY);

                        if (fd < 0)
                        {
                            perror("open");
                            continue;
                        }

                        file_length = lseek(fd, 0, SEEK_END);
                        lseek(fd, 0, SEEK_SET);

                        printf("Invio lunghezza file: %i\n", file_length);
                       
                        // INVIO LUNGHEZZA FILE
                        file_length = htonl(file_length);
                        if (write(conn_sd, &file_length, sizeof(file_length)) < 0)
                        {
                            perror("Errore nell'invio della lunghezza\n");

                            close(fd);
                            close(conn_sd);
                            exit(6);
                        }

                        // INVIO FILE
                        while ((nread = read(fd, buff, sizeof(buff))) > 0)
                        {
                            if (nread < 0)
                            {
                                perror("read");
                                close(fd);
                                close(conn_sd);
                                exit(6);
                            }
                            if (write(conn_sd, buff, nread) < 0)
                            {
                                perror("write");
                                close(fd);
                                close(conn_sd);
                                exit(6);
                            }
                        }

                        close(fd);
                    }
                }
                else
                { // err apertura dir
                    risp = 'N';
                    printf("Invio risposta negativa al client\n");
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