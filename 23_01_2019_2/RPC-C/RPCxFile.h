/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _RPCXFILE_H_RPCGEN
#define _RPCXFILE_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif

#define N 12
#define MAX_RIS 6
#define MAX_TARGA 8
#define MAX_PATENTE 6
#define MAX_TVEICOLO 7
#define MAX_IMMAGINI 13

struct Riga {
	char targa[MAX_TARGA];
	char patente[MAX_PATENTE];
	char tipoVeicolo[MAX_TVEICOLO];
	char immagini[MAX_IMMAGINI];
};
typedef struct Riga Riga;

struct Output {
	Riga ris[MAX_RIS];
};
typedef struct Output Output;

struct Input {
	char targa[MAX_TARGA];
	char patente[MAX_PATENTE];
};
typedef struct Input Input;

#define OPERATION 0x20000013
#define OPERATIONVERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define visualizza_prenotazioni 1
extern  Output * visualizza_prenotazioni_1(char **, CLIENT *);
extern  Output * visualizza_prenotazioni_1_svc(char **, struct svc_req *);
#define aggiorna_patente 2
extern  bool_t * aggiorna_patente_1(Input *, CLIENT *);
extern  bool_t * aggiorna_patente_1_svc(Input *, struct svc_req *);
extern int operation_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define visualizza_prenotazioni 1
extern  Output * visualizza_prenotazioni_1();
extern  Output * visualizza_prenotazioni_1_svc();
#define aggiorna_patente 2
extern  bool_t * aggiorna_patente_1();
extern  bool_t * aggiorna_patente_1_svc();
extern int operation_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_Riga (XDR *, Riga*);
extern  bool_t xdr_Output (XDR *, Output*);
extern  bool_t xdr_Input (XDR *, Input*);

#else /* K&R C */
extern bool_t xdr_Riga ();
extern bool_t xdr_Output ();
extern bool_t xdr_Input ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_RPCXFILE_H_RPCGEN */
