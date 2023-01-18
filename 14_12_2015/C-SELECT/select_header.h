#define MAX_PREN 10
#define STRING_LENGTH 255
#define ID_LENGTH 7
#define NPERS_LENGTH 4
#define TPREN_LENGTH 16
#define TVEIC_LENGTH 7
#define TARGA_LENGTH 8
#define IMMAG_LENGTH 25

typedef struct
{
    char id[ID_LENGTH];
    char n_persone[NPERS_LENGTH];
    char tipo_prenotazione[TPREN_LENGTH];
    char tipo_veicolo[TVEIC_LENGTH];
    char targa[TARGA_LENGTH];
    char immagine[IMMAG_LENGTH];
} Prenotazione;

typedef struct
{
    char id[ID_LENGTH];
    char n_persone[NPERS_LENGTH];
} UDPReq;
