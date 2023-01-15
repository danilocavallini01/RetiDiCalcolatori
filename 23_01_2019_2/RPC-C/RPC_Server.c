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
        strcpy(table[i].targa,"L");
        strcpy(table[i].patente,"L");
        strcpy(table[i].tipoVeicolo,"L");
        strcpy(table[i].immagini,"L");
    }

    strcpy(table[0].targa,"GG111AA");
    strcpy(table[0].patente,"00111");
    strcpy(table[0].tipoVeicolo,"auto");
    strcpy(table[0].immagini,"AA111AA_img/");

    strcpy(table[1].targa,"ZA111AA");
    strcpy(table[1].patente,"00111");
    strcpy(table[1].tipoVeicolo,"camper");
    strcpy(table[1].immagini,"AA111AA_img/");
}

Output *visualizza_prenotazioni_1_svc(char **tipoveicolo, struct svc_req *reqstp) {
    static Output res;
    int j = 0;

    if ( inizializzato == 0 ) {
        inizializza();
    }

    for ( int i = 0; i < MAX_RIS; i++ ) {
        strcpy(res.ris[i].targa,"L");
        strcpy(res.ris[i].patente,"L");
        strcpy(res.ris[i].tipoVeicolo,"L");
        strcpy(res.ris[i].immagini,"L");
    }

    for ( int i = 0; i < N; i++ ) {
        // D = 68, E = 69
        if ((table[i].targa[0] > 69 || ( table[i].targa[0] == 69 && table[i].targa[1] > 68)) && strcmp( table[i].tipoVeicolo , *tipoveicolo ) == 0) {
            res.ris[j++] = table[i];
            
            if ( j >= 6 ) {
                return(&res);
            } 
            
        }
    }

    return(&res);
}

bool_t *aggiorna_patente_1_svc(Input *input,struct svc_req *reqstp ) {
    static bool_t res;
    res = FALSE;

    if ( inizializzato == 0 ) {
        inizializza();
    }
    
    for ( int i = 0; i < N; i++ ) {
        if ( strcmp(table[i].targa, input->targa) == 0 ) {
            strcpy(table[i].patente,input->patente);
            res = TRUE;
            return(&res);
        }
    }

    return(&res);
}
