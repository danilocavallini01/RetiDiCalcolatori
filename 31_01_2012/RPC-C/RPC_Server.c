#include "RPCxFile.h"
#include <rpc/rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Giorno table[N];
static int inizializzato = 0;

void debug_print() {
    for ( int i = 0; i < N; i++ ) {
        printf("%s,%s,%i,%i,%s\n", table[i].giorno, table[i].previsione, table[i].t_min, table[i].t_max, table[i].file_audio);
    }
}

void inizializza() {
    if ( inizializzato == 1 ) return;

    for ( int i = 0; i < N; i++ ) {
        strcpy(table[i].giorno,"L");
        strcpy(table[i].previsione,"L");
        table[i].t_min = -255;
        table[i].t_max = -255;
        strcpy(table[i].file_audio, "L");
    }

    strcpy(table[0].giorno,"Lunedi");
    strcpy(table[0].previsione,"nuvoloso");
    table[0].t_min = -4;
    table[0].t_max = 6;
    strcpy(table[0].file_audio, "lu54.img");

    strcpy(table[1].giorno,"Martedi");
    strcpy(table[1].previsione,"nuvoloso");
    table[1].t_min = 5;
    table[1].t_max = 9;
    strcpy(table[1].file_audio, "ds32.img");


    inizializzato = 1;
}

Response * visualizza_dati_1_svc(char **req, struct  svc_req * reqstp) {

    static Response res;

    inizializza();

    if ( *req == NULL ) {
        return NULL;
    }

    res.size = 0;

    for ( int i = 0; i < N; i++ ) {
        if ( strcmp(table[i].previsione,*req) == 0 ) {
            strcpy(res.giorni[res.size].giorno, table[i].giorno);
            strcpy(res.giorni[res.size].previsione, table[i].previsione);
            res.giorni[res.size].t_min = table[i].t_min;
            res.giorni[res.size].t_max = table[i].t_max;
            strcpy(res.giorni[res.size].file_audio, table[i].file_audio);
            res.size++;
        }
    }

    return (&res);
}

int * aggiorna_temperature_1_svc(Request *req, struct svc_req *reqstp) {

    static int res;

    inizializza();

    res = -1;

    if ( req == NULL ) {
        return (&res);
    }

    for ( int i = 0; i < N; i++ ) {
        if ( strcmp(table[i].giorno,req->giorno) == 0 ) {
            
            table[i].t_min = req->t_min;
            table[i].t_max = req->t_max;

            res = 0;
            return(&res);
        }
    }

    return (&res);
}