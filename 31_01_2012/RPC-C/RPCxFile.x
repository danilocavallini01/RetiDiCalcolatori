const N = 7;

const MAX_GIORNO = 10;
const MAX_PREVI = 10;
const MAX_AUDIO = 20;

struct Giorno {
    char giorno[MAX_GIORNO];
    char previsione[MAX_PREVI];
    int t_max;
    int t_min;
    char file_audio[MAX_AUDIO];
};

struct Response {
    Giorno giorni[N];
    int size;
};

struct Request {
    char giorno[MAX_GIORNO];
    int t_max;
    int t_min;
};

program OPERATION {
    version OPERATIONVERS {
        Response visualizza_dati(string previsione<MAX_PREVI>) = 1;
        int aggiorna_temperature(Request request) = 2;
    } = 1;
} = 0x20000013;