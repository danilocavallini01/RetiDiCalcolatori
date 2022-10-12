#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAX_STRING_LENGTH 256

int filterString(int fd_source,int fd_dest, char* prefix);
void figlio(int fd, char* prefix, char* filename);

int main(int argc, char* argv[]){
	char *prefix;
	int fd, pid;

    prefix = argv[1];

    for ( int i = 0 ; i < argc - 2 ; i++) {
        fd = open(argv[i+2], O_RDONLY);
        pid = fork();
        if ( pid < 0 ) {
            perror("P0: errore nella creazione del figlio");
            return EXIT_FAILURE;
        } else if ( pid == 0) {
            figlio(fd,prefix, argv[i+2]);
        } 
        close(fd);
    }
	
	return EXIT_SUCCESS;
	
}

void figlio(int fd, char* prefix, char* filename) {
    
    char tmp[MAX_STRING_LENGTH] = ".tmp_";
    char* filename_tmp = strcat(tmp,filename);
    int fd_out = open(filename_tmp, O_WRONLY|O_CREAT|O_TRUNC, 00640);

    //printf("%d,%s,%s\n", fd_out, prefix, filename);

    if (fd_out < 0){
        printf("P%d: Impossibile creare/aprire il file", getpid());
         exit(EXIT_SUCCESS);
    }
    
    filterString(fd,fd_out,prefix);


    close(fd_out);
    close(fd);

    execlp("mv","mv",filename_tmp,filename);

    exit(EXIT_FAILURE);
}


int filterString(int fd_source,int fd_dest, char* prefix){

    int nread, i, written;
    char read_char;
    int found = 0;

    while((nread = read(fd_source, &read_char, sizeof(char))) != 0 ) { //till EOF
        for(i=0; i < strlen(prefix); i++){
            if(read_char == prefix[i]){
                found = 1;
            }
        }

        if(found == 0) {
            written = write(fd_dest, &read_char, sizeof(char));


            if (written < 0){
                perror("P0: errore nella scrittura sul file");
                return EXIT_FAILURE;
            }
        }

        found=0;
    }
}

	
