#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "select_header.h"

int main(int argc, char *argv[])
{
    int port, nread, sd, nwrite;
    struct hostent *host;
    struct sockaddr_in servaddr;

    char tipo_prenotazione[TPREN_LENGTH], file_name[IMMAG_LENGTH], buff[STRING_LENGTH];
    int tipo_pren_length, num_prenotazioni, file_name_length, file_length;
    int image_fd;

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
    printf("Inserire tipo di prenotazione, EOF per terminare: ");
    while (gets(tipo_prenotazione))
    {
        printf("Invio il tipo prenotazione: %s\n", tipo_prenotazione);

        // INVIO LUNGHEZZA TIPO PRENOTAZIONE
        tipo_pren_length = htonl(strlen(tipo_prenotazione) + 1);
        if (write(sd, &tipo_pren_length, sizeof(tipo_pren_length)) < 0)
        {
            perror("write");
            printf("Inserire il tipo di prenotazione: ");
            continue;
        }

        // INVIO TIPO PRENOTAZIONE
        if (write(sd, tipo_prenotazione, strlen(tipo_prenotazione) + 1) < 0)
        {
            perror("write");
            printf("Inserire il tipo di prenotazione: ");
            continue;
        }

        // LEGGO NUMERO PRENOTAZIONI TROVATE
        if (read(sd, &num_prenotazioni, sizeof(num_prenotazioni)) < 0)
        {
            perror("read");
            printf("Inserire il tipo di prenotazione: ");
            continue;
        }

        num_prenotazioni = ntohl(num_prenotazioni);

        printf("Ricevuto numero prenotazioni: %i\n", num_prenotazioni);

        for (int i = 0; i < num_prenotazioni; i++)
        {
            // RICEVO LUNGHEZZA NOME FILE
            if (read(sd, &file_name_length, sizeof(file_name_length)) < 0)
            {
                perror("read");
                printf("Inserire il tipo di prenotazione: ");
                continue;
            }

            
            file_name_length = ntohl(file_name_length);

            // RICEVO NOME DEL FILE
            if (read(sd, file_name, file_name_length) < 0)
            {
                perror("read");
                printf("Inserire il tipo di prenotazione: ");
                continue;
            }

            image_fd = open(file_name, O_WRONLY | O_CREAT, 0777);

            // RICEVO LUNGHEZZA FILE
            if (read(sd, &file_length, sizeof(file_length)) < 0)
            {
                perror("read");
                printf("Inserire il tipo di prenotazione: ");
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
                    perror("nread");
                    printf("Inserire il tipo di prenotazione: ");
                    continue;
                }

                write(image_fd, buff, nread);

                file_length -= nread;
            }

            close(image_fd);
        }

        printf("Inserire il tipo di prenotazione: ");
    }
    /* Chiusura socket in ricezione */
    close(sd);
    printf("\nClient: termino...\n");
    exit(0);
}