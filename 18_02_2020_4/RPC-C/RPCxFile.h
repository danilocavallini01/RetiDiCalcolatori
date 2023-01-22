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

#define N 10
#define MAX_CANTANTE 20
#define MAX_TITOLO 50
#define MAX_FILE 50
#define MAX_RESULT 6

struct Canzone {
	char cantante[MAX_CANTANTE];
	char titolo[MAX_TITOLO];
	int voti;
	char nomefile[MAX_FILE];
};
typedef struct Canzone Canzone;

struct Response {
	Canzone canzoni[MAX_RESULT];
	int size;
};
typedef struct Response Response;

struct Request {
	int isCantante;
	char cantante[MAX_CANTANTE];
	char titolo[MAX_TITOLO];
};
typedef struct Request Request;

#define OPERATION 0x20000013
#define OPERATIONVERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define visualizza_canzoni_conmenodi_V_voti 1
extern  Response * visualizza_canzoni_conmenodi_v_voti_1(int *, CLIENT *);
extern  Response * visualizza_canzoni_conmenodi_v_voti_1_svc(int *, struct svc_req *);
#define esprimi_voto 2
extern  int * esprimi_voto_1(Request *, CLIENT *);
extern  int * esprimi_voto_1_svc(Request *, struct svc_req *);
extern int operation_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define visualizza_canzoni_conmenodi_V_voti 1
extern  Response * visualizza_canzoni_conmenodi_v_voti_1();
extern  Response * visualizza_canzoni_conmenodi_v_voti_1_svc();
#define esprimi_voto 2
extern  int * esprimi_voto_1();
extern  int * esprimi_voto_1_svc();
extern int operation_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_Canzone (XDR *, Canzone*);
extern  bool_t xdr_Response (XDR *, Response*);
extern  bool_t xdr_Request (XDR *, Request*);

#else /* K&R C */
extern bool_t xdr_Canzone ();
extern bool_t xdr_Response ();
extern bool_t xdr_Request ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_RPCXFILE_H_RPCGEN */
