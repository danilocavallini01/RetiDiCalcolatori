#define MAX_PREN 10
#define M 6
#define STRING_LENGTH 255

#define MAX_TITOLO 21
#define MAX_COPIA 11

typedef struct
{
    char titolo[MAX_TITOLO];
    int copie[M][3];
    int copieTotali;
    int copieDisp;
} Prenotazione;

typedef struct
{
    char titolo[MAX_TITOLO];
    int gg;
    int mm;
    int aa;
} UDPReq;
