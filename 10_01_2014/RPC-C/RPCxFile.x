const N = 10;

const DESC_LENGTH = 31;
const TIPO_LENGTH = 9;
const DATA_LENGTH = 11;
const LUOGO_LENGTH = 16;

struct Inserimento {
    char descrizione[DESC_LENGTH];
    char tipo[TIPO_LENGTH];
    char date[DATA_LENGTH];
    char luogo[LUOGO_LENGTH];
    int disponibilita;
    int prezzo;
};

struct Richiesta {
    char descrizione[DESC_LENGTH];
    int num_biglietti;
};

program OPERATION {
    version OPERATIONVERS {
        int inserimento_evento(Inserimento richiesta) = 1;
        int acquista_biglietti(Richiesta richiesta) = 2;
    } = 1;
} = 0x20000013;