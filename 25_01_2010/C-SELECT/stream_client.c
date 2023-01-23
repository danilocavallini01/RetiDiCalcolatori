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

    int N, gg, mm, aa, found, title_length;
    char buffer[50], risp, title[MAX_TITOLO];
    
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
    printf("Inserire il numero di giorni (N), EOF per terminare: ");
    while (scanf("%i",&N) == 1)
    {
        gets(buffer);

        // INVIO N
        N = htonl(N);   
        if (write(sd, &N, sizeof(N)) < 0)
        {
            perror("write");
            printf("Inserire il tipo di prenotazione: ");
            continue;
        }

        // LEGGO RISP
        if (read(sd, &risp, sizeof(risp)) < 0)
        {
            perror("read");
            printf("Inserire il tipo di prenotazione: ");
            continue;
        }

        if ( risp == 'N' ) {
            printf("N invalido\n");
            continue;
        }

        printf("\nInserisci giorno: ");
        while(scanf("%i",&gg ) != 1) {
            while ( getchar() != '\n' );
            printf("\nInserisci giorno: ");
        }
        gets(buffer);

        printf("\nInserisci mese: ");
        while(scanf("%i",&mm ) != 1) {
            while ( getchar() != '\n' );
            printf("\nInserisci mese: ");
        }
        gets(buffer);

        printf("\nInserisci anno: ");
        while(scanf("%i",&aa ) != 1) {
            while ( getchar() != '\n' );
            printf("\nInserisci anno: ");
        }
        gets(buffer);

        // INVIO GIORNO
        gg = htonl(gg);   
        if (write(sd, &gg, sizeof(gg)) < 0)
        {
            perror("write");
            printf("Inserire il tipo di prenotazione: ");
            continue;
        }

        // INVIO MESE
        mm = htonl(mm);   
        if (write(sd, &mm, sizeof(mm)) < 0)
        {
            perror("write");
            printf("Inserire il tipo di prenotazione: ");
            continue;
        }

        // INVIO ANNO
        aa = htonl(aa);   
        if (write(sd, &aa, sizeof(aa)) < 0)
        {
            perror("write");
            printf("Inserire il tipo di prenotazione: ");
            continue;
        }

        // LEGGO NUMERO DI TITOLI TROVATI
        if (read(sd, &found, sizeof(found)) < 0)
        {
            perror("read");
            printf("Inserire il tipo di prenotazione: ");
            continue;
        }

        found = ntohl(found);

        for ( int i = 0; i < found; i++ ) {
             // LEGGO LUNGHEZZA TITOLO
            if (read(sd,&title_length,sizeof(title_length)) < 0) {
                perror("read");
                printf("Inserire il tipo di prenotazione: ");
                continue;
            }

            title_length = ntohl(title_length);
            
            //LEGGO RIGA
            if ( read(sd, title, title_length) < 0 ) {
                perror("read");
                printf("Inserire il tipo di prenotazione: ");
                continue;
            }

            printf("%s\n", title);
        }

        printf("Inserire il numero di giorni (N), EOF per terminare: ");
    }
    /* Chiusura socket in ricezione */
    close(sd);
    printf("\nClient: termino...\n");
    exit(0);
}