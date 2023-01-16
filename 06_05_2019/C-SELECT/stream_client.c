#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define STRING_LENGTH 50

int main(int argc, char *argv[])
{
    int port, nread, sd, nwrite;
    struct hostent *host;
    struct sockaddr_in servaddr;

    char dir[STRING_LENGTH], risp, file_name[STRING_LENGTH], buff[STRING_LENGTH];
    int dir_length, file_count, file_name_length, fd, file_length;

    /* CONTROLLO ARGOMENTI ---------------------------------- */
    if (argc != 3)
    {
        printf("Error:%s serverAddress serverPort\n", argv[0]);
        exit(1);
    }

    /* INIZIALIZZAZIONE INDIRIZZO SERVER -------------------------- */
    memset((char *)&servaddr, 0, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    host = gethostbyname(argv[1]);
    if (host == NULL)
    {
        printf("%s not found in /etc/hosts\n", argv[1]);
        exit(1);
    }

    nread = 0;
    while (argv[2][nread] != '\0')
    {
        if ((argv[2][nread] < '0') || (argv[2][nread] > '9'))
        {
            printf("Secondo argomento non intero\n");
            exit(2);
        }
        nread++;
    }

    port = atoi(argv[2]);
    if (port < 1024 || port > 65535)
    {
        printf("Porta scorretta...");
        exit(2);
    }

    servaddr.sin_addr.s_addr = ((struct in_addr *)(host->h_addr))->s_addr;
    servaddr.sin_port = htons(port);

    /* CREAZIONE SOCKET ------------------------------------ */
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0)
    {
        perror("apertura socket");
        exit(3);
    }
    printf("Client: creata la socket sd=%d\n", sd);

    /* Operazione di BIND implicita nella connect */
    if (connect(sd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in)) < 0)
    {
        perror("connect");
        exit(3);
    }
    printf("Client: connect ok\n");

    /* CORPO DEL CLIENT:
     ciclo di accettazione di richieste da utente ------- */
    printf("Inserire directory da scaricare, EOF per terminare: ");
    while (gets(dir))
    {
        printf("Invio nome della directory: %s\n", dir);

        // INVIO LUNGHEZZA NOME DIR
        dir_length = htonl(strlen(dir) + 1);
        if (write(sd, &dir_length, sizeof(dir_length)) < 0)
        {
            perror("write");
            printf("Inserire directory da scaricare, EOF per terminare: ");
            continue;
        }

        // INVIO NOME DIR
        if (write(sd, dir, strlen(dir) + 1) < 0)
        {
            perror("write");
            printf("Inserire directory da scaricare, EOF per terminare: ");
            continue;
        }

        // LEGGO RISPOSTA DAL SERVER
        if (read(sd, &risp, sizeof(char)) < 0)
        {
            perror("read");
            printf("Inserire directory da scaricare, EOF per terminare: ");
            continue;
        }

        if (risp == 'S')
        {
            // LEGGO NUMERO FILE
            if (read(sd, &file_count, sizeof(file_count)) < 0)
            {
                perror("read");
                printf("Inserire directory da scaricare, EOF per terminare: ");
                continue;
            }
            file_count = ntohl(file_count);
            printf("ricevuto numero file : %i\n", file_count);

            for (int i = 0; i < file_count; i++)
            {
                // LEGGO LUNGHEZZA NOME FILE
                if (read(sd, &file_name_length, sizeof(file_name_length)) < 0)
                {
                    perror("read");
                    printf("Inserire directory da scaricare, EOF per terminare: ");
                    continue;
                }
                file_name_length = ntohl(file_name_length);

                // LEGGO NOME FILE
                if (read(sd, &file_name, file_name_length) < 0)
                {
                    perror("read");
                    printf("Inserire directory da scaricare, EOF per terminare: ");
                    continue;
                }

                fd = open(file_name, O_WRONLY | O_CREAT, 0777);

                if ( fd < 0 ) {
                    perror("open");
                    close(fd);
                    printf("Inserire directory da scaricare, EOF per terminare: ");
                    continue;
                }

                // RICEVO LUNGHEZZA FILE
                if (read(sd, &file_length, sizeof(file_length)) < 0)
                {
                    perror("read");
                    close(fd);
                    printf("Inserire directory da scaricare, EOF per terminare: ");
                    continue;
                }
                
                file_length = ntohl(file_length);
                printf("Ricevuta lunghezza file : %i\n", file_length);

                // RICEVO FILE
                while (file_length > 0)
                {
                    if ( sizeof(buff) < file_length ) {
                        nread = read(sd, buff, sizeof(buff));
                    } else {
                        nread = read(sd, buff, file_length);
                    }
                    
                    if (nread <= 0) {
                        perror("read");
                        close(fd);
                        printf("Inserire directory da scaricare, EOF per terminare: ");
                        continue;
                    }

                    if ( write(fd, buff, nread) < 0 ) {
                        perror("read");
                        close(fd);
                        printf("Inserire directory da scaricare, EOF per terminare: ");
                        continue;
                    }

                    file_length -= nread;
                }

                close(fd);
            }
        }
        else
        {
            printf("directory non presente sul server\n");
        }
        printf("Inserire directory da scaricare, EOF per terminare: ");
    }
    /* Chiusura socket in ricezione */
    close(sd);
    printf("\nClient: termino...\n");
    exit(0);
}