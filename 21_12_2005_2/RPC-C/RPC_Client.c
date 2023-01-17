#include "RPCxFile.h"
#include <rpc/rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char   *host; // nome host
    CLIENT *cl;   // gestore del trasporto

    char ok[2], *nome;
    Richiesta request;
    int *ris;
    Risultato *response;

    nome = (char *)malloc(MAX_NOME);
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
    printf("Inserire:\n1\tAggiungi stanza\n2\tElimina utente\n^D\tper terminare: ");

    while (gets(ok)) {

        // 1
        if (strcmp(ok, "1") == 0) {

            printf("\nInserisci nome della stanza: ");
            gets(request.nome);

            printf("\nInserisci il tipo di comunicazione: ");
            gets(request.stato);

            // Invocazione remota
            ris = aggiungi_stanza_1(&request, cl);
            // Controllo del risultato
            if (ris == (int *)NULL) {
                // Errore di RPC
                clnt_perror(cl, host);
                exit(1);
            }

            if (*ris == -1) {
                printf("Errore durante inserimento stanza\n");
            } else if (*ris == 0) {
                printf("Stanza aggiunta corretamente\n");
            }

        }
        // 2
        else if (strcmp(ok, "2") == 0)
        {
            printf("\nInserisci nome da eliminare: ");
            gets(nome);

            // Invocazione remota
            response = elimina_utente_1(&nome, cl);
            
            // Controllo del risultato
            if (response == (Risultato *)NULL) {
                // Errore di RPC
                clnt_perror(cl, host);
                exit(1);
            }

            if (response->success == 0) {
                printf("Stampo lista di righe rimosse\n");
                for ( int i = 0; i < response->size; i++ ) {
                    printf("%s|%s",response->ris[i].nome,response->ris[i].stato);
                    for ( int j = 0; j < K; j++) {
                        printf("|%s", response->ris[i].utenti[j].utente);
                    }
                    printf("\n");
                }
            } else {
                printf("Errore durante eliminazione utente\n");
            }
        } else {
            printf("Operazione richiesta non disponibile!!\n");
        }

        printf("Inserire:\n1\tAggiungi stanza\n2\tElimina utente\n^D\tper terminare: ");
    } // while

    free(nome);
    // Libero le risorse, distruggendo il gestore di trasporto
    clnt_destroy(cl);
    exit(0);
} // main
