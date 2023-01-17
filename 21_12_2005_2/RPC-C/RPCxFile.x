const N = 10;
const K = 8;


const MAX_NOME = 20;
const MAX_STATO = 3;

struct Utenti {
    char utente[MAX_NOME];
};

struct Riga {
    char nome[MAX_NOME];
    char stato[MAX_STATO];
    Utenti utenti[K];
};

struct Risultato {
    Riga ris[N];
    int size;
    int success;
};

struct Richiesta {
    char nome[MAX_NOME];
    char stato[MAX_STATO];
};

program OPERATION {
    version OPERATIONVERS {
        int aggiungi_stanza(Richiesta richiesta) = 1;
        Risultato elimina_utente(string nome<MAX_NOME>) = 2;
    } = 1;
} = 0x20000013;