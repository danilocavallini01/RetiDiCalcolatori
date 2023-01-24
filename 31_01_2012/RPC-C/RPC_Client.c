#include "RPCxFile.h"
#include <rpc/rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char   *host; // nome host
    CLIENT *cl;   // gestore del trasporto

    char ok[2], buffer[50];
    char *previsione;
    int voto;

    previsione = (char *)malloc(MAX_PREVI);

    int *ris;
    Response *res;
    Request req;

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
    printf("Inserire:\n1\tVisualizzaDati\n2\tAggiornaTemperature\n^D\tper terminare: ");

    while (gets(ok)) {

        // 1
        if (strcmp(ok, "1") == 0) {

            printf("\nInserisci previsione: ");
            gets(previsione);

            // Invocazione remota
            res = visualizza_dati_1(&previsione, cl);
            // Controllo del risultato
            if (res == NULL) {
                // Errore di RPC
                clnt_perror(cl, host);
                exit(1);
            }

            printf("Previsioni trovate\n");
           
            for ( int i = 0; i < res->size; i++ ) {
                printf("%s,%s,%i,%i,%s\n", res->giorni[i].giorno, res->giorni[i].previsione, 
                res->giorni[i].t_min, res->giorni[i].t_max, res->giorni[i].file_audio);
            }
        }
        // 2
        else if (strcmp(ok, "2") == 0)
        {
            printf("\nInserisci giorno: ");
            gets(req.giorno);

            printf("\nInserisci temp min: ");
            while(scanf("%i",&req.t_min) != 1) {
                while ( getchar() != '\n' );
                printf("\nInserisci temp min: ");
            }
            gets(buffer);

            printf("\nInserisci temp max: ");
            while(scanf("%i",&req.t_max) != 1) {
                while ( getchar() != '\n' );
                printf("\nInserisci temp min: ");
            }
            gets(buffer);

            // Invocazione remota
            ris = aggiorna_temperature_1(&req, cl);
            
            // Controllo del risultato
            if (ris == NULL) {
                // Errore di RPC
                clnt_perror(cl, host);
                exit(1);
            }

            if (*ris == -1) {
                printf("Errore \n");
            } else if (*ris == 0) {
                printf("Voto espresso correttamente\n");
            }

        } else {
            printf("Operazione richiesta non disponibile!!\n");
        }

         printf("Inserire:\n1\tVisualizzaDati\n2\tAggiornaTemperature\n^D\tper terminare: ");
    } // while

    free(previsione);
    // Libero le risorse, distruggendo il gestore di trasporto
    clnt_destroy(cl);
    exit(0);
} // main
