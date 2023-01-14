/* operazioni_proc.c
 * 	+implementazione delle procedure remote: "somma" e "moltiplicazione".
 *	+include operazioni.h.
 */

#include "procedure.h"
#include <rpc/rpc.h>
#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>

FileScanRes *filescan_1_svc(char **filename, struct svc_req *rp) {
    static FileScanRes *ris;
    free(ris);
    
    printf("File ricevuto: %s\n", *filename);
    
    int fd = open(filename,"r+",O_RDONLY);
    
    ris->characters = 0;
    ris->lines = 0;
    ris->words = 0;
    ris->errno = 0;

    if ( fd == -1 ) {
        ris->errno = 1;
        close(fd);
        return(&ris);
    }

    char c;

    while (read(fd,&c,1) == 1) {
        if (c == '\n') {
            ris->lines++;
        } else if ( c == ' ' || c == '\t') {
            ris->words++;
        } else {
            ris->characters++;
        }
    }

    ris->lines++;
    ris->errno = 0;
    
    close(fd);

    return (&ris);
}

int *dirscan_1_svc(DirInput *input, struct svc_req *rp) {
    static int ris;
    printf("Dati ricevuti: %s - %i\n", input->dir, input->threshold);

    ris = 0;

    DIR *d;
    struct dirent *dir;
    d = opendir(input->dir);
    
    if ( !d ) {
        ris = -1;
        return(&ris);
    }

    char *filename = input->dir;
    while ((dir = readdir(d)) != NULL) {
        filename = input->dir;
        strcat(filename,"/");
        strcat(filename,dir->d_name);

        int fd = open(filename,"r+",O_RDONLY);
        if ( fd == -1 ) {
            ris = -1;
            close(fd);
            return(&ris);
        }

        fseek(fd, 0, SEEK_END);
        int size = ftell(fd);

        if ( size <= input->threshold ) {
            ris++;
        }
        
        close(fd);
    }
    closedir(d);
    
    return (&ris);
}
