#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAX_STRING_LENGTH 256
#define MAX_FILENAMES 100

char *gets(char *s);

int main(int argc, char* argv[]){
	int fd[MAX_FILENAMES], bytes_to_write, written;
    int prefix;
	char *file_out;
    char riga[MAX_STRING_LENGTH];
    char *token;
    
    char *prefix_string;
	
    if ( argc < 2) {
        perror("P0: Passare almeno un fileName");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < argc - 1 ; i++) {
        fd[i] = open(argv[i+1], O_WRONLY|O_CREAT|O_TRUNC, 00640);   
        if (fd[i] < 0){
            perror("P0: Impossibile creare/aprire il file");
            return EXIT_FAILURE;
        }
    }
	
	
	printf("Inserisci le nuove righe, o EOF [CTRL^D] per terminare\n");

    while(gets(riga)>0){
		strcat(riga, "\n");
        prefix_string = strtok(riga,":");
        prefix = atoi(prefix_string);

        if ( prefix == 0 || prefix > argc - 1) {
            perror("P0: errore nel numero del file");
			return EXIT_FAILURE;
        }


        token = strtok(NULL, ":");
        
        bytes_to_write = strlen(token);

        written = write(fd[prefix-1], token, bytes_to_write);

        
		if (written < 0){
			perror("P0: errore nella scrittura sul file");
			return EXIT_FAILURE;
		}
		
    }

	for (int i = 0; i < argc - 1; i++) {
        close(fd[i]);
    }

	return EXIT_SUCCESS;
}