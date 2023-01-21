#include "RPCxFile.h"
#include <rpc/rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char   *host; // nome host
    CLIENT *cl;   // gestore del trasporto

    char ok[2], *nomefile, buffer[50];
    int *ris;
    Request request;
    Response * res;

    nomefile = (char *)malloc(FILE_LENGTH);

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
    printf("Inserire:\n1\tElimina occorrenze\n2\tLista file di testo\n^D\tper terminare: ");

    while (gets(ok)) {

        // 1
        if (strcmp(ok, "1") == 0) {

            printf("\nInserisci nome del file: ");
            gets(nomefile);

            // Invocazione remota
            ris = elimina_occorrenze_1(&nomefile, cl);
            // Controllo del risultato
            if (ris == NULL) {
                // Errore di RPC
                clnt_perror(cl, host);
                exit(1);
            }

            if (*ris == -1) {
                printf("Errore\n");
            } else if (*ris >= 0) {
                printf("Sono state eliminate %i occorrenze nel file\n", *ris);
            }

        }
        // 2
        else if (strcmp(ok, "2") == 0)
        {
            printf("\nInserisci nome del direttorio: ");
            gets(request.nomedir);

            printf("\nInserisci carattere da trovare: ");
            while(scanf("%c",&request.chr ) != 1) {
                while ( getchar() != '\n' );
                printf("\nInserisci carattere da trovare: ");
            }
            gets(buffer);

            printf("\nInserisci occorrenze da trovare: ");
            while(scanf("%i",&request.noccorrenze ) != 1) {
                while ( getchar() != '\n' );
                printf("\nInserisci occorrenze da trovare: ");
            }
            gets(buffer);

            // Invocazione remota
            res = lista_file_carattere_1(&request, cl);
            
            // Controllo del risultato
            if (res == NULL) {
                // Errore di RPC
                clnt_perror(cl, host);
                exit(1);
            }

            if (res->error == 1) {
                printf("Errore\n");
            } else {
                printf("File trovati\n");
                for ( int i = 0; i < res->size; i++ ) {
                    printf("%s\n", res->files[i].nomefile);
                }
            }

        } else {
            printf("Operazione richiesta non disponibile!!\n");
        }

        printf("Inserire:\n1\tElimina occorrenze\n2\tLista file di testo\n^D\tper terminare: ");
    } // while

    free(nomefile);
    // Libero le risorse, distruggendo il gestore di trasporto
    clnt_destroy(cl);
    exit(0);
} // main
