/* operazioni_client.c
 *
 */

#include "procedure.h"
#include <rpc/rpc.h>
#include <stdio.h>

int main(int argc, char *argv[]) {

    CLIENT  *cl;
    char    *server;

    FileScanRes *ris1;
    int         *ris2;

    char    *operation;
    char    *filename,*threshold;
    DirInput input;

    cl = clnt_create(server, PROCEDUREPROG, PROCEDUREVERS, "udp");
    if (cl == NULL) {
        clnt_pcreateerror(server);
        exit(1);
    }

    printf("Scrivere f per File_Scan || d per Dir_Scan\n");
    while(gets(operation)) {
        if ( operation[0] == 'f' ) {
            
            printf("Inserire nome del file: ");
            if ( gets(filename) == NULL ) {
                 printf("Scrivere f per File_Scan || d per Dir_Scan\n");
                break;
            }

            ris1 = filescan_1(&filename, cl);

        } else if ( operation[0] == 'd') {
            
            printf("Inserire nome del file: ");
            if ( gets(input.dir) == NULL ) {
                 printf("Scrivere f per File_Scan || d per Dir_Scan\n");
                break;
            }

            printf("Inserire soglia del file: ");
            if ( gets(threshold) == NULL ) {
                 printf("Scrivere f per File_Scan || d per Dir_Scan\n");
                break;
            }

            input.threshold = atoi(threshold);

            ris2 = dirscan_1(&input,cl);
        }
        printf("Scrivere f per File_Scan || d per Dir_Scan\n");
    }
    // Libero le risorse distruggendo il gestore di trasporto
    clnt_destroy(cl);
}
