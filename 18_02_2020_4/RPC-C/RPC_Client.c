#include "RPCxFile.h"
#include <rpc/rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char   *host; // nome host
    CLIENT *cl;   // gestore del trasporto

    char ok[2], buffer[50];
    int voto;

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
    printf("Inserire:\n1\tVisuallizzaSottoVotto\n2\tEsprimiVoto\n^D\tper terminare: ");

    while (gets(ok)) {

        // 1
        if (strcmp(ok, "1") == 0) {

            printf("\nInserisci voto: ");
            while(scanf("%i",&voto) != 1) {
                while ( getchar() != '\n' );
                printf("\nInserisci voto: ");
            }
            gets(buffer);

            // Invocazione remota
            res = visualizza_canzoni_conmenodi_v_voti_1(&voto, cl);
            // Controllo del risultato
            if (res == NULL) {
                // Errore di RPC
                clnt_perror(cl, host);
                exit(1);
            }

            printf("Canzoni trovate\n");
            for ( int i = 0; i < res->size; i++ ) {
                 printf("%s,%s,%i,%s\n", (*res).canzoni[i].cantante,(*res).canzoni[i].titolo,
                 (*res).canzoni[i].voti,(*res).canzoni[i].nomefile);
            }
        }
        // 2
        else if (strcmp(ok, "2") == 0)
        {
            do {
                printf("\nVuoi esprimere voto per (T=Titolo,C=Cantante): ");
                gets(buffer);
            } while ( buffer[0] != 'C' && buffer[0] != 'T');

            if ( buffer[0] == 'C' ) {
                req.isCantante = 1;
                printf("Inserisci cantante: ");
                gets(req.cantante);
            } else {
                req.isCantante = 0;
                printf("Inserisci titolo: ");
                gets(req.titolo);
            }

            // Invocazione remota
            ris = esprimi_voto_1(&req, cl);
            
            // Controllo del risultato
            if (ris == NULL) {
                // Errore di RPC
                clnt_perror(cl, host);
                exit(1);
            }

            if (*ris == -1) {
                printf("Errore durante esprimi voto\n");
            } else if (*ris == 0) {
                printf("Voto espresso correttamente\n");
            }

        } else {
            printf("Operazione richiesta non disponibile!!\n");
        }

         printf("Inserire:\n1\tVisuallizzaSottoVotto\n2\tEsprimiVoto\n^D\tper terminare: ");
    } // while

    // Libero le risorse, distruggendo il gestore di trasporto
    clnt_destroy(cl);
    exit(0);
} // main
