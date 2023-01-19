#include "RPCxFile.h"
#include <rpc/rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char   *host; // nome host
    CLIENT *cl;   // gestore del trasporto

    char ok[2], buffer[50];
    Richiesta request;
    Inserimento event;

    int *ris;

    // Controllo degli argomenti
    if (argc != 2) {
        printf("usage: %s server_host\n", argv[0]);
        exit(1);
    }
    host = argv[1];

    // Creazione gestore del trasporto
    cl = clnt_create(host, OPERATION, OPERATIONVERS, "udp");
    if (cl == NULL) {
        clnt_pcreateerror(host);
        exit(1);
    }

    // Interazione con l'utente
    printf("Inserire:\n1\tInserisci evento\n2\tAcquista biglietto\n^D\tper terminare: ");

    while (gets(ok)) {

        // 1
        if (strcmp(ok, "1") == 0) {

            printf("\nInserisci descrizione evento: ");
            gets(event.descrizione);

            printf("\nInserisci tipo evento: ");
            gets(event.tipo);

            printf("\nInserisci data evento: ");
            gets(event.date);

            printf("\nInserisci luogo evento: ");
            gets(event.luogo);

            printf("\nInserisci disponibilita' evento: ");
            while(scanf("%i",&event.disponibilita) != 1) {
                while ( getchar() != '\n' );
                printf("\nInserisci disponibilita' evento: ");
            }
            gets(buffer);

            printf("\nInserisci prezzo' evento: ");
            while(scanf("%i",&event.prezzo) != 1) {
                while ( getchar() != '\n' );
                printf("\nInserisci prezzo' evento: ");
            }
            gets(buffer);

            // Invocazione remota
            ris = inserimento_evento_1(&event, cl);
            // Controllo del risultato
            if (ris == (int *)NULL) {
                // Errore di RPC
                clnt_perror(cl, host);
                exit(1);
            }

            if (*ris == -1) {
                printf("Errore durante inserimento evento\n");
            } else if (*ris == 0) {
                printf("Evento aggiunto corretamente\n");
            }

        }
        // 2
        else if (strcmp(ok, "2") == 0)
        {
            printf("\nInserisci descrizione evento: ");
            gets(request.descrizione);

            printf("\nInserisci biglietti da acquistare: ");
            while(scanf("%i",&request.num_biglietti ) != 1) {
                while ( getchar() != '\n' );
                printf("\nInserisci biglietti da acquistare: ");
            }
            gets(buffer);

            // Invocazione remota
            ris = acquista_biglietti_1(&request, cl);
            
            // Controllo del risultato
            if (ris == (int *)NULL) {
                // Errore di RPC
                clnt_perror(cl, host);
                exit(1);
            }

            if (*ris == -1) {
                printf("Errore durante acquisto biglietti\n");
            } else if (*ris == 0) {
                printf("Biglietti acquistati corretamente\n");
            }

        } else {
            printf("Operazione richiesta non disponibile!!\n");
        }

        printf("Inserire:\n1\tInserisci evento\n2\tAcquista biglietto\n^D\tper terminare: ");
    } // while

    // Libero le risorse, distruggendo il gestore di trasporto
    clnt_destroy(cl);
    exit(0);
} // main
