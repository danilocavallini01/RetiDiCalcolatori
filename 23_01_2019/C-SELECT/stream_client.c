#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define TARGA_LENGTH 8
#define PATENTE_LENGTH 6
#define NAME_LENGTH 255

int main(int argc, char *argv[])
{
    int port, nread, sd, nwrite;
    struct hostent *host;
    struct sockaddr_in servaddr;

    char targa[TARGA_LENGTH], buffChar, file_name[NAME_LENGTH], buff[NAME_LENGTH];
    int file_count, image_fd, file_length, offset, file_name_length;

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
    printf("Inserire nome della targa, EOF per terminare: ");
    while (gets(targa))
    {
        printf("Invio il nome della targa: %s\n", targa);

        // invio targa
        if (write(sd, targa, strlen(targa) + 1) < 0)
        {
            perror("write");
            printf("Nome della targa, EOF per terminare: ");
            continue;
        }

        // Lettura risposta dal server

        if (read(sd, &buffChar, sizeof(char)) < 0)
        {
            perror("read");
            printf("Nome della targa, EOF per terminare: ");
            continue;
        }

        if (buffChar == 'S')
        {
            // Ricevo numero file
            if (read(sd, &file_count, sizeof(file_count)) < 0)
            {
                perror("read");
                printf("Nome della targa, EOF per terminare: ");
                continue;
            }
            printf("ricevuto numero file : %i\n", file_count);

            for (int i = 0; i < file_count; i++)
            {
                // ric lunghzza nome file
                if (read(sd, &file_name_length, sizeof(file_name_length)) < 0)
                {
                    perror("read");
                    printf("Nome della targa, EOF per terminare: ");
                    continue;
                }

                
                file_name_length = ntohl(file_name_length);

                // ric nome file
                if (read(sd, &file_name, file_name_length) < 0)
                {
                    perror("read");
                    printf("Nome della targa, EOF per terminare: ");
                    continue;
                }

                image_fd = open(file_name, O_WRONLY | O_CREAT, 0777);

                // ric lunghezza file 
                if (read(sd, &file_length, sizeof(file_length)) < 0)
                {
                    perror("read");
                    printf("Nome della targa, EOF per terminare: ");
                    continue;
                }
                
                file_length = ntohl(file_length);
                printf("Ricevuta lunghezza file : %i\n", file_length);

                //ric file
                while (file_length > 0)
                {
                    if ( sizeof(buff) < file_length ) {
                        nread = read(sd, buff, sizeof(buff));
                    } else {
                        nread = read(sd, buff, file_length);
                    }
                    
                    if (nread <= 0) break;

                    printf("%s\nnread: %i\n", buff, nread);
                    write(image_fd, buff, nread);

                    file_length -= nread;
                }

                close(image_fd);
            }
        }
        else
        {
            printf("directory non presente sul server\n");
        }
        printf("Nome della targa, EOF per terminare: ");
    }
    /* Chiusura socket in ricezione */
    close(sd);
    printf("\nClient: termino...\n");
    exit(0);
}