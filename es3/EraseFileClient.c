/* Client per richiedere l'ordinamento remoto di un file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define DIM_BUFF 10

int main(int argc, char *argv[])
{
	int sd, port, fd_sorg, fd_dest, nread;

	char buff;
	
	struct hostent *host;
	struct sockaddr_in servaddr;

    char file[FILENAME_MAX+1];
    int filefd, line;

	/* CONTROLLO ARGOMENTI ---------------------------------- */
	if(argc != 5){
		printf("Error:%s serverAddress serverPort file line\n", argv[0]);
		exit(1);
	}

	/* INIZIALIZZAZIONE INDIRIZZO SERVER -------------------------- */
	memset((char *)&servaddr, 0, sizeof(struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	host = gethostbyname(argv[1]);

	/*VERIFICA INTERO*/
	nread=0;
	while( argv[2][nread]!= '\0' ){
		if( (argv[2][nread] < '0') || (argv[2][nread] > '9') ){
			printf("Secondo argomento non intero\n");
			exit(2);
		}
		nread++;
	}
	port = atoi(argv[2]);

	/* VERIFICA PORT e HOST */
	if (port < 1024 || port > 65535){
		printf("%s = porta scorretta...\n", argv[2]);
		exit(2);
	}
	if (host == NULL){
		printf("%s not found in /etc/hosts\n", argv[1]);
		exit(2);
	}else{
		servaddr.sin_addr.s_addr=((struct in_addr *)(host->h_addr))->s_addr;
		servaddr.sin_port = htons(port);
	}

    strcpy(file,argv[3]);
    if ((filefd = open(file,O_RDONLY)) < 0) {
        perror("open del file");
        exit(2);
    }

    line = htonl(atoi(argv[4]));

	sd=socket(AF_INET, SOCK_STREAM, 0);
    if(sd<0) {perror("apertura socket"); exit(1);}
    printf("Client: creata la socket sd=%d\n", sd);

    /* Operazione di BIND implicita nella connect */
    if(connect(sd,(struct sockaddr *) &servaddr, sizeof(struct sockaddr))<0)
    {perror("connect"); exit(1);}
    printf("Client: connect ok\n");


    if ( write(sd,&line,sizeof(line)) <= 0 ) {
        perror("sending line to erase");
        exit(2);
    }
    
    /*INVIO File*/
    printf("Client: stampo e invio file da ordinare\n");
    while((nread=read(filefd, &buff, sizeof(char))) > 0){
        write(sd,&buff,nread);
    }

	write(sd,'\0',1);

    close(filefd);
    if ((filefd = open(file,O_WRONLY | O_TRUNC)) < 0) {
        perror("open del file");
        exit(2);
    }

    printf("Client: stampo e invio file da ordinare\n");
    while((nread=read(sd,&buff, sizeof(char))) > 0){
        write(filefd,&buff,nread);
    }
    printf("\nClient: finito di modificare file\n");

    /* Chiusura file */
    close(filefd);
    close(sd);

	printf("\nClient: termino...\n");
	exit(0);
}

