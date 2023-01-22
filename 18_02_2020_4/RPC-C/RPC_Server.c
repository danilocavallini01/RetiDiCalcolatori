#include "RPCxFile.h"
#include <rpc/rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Canzone table[N];
static int inizializzato = 0;

void debug_print() {

    for ( int i = 0; i < N; i++ ) {
        printf("%s,%s,%i,%s\n", table[i].cantante,table[i].titolo,table[i].voti,
            table[i].nomefile);
    }

}

void inizializza() {
    if ( inizializzato == 1 ) return;

    for ( int i = 0; i < N; i++ ) {
        strcpy(table[i].cantante,"L");
        strcpy(table[i].titolo,"L");
        strcpy(table[i].nomefile, "L");
        table[i].voti = -1;
    }

    strcpy(table[0].cantante,"Brasco");
    strcpy(table[0].titolo,"IoOdioGiovedi");
    strcpy(table[0].nomefile, "odioGiovedi.txt");
    table[0].voti = 100;

    strcpy(table[1].cantante,"Viga");
    strcpy(table[1].titolo,"Bar Da Alfredo");
    strcpy(table[1].nomefile, "alfred.txt");
    table[1].voti = 50;

    inizializzato = 1;
}

Response * visualizza_canzoni_conmenodi_v_voti_1_svc(int *voto, struct svc_req *reqstp) {

    static Response res;

    inizializza();

    res.size = 0;
    if ( voto == NULL ) {
        return NULL;
    }

    for ( int i = 0; i < N; i++ ) {
        if ( table[i].voti != -1 && table[i].voti < *voto ) {
            strcpy(res.canzoni[res.size].cantante,table[i].cantante);
            strcpy(res.canzoni[res.size].titolo,table[i].titolo);
            strcpy(res.canzoni[res.size].nomefile,table[i].nomefile);
            res.canzoni[res.size].voti = table[i].voti;
            res.size++;

            if ( res.size == 6 ) {
                break;
            }
        }
    }

    return (&res);
}

int * esprimi_voto_1_svc(Request *richiesta, struct svc_req *reqstp) {

    static int res;

    inizializza();

    res = -1;

    if ( richiesta == NULL) {
        return (&res);
    }

    if ( richiesta->isCantante == 1 ) {
        
        for ( int i = 0; i < N; i++ ) {
            if ( strcmp(richiesta->cantante,table[i].cantante) == 0 ) {
                table[i].voti++;
                res = 0;
                break;
            }
        }
    } else {
        for ( int i = 0; i < N; i++ ) {
            if ( strcmp(richiesta->titolo,table[i].titolo) == 0 ) {
                table[i].voti++;
                res = 0;
                break;
            }
        }
    }

    debug_print();

    return (&res);
}
