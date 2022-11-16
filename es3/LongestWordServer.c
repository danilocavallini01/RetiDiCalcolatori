/* Server che fornisce la valutazione di un'operazione tra due interi */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define MAX_LENGTH 256

/*Struttura di una richiesta*/
/********************************************************/
typedef struct{
	char nomefile[MAX_LENGTH];
}Request;
/********************************************************/

int main(int argc, char **argv){
	int sd, port, len, num1, ris, nread, conn_sd, file;
    int maxWord = 0, currentMaxWord = 0;
	const int on = 1;
	struct sockaddr_in servaddr, cliaddr;
	
    char nomefile[MAX_LENGTH], buf;

	Request* req =(Request*)malloc(sizeof(Request));

	/* CONTROLLO ARGOMENTI ---------------------------------- */
	if(argc!=2){
		printf("Error: %s port\n", argv[0]);
		exit(1);
	}
	else{
		num1=0;
		while( argv[1][num1]!= '\0' ){
			if((argv[1][num1] < '0') || (argv[1][num1] > '9')){
				printf("Secondo argomento non intero\n");
				printf("Error: %s port\n", argv[0]);
				exit(2);
			}
			num1++;
		}  	
	  	port = atoi(argv[1]);
  		if (port < 1024 || port > 65535){
		      printf("Error: %s port\n", argv[0]);
		      printf("1024 <= port <= 65535\n");
		      exit(2);  	
  		}
	}

	/* INIZIALIZZAZIONE INDIRIZZO SERVER ---------------------------------- */
	memset ((char *)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;  
	servaddr.sin_port = htons(port);  

	/* CREAZIONE, SETAGGIO OPZIONI E CONNESSIONE SOCKET -------------------- */
	sd=socket(AF_INET, SOCK_DGRAM, 0);

	if (sd <0) {
        perror("creazione socket "); 
        exit(1);
    }
	printf("Server: creata la socket, sd=%d\n", sd);

	if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))<0) {
        perror("set opzioni socket ");
        exit(1);
    }

	printf("Server: set opzioni socket ok\n");

	if (bind(sd,(struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
        perror("bind socket "); 
        exit(1);
    }
	printf("Server: bind socket ok\n");

	/* CICLO DI RICEZIONE RICHIESTE ------------------------------------------ */
    len=sizeof(struct sockaddr_in);

	while(1) {

		if (recvfrom(sd, nomefile, sizeof(nomefile), 0, (struct sockaddr *)&cliaddr, &len)<0) {
            perror("recvfrom "); 
            continue;
        }

		printf("\nFile ricevuto: %s\n" , nomefile);

        if ((file = open(nomefile,O_RDONLY)) < 0) {
            perror("open del file");
            ris = -1;
        } else {
            while((nread = read(file, &buf, 1)) > 0) {
                if ( buf == '\n' || buf == ' ') {
                    if ( maxWord < currentMaxWord ) {
                        maxWord = currentMaxWord;
                    }
                    currentMaxWord = 0;
                } else {
                    currentMaxWord++;
                }
            }
            ris = maxWord;
        }

        

		if (sendto(sd, &ris, sizeof(ris), 0, (struct sockaddr *)&cliaddr, len)<0) {
            perror("sendto "); continue;
        }

        close(file);
	} 
}
