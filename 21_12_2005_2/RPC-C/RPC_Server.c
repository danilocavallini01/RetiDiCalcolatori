#include "RPCxFile.h"
#include <rpc/rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Riga table[N];
static int inizializzato = 0;

void inizializza() {
    if ( inizializzato == 1 ) return;
    
    inizializzato = 1;

    for ( int i = 0; i < N; i++ ) {
        strcpy(table[i].nome,"L");
        strcpy(table[i].stato,"L");
        for ( int j = 0; j < K; j++ ) {
            strcpy(table[i].utenti[j].utente,"L");
        }
    }

    strcpy(table[0].nome,"Stanza 1");
    strcpy(table[0].stato,"P");
    stpcpy(table[0].utenti[0].utente,"Pippo");
    stpcpy(table[0].utenti[1].utente,"Pluto");

    strcpy(table[1].nome,"Stanza 2");
    strcpy(table[1].stato,"M");
    stpcpy(table[1].utenti[0].utente,"Paperino");
    stpcpy(table[1].utenti[1].utente,"Minny");
}

int *aggiungi_stanza_1_svc(Richiesta *request, struct svc_req *reqstp) {

    static int res;

    if ( inizializzato == 0 ) {
        inizializza();
    }

    for ( int i = 0; i < N; i++ ) {
        if ( strcmp(table[i].nome,"L") == 0 ) {

            strcpy(table[i].nome,  request->nome);
            strcpy(table[i].stato, request->stato);

            res = 0;
            return (&res);

        } else if ( strcmp(table[i].nome,request->nome) == 0 ) {
            res = -1;
            return (&res);
        }
    }

    return (&res);
}

Risultato *elimina_utente_1_svc(char **utente, struct svc_req *reqstp) {

    static Risultato res;
    int ind = 0;

    if ( inizializzato == 0 ) {
        inizializza();
    }

    for ( int i = 0; i < N; i++ ) {
        for ( int j = 0; j < K; j++ ) {
            if ( strcmp(table[i].utenti[j].utente,*utente) == 0 ) {
                
                // COPIA RECORD
                strcpy(res.ris[ind].nome,table[i].nome);
                strcpy(res.ris[ind].stato,table[i].stato);
                for ( j = 0; j < K; j++ ) {
                    strcpy(res.ris[ind].utenti[j].utente, table[i].utenti[j].utente);
                }
                
                // ELIMINA RECORD
                strcpy(table[i].nome,"L");
                strcpy(table[i].stato,"L");
                for ( j = 0; j < K; j++ ) {
                    strcpy(table[i].utenti[j].utente,"L");
                }

                ind++;

                break;
            }
        }
    } 

    res.size = ind;

    if ( ind == 0 ) {
        res.success = -1;
    } else {
        res.success = 0;
    }

    return (&res);
}