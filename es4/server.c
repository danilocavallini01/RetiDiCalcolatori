    /* Server Select
 * 	Nuovo figlio per ogni richiesta file.
 */

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define DIM_BUFF         100
#define LENGTH_FILE_NAME 20
#define max(a, b)        ((a) > (b) ? (a) : (b))

typedef struct{
    char nome_file[LENGTH_FILE_NAME];
    char parola[LENGTH_FILE_NAME];
} Request;

/*Funzione conteggio file in un direttorio*/
/********************************************************/
int conta_file(char *name) {
    DIR           *dir;
    struct dirent *dd;
    int            count = 0;
    dir                  = opendir(name);
    if (dir == NULL)
        return -1;
    while ((dd = readdir(dir)) != NULL) {
        printf("Trovato il file %s\n", dd->d_name);
        count++;
    }
    /*Conta anche direttorio stesso e padre*/
    printf("Numero totale di file %d\n", count);
    closedir(dir);
    return count;
}
/********************************************************/
void gestore(int signo) {
    int stato;
    printf("esecuzione gestore di SIGCHLD\n");
    wait(&stato);
}
/********************************************************/

int main(int argc, char **argv) {
    int                listenfd, connfd, udpfd, fd_file,fd_temp, nready, maxfdp1;
    const int          on = 1;
    char               buff[DIM_BUFF], nome_file[LENGTH_FILE_NAME],tmp[LENGTH_FILE_NAME], nome_dir[LENGTH_FILE_NAME], parola[LENGTH_FILE_NAME], chr;
    fd_set             rset;
    int                len, nread, nwrite, num, ris, port, erased;
    struct sockaddr_in cliaddr, servaddr;
    Request *req = (Request *)malloc(sizeof(Request));


       /* CONTROLLO ARGOMENTI ---------------------------------- */
    if (argc != 2) {
        printf("Error: %s port\n", argv[0]);
        exit(1);
    }

    nread = 0;
    while (argv[1][nread] != '\0') {
        if ((argv[1][nread] < '0') || (argv[1][nread] > '9')) {
            printf("Terzo argomento non intero\n");
            exit(2);
        }
        nread++;
    }
    port = atoi(argv[1]);
    if (port < 1024 || port > 65535) {
        printf("Porta scorretta...");
        exit(2);
    }

    /* INIZIALIZZAZIONE INDIRIZZO SERVER E BIND ---------------------------- */
    memset((char *)&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port        = htons(port);
    printf("Server avviato\n");

    /* CREAZIONE SOCKET TCP ------------------------------------------------ */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("apertura socket TCP ");
        exit(1);
    }
    printf("Creata la socket TCP d'ascolto, fd=%d\n", listenfd);

    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("set opzioni socket TCP");
        exit(2);
    }
    printf("Set opzioni socket TCP ok\n");

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind socket TCP");
        exit(3);
    }
    printf("Bind socket TCP ok\n");

    if (listen(listenfd, 5) < 0) {
        perror("listen");
        exit(4);
    }
    printf("Listen ok\n");

    /* CREAZIONE SOCKET UDP ------------------------------------------------ */
    udpfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpfd < 0) {
        perror("apertura socket UDP");
        exit(5);
    }
    printf("Creata la socket UDP, fd=%d\n", udpfd);

    if (setsockopt(udpfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("set opzioni socket UDP");
        exit(6);
    }
    printf("Set opzioni socket UDP ok\n");

    if (bind(udpfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind socket UDP");
        exit(7);
    }
    printf("Bind socket UDP ok\n");

    /* AGGANCIO GESTORE PER EVITARE FIGLI ZOMBIE -------------------------------- */
    signal(SIGCHLD, gestore);

    /* PULIZIA E SETTAGGIO MASCHERA DEI FILE DESCRIPTOR ------------------------- */
    FD_ZERO(&rset);
    maxfdp1 = max(listenfd, udpfd) + 1;

    /* CICLO DI RICEZIONE EVENTI DALLA SELECT ----------------------------------- */

    /* CICLO DI RICEZIONE EVENTI DALLA SELECT ----------------------------------- */
    for (;;) {
        FD_SET(listenfd, &rset);
        FD_SET(udpfd, &rset);

        if ((nready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0) {
            if (errno == EINTR)
                continue;
            else {
                perror("select");
                exit(8);
            }
        }

        /* GESTIONE RICHIESTE DI GET DI UN FILE ------------------------------------- */
        if (FD_ISSET(listenfd, &rset)) {
            printf("Ricevuta richiesta eliminazione di parola da un file\n");
            len = sizeof(struct sockaddr_in);
            if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len)) < 0) {
                if (errno == EINTR)
                    continue;
                else {
                    perror("accept");
                    exit(9);
                }
            }

            if (fork() == 0) { /* processo figlio che serve la richiesta di operazione */
                close(listenfd);
                
                if ( read(connfd,nome_dir,LENGTH_FILE_NAME) < 0 ) {
                    perror("nomedir");
                    exit(2);
                }

                printf("Ricevuta directory: %s\n", nome_dir);
                close(1);
                dup(connfd);
                
                execlp("ls","ls",nome_dir,(char *)NULL);
                
                close(connfd);
                exit(2);
            }
            close(connfd);
        }

        /* GESTIONE RICHIESTE DI CONTEGGIO ------------------------------------------ */
        if (FD_ISSET(udpfd, &rset)) {
            
            if (fork() == 0) { /* processo figlio che serve la richiesta di operazione */
                printf("UDP\n");

                len = sizeof(struct sockaddr_in);
                if (recvfrom(udpfd, req, sizeof(Request), 0, (struct sockaddr *)&cliaddr, &len) < 0) {
                    perror("recvfrom");
                    continue;
                }

            
                fd_file = open(req->nome_file, O_RDONLY);
                fd_temp = open("._temp", O_WRONLY | O_CREAT , 0777 );

                printf("Richiesto eliminazione dal file %s della parola %s\n", req->nome_file, req->parola);
                strcpy(parola,req->parola);
                
                if ( fd_file < 0 || fd_temp < 0) {
                    ris = -1;
                    if (sendto(udpfd, &ris, sizeof(ris), 0, (struct sockaddr *)&cliaddr, len) < 0) {
                        perror("sendto");
                        continue;
                    }
                } else {
                    num = 0;
                    erased = 0;

                    while((nread = read(fd_file, &chr, sizeof(char))) > 0) {
                        if ( chr == ' ' || chr == '\n') {
                            buff[num] = '\0';
                            if ( strcmp(buff,parola) != 0 ) {
                                buff[num++] = chr;
                                buff[num] = '\0';

                                if ((nwrite = write(fd_temp, buff, strlen(buff))) < 0) {
                                    perror("write");
                                    break;
                                }
                            } else {
                                erased++;
                            }
                            num = 0;
                        } else {
                            buff[num++] = chr;
                        }
                    }

                    buff[num] = '\0';
                    if ( strcmp(buff,parola) != 0 ) {
                        buff[num++] = chr;
                        buff[num] = '\0';

                        if ((nwrite = write(fd_temp, buff, strlen(buff))) < 0) {
                            perror("write");
                            break;
                        }
                    } else {
                        erased++;
                    }

                    if (sendto(udpfd, &erased, sizeof(erased), 0, (struct sockaddr *)&cliaddr, len) < 0) {
                        perror("sendto");
                        continue;
                    }

                    close(fd_file);
                    close(fd_temp);
                    execlp("mv","mv", "._temp", req->nome_file, (char *)NULL );
                    perror("fork\n");
                    exit(2);
                }
            }
        }

    }

    exit(0);
} 
