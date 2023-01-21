#include "RPCxFile.h"
#include <rpc/rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>


int *elimina_occorrenze_1_svc(char **nomefile, struct svc_req *reqstp ) {

    static int res;

    res = -1;
    int name_l, file_fd, tmp_fd, nread;
    char chr;
    char filename_tmp[FILE_LENGTH];

    if ( *nomefile == NULL ) {
        return (&res);
    }

   
    // CONTROLLO SE FILE DI INPUT E' UN FILE DI TESTO
    name_l = strlen(*nomefile);
    
    if ( name_l < 4 && !( *nomefile[name_l-1] == 't' && *nomefile[name_l-2] == 'x' 
        && *nomefile[name_l - 3] == 't' && *nomefile[name_l - 4] == '.' ) ) {

        return (&res);
    }

    // APRO FILE
    if ( (file_fd = open(*nomefile,O_RDONLY)) < 0) {
        return (&res);
    }
   
    // CREO FILE DI SUPPORTO
    strcpy(filename_tmp,*nomefile);
    strcat(filename_tmp,"__TMP");

    if ( (tmp_fd = open(filename_tmp,O_WRONLY | O_CREAT)) < 0) {
        close(file_fd);
        return (&res);
    }

   
    res = 0;
    while ( (nread = read(file_fd, &chr, sizeof(chr))) > 0 ) {

        if ( chr < 48 || chr > 57 ) {
            if ( write(tmp_fd, &chr, sizeof(chr)) < 0 ) {
                res = -1;
                close(tmp_fd);
                close(file_fd);
                return (&res);
            }
        } else {
            res++;
        }
    }

    rename(filename_tmp,*nomefile);

    close(tmp_fd);
    close(file_fd);
    return (&res);

}

Response *lista_file_carattere_1_svc(Request * request, struct svc_req *reqstp ) {

    static Response res;

    if ( request == NULL ) {
        return (&res);
    }

    DIR *dir;
    struct dirent *file;
    char nomefile[256];
    int name_l, occorrenze;

    dir = opendir(request->nomedir);

    if ( dir == NULL ) {
        res.error = 1;
        return (&res);
    }

    res.size = 0;

    while ( (file = readdir(dir)) != NULL ) {

        strcpy(nomefile, file->d_name);
        //strcat(nomefile, '\0');

        name_l = strlen(nomefile);

        // CONTROLLO CHE FINISCA CON TXT
        if ( name_l > 4 && !nomefile[name_l-1] != 't' || nomefile[name_l-2] != 'x' 
            || nomefile[name_l - 3] != 't' || nomefile[name_l - 4] != '.' ) {

            // CONTROLLO CHE FILE ABBIA LE OCCORRENZE
            occorrenze = 0;
            for ( int i = 0; i < name_l - 4; i++ ) {
                
                if ( nomefile[i] == request->chr ) {
                    occorrenze++;

                    if ( occorrenze == request->noccorrenze ) {
                        strcpy(res.files[res.size++].nomefile, nomefile);
                        break;
                    }
                }
            }

            // CONTROLLO CHE NON SIAMO A MAX CAPACITA
            if ( res.size == 6 ) {
                break;
            }
        }
    }

    res.error = 0;

    closedir(dir);
    return (&res);
}   
