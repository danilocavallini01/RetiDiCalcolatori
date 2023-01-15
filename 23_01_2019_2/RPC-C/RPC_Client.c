/*
 * xfactor_c.c
 */

#include "RPCxFile.h"
#include <rpc/rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char   *host; // nome host
    CLIENT *cl;   // gestore del trasporto

    char ok[2], nl[2], *tipoveicolo;
    Input input;
    Output *righe;
    bool_t *ris;

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
    printf("Inserire:\n1\tVisualizza Prenotazioni\n2\tAggiorna Patente\n^D\tper terminare: ");

    while (scanf("%s", ok) == 1) {
	// Consuma fine linea
	    gets(&nl);

        /********* 1 - Classifica Giudici *********/
        if (strcmp(ok, "1") == 0) {

            printf("\nInserisci tipoveicolo della macchina: ");
            gets(tipoveicolo);

            // Invocazione remota
            righe = visualizza_prenotazioni_1(&tipoveicolo, cl);
            // Controllo del risultato
            if (righe == NULL) {
                // Errore di RPC
                clnt_perror(cl, host);
                exit(1);
            }

            printf("Record trovati:\n");

            for (int i = 0; i < MAX_RIS; i++) {
                if ( strcmp(righe->ris[i].targa,"L") != 0 ) {
                    printf("%s|%s|%s|%s\n",righe->ris[i].targa,righe->ris[i].patente,righe->ris[i].tipoVeicolo,righe->ris[i].immagini);
                }
            }

        }
        /********* 2 - Inserisci Voto *********/
        else if (strcmp(ok, "2") == 0)
        {
            printf("\nInserisci targa della macchina: ");
            gets(input.targa);

            printf("\nInserisci nuova patente della macchina: ");
            gets(input.patente);

            // Invocazione remota
            ris = aggiorna_patente_1(&input, cl);

            // Controllo del risultato
            if (ris == NULL) {
                // Errore di RPC
                clnt_perror(cl, host);
                exit(1);
            }

            if (*ris == FALSE) {
                printf("Problemi nell'aggiornamento patente\n");
            } else if (*ris == TRUE) {
                printf("Patente aggiornata con successo\n");
            }
        } else {
            printf("Operazione richiesta non disponibile!!\n");
        }

        printf("Inserire:\n1\tVisualizza Prenotazioni\n2\tAggiorna Patente\n^D\tper terminare: ");
    } // while

    // Libero le risorse, distruggendo il gestore di trasporto
    clnt_destroy(cl);
    exit(0);
} // main
