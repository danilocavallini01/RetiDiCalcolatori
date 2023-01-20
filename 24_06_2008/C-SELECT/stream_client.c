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

    char nome_file[STRING_LENGTH], parola[STRING_LENGTH];
    int nome_file_length, parola_length, parole_trovate, riga_length;
    char c;

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
    printf("Inserire nome del file, EOF per terminare: ");
    while (gets(nome_file))
    {
        printf("Invio il nome del file %s\n", nome_file);

        // INVIO LUNGHEZZA NOME FILE
        nome_file_length = htonl(strlen(nome_file) + 1);
        if (write(sd, &nome_file_length, sizeof(nome_file_length)) < 0)
        {
            perror("write");
            printf("Inserire il tipo di prenotazione: ");
            continue;
        }

        // INVIO NOME FILE
        if (write(sd, nome_file, strlen(nome_file) + 1) < 0)
        {
            perror("write");
            printf("Inserire il tipo di prenotazione: ");
            continue;
        }

        printf("Inserire parola da cercare: ");

        if ( !gets(parola) ) {
            perror("gets");
            printf("Inserire il tipo di prenotazione: ");
            continue;
        }

        parola_length = htonl(strlen(parola) + 1);
        // INVIO LUNGHEZZA PAROLA
        if (write(sd, &parola_length, sizeof(parola_length)) < 0)
        {
            perror("write");
            printf("Inserire il tipo di prenotazione: ");
            continue;
        }

         // INVIO PAROLA
        if (write(sd, parola, strlen(parola) + 1) < 0)
        {
            perror("write");
            printf("Inserire il tipo di prenotazione: ");
            continue;
        }

        // LEGGO PAROLE TROVATE
        if (read(sd,&parole_trovate,sizeof(parole_trovate)) < 0) {
            perror("read");
            printf("Inserire il tipo di prenotazione: ");
            continue;
        }

        parole_trovate = ntohl(parole_trovate);

        // LOOP SULLE RIGHE TROVATE
        for ( int i = 0; i < parole_trovate; i++ ) {

            // LEGGO LUNGHEZZA RIGA
            if (read(sd,&riga_length,sizeof(riga_length)) < 0) {
                perror("read");
                printf("Inserire il tipo di prenotazione: ");
                continue;
            }

            riga_length = ntohl(riga_length);
            //LEGGO RIGA
            for ( int i = 0; i < riga_length; i++ ) {
                if ( read(sd, &c, sizeof(c)) < 0 ) {
                    perror("read");
                    printf("Inserire il tipo di prenotazione: ");
                    continue;
                }

                printf("%c", c);
            }
            printf("\n");
        }

        printf("Inserire il tipo di prenotazione: ");
    }
    /* Chiusura socket in ricezione */
    close(sd);
    printf("\nClient: termino...\n");
    exit(0);
}