#include "RPCxFile.h"
#include <rpc/rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Inserimento table[N];
static int inizializzato = 0;

void debug_print() {

    for ( int i = 0; i < N; i++ ) {
        printf("%s,%s,%s,%s,%i,%i\n", table[i].descrizione,table[i].tipo,table[i].date,
            table[i].luogo,table[i].disponibilita,table[i].prezzo);
    }

}
void inizializza() {
    if ( inizializzato == 1 ) return;

    for ( int i = 0; i < N; i++ ) {
        strcpy(table[i].descrizione,"L");
        strcpy(table[i].tipo,"L");
        strcpy(table[i].date, "L");
        strcpy(table[i].luogo,"L");
        table[i].disponibilita = -1;
        table[i].prezzo = -1;
    }

    inizializzato = 1;
}

int *inserimento_evento_1_svc(Inserimento *evento,  struct svc_req *reqstp ) {

    static int res;

    res = -1;

    inizializza();

    // CONTROLLO CHE NON SIA NULL STRUCT
    if ( evento == NULL) {
        return (&res);
    }

    // CONTROLLO CHE CI SIA LO STESSO ID
    for ( int i = 0; i < N; i++ ) {
        if ( strcmp(table[i].descrizione, evento->descrizione) == 0 ) {
            return (&res);
        }
    }

    // AGGIUNGO EVENTO
    for ( int i = 0; i < N; i++ ) {
        if ( strcmp(table[i].descrizione,"L") == 0 ) {
            strcpy(table[i].descrizione, evento->descrizione);
            strcpy(table[i].tipo, evento->tipo);
            strcpy(table[i].date, evento->date);
            strcpy(table[i].luogo, evento->luogo);
            table[i].disponibilita = evento->disponibilita;
            table[i].prezzo = evento->prezzo;

            res = 0;
            break;
        }
    }

    debug_print();

    return (&res);
}

int *acquista_biglietti_1_svc(Richiesta *richiesta, struct svc_req *reqstp) {

    static int res;

    res = -1;

    inizializza();

    // CONTROLLO CHE NON SIA NULL
    if ( richiesta == NULL ) {
        return (&res);
    }

    // SOTTRAI NUM BIGLIETTI SE DISPONIBILI
    for ( int i = 0; i < N; i++ ) {
        if ( strcmp(richiesta->descrizione, table[i].descrizione) == 0) {

            if ( table[i].disponibilita >= richiesta->num_biglietti ) {
                table[i].disponibilita -= richiesta->num_biglietti;
                
                res = 0;
            }

            break;
        }
    } 

    debug_print();

    return (&res);
}