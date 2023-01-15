const N = 12;
const MAX_RIS = 6;
const MAX_TARGA = 8;
const MAX_PATENTE = 6;
const MAX_TVEICOLO = 7;
const MAX_IMMAGINI = 13;

struct Riga {
    char targa[MAX_TARGA];
    char patente[MAX_PATENTE];
    char tipoVeicolo[MAX_TVEICOLO];
    char immagini[MAX_IMMAGINI];    
};

struct Output {
    Riga ris[MAX_RIS];
};

struct Input {
    char targa[MAX_TARGA];
    char patente[MAX_PATENTE];
};

program OPERATION {
    version OPERATIONVERS {
        Output visualizza_prenotazioni(string tipoveicolo<7>) = 1;
        bool aggiorna_patente(Input) = 2;
    } = 1;
} = 0x20000013;