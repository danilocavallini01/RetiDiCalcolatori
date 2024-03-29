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

#define FILE_LENGTH 50
#define MAX_RESPONSE_FILES 6

struct Request {
	char nomedir[FILE_LENGTH];
	char chr;
	int noccorrenze;
};
typedef struct Request Request;

struct RFile {
	char nomefile[FILE_LENGTH];
};
typedef struct RFile RFile;

struct Response {
	RFile files[MAX_RESPONSE_FILES];
	int size;
	int error;
};
typedef struct Response Response;

#define OPERATION 0x20000013
#define OPERATIONVERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define elimina_occorrenze 1
extern  int * elimina_occorrenze_1(char **, CLIENT *);
extern  int * elimina_occorrenze_1_svc(char **, struct svc_req *);
#define lista_file_carattere 2
extern  Response * lista_file_carattere_1(Request *, CLIENT *);
extern  Response * lista_file_carattere_1_svc(Request *, struct svc_req *);
extern int operation_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define elimina_occorrenze 1
extern  int * elimina_occorrenze_1();
extern  int * elimina_occorrenze_1_svc();
#define lista_file_carattere 2
extern  Response * lista_file_carattere_1();
extern  Response * lista_file_carattere_1_svc();
extern int operation_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_Request (XDR *, Request*);
extern  bool_t xdr_RFile (XDR *, RFile*);
extern  bool_t xdr_Response (XDR *, Response*);

#else /* K&R C */
extern bool_t xdr_Request ();
extern bool_t xdr_RFile ();
extern bool_t xdr_Response ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_RPCXFILE_H_RPCGEN */
