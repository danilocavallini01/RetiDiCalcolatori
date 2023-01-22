const N = 10;

const MAX_CANTANTE = 20;
const MAX_TITOLO = 50;
const MAX_FILE = 50;
const MAX_RESULT = 6;

struct Canzone {
    char cantante[MAX_CANTANTE];
    char titolo[MAX_TITOLO];
    int voti;
    char nomefile[MAX_FILE];
};

struct Response {
    Canzone canzoni[MAX_RESULT];
    int size;
};

struct Request {
    int isCantante;
    char cantante[MAX_CANTANTE];
    char titolo[MAX_TITOLO];
};

program OPERATION {
    version OPERATIONVERS {
        Response visualizza_canzoni_conmenodi_V_voti(int voto) = 1;
        int esprimi_voto(Request richiesta) = 2;
    } = 1;
} = 0x20000013;