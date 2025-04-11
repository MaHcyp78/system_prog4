#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT "8080"
#define BUFFSIZE 1024
#define MAX_GUESS_NUM 1000

void guess(int sd){
	srand(time(NULL));
	
	char buffer[BUFFSIZE];
	char answer[BUFFSIZE];
	
	while(1){
		memset(buffer, 0, BUFFSIZE);
		memset(answer, 0, BUFFSIZE);

		printf("Enter a number: ");
		scanf("%s", buffer);
		printf("\tbuf-%s\n", buffer);
		send(sd, buffer, BUFFSIZE, 0);
		read(sd, answer, BUFFSIZE);
		printf("\t\tans-%s\n", answer);
		if(!strcmp(buffer, answer)){
			break;
		}
	}
}

int main(int argc, char **argv){
	if(argc < 3){
		printf("Usage: ./argv[0] <server ip> <port>\n");
		exit(EXIT_FAILURE);
	}
	
	int status, sd;
	struct addrinfo hints, *res, *p;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	status = getaddrinfo(argv[1], PORT, &hints, &res);
	
	for(p = res; p != NULL; p = p->ai_next){
		if((sd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
			perror("client: socket");
			continue;
		}

		if(connect(sd, res->ai_addr, res->ai_addrlen) == -1){
			close(sd);
			perror("client: connect");
			continue;
		}
		break;
	}
	
	guess(sd);

	freeaddrinfo(res);
	close(sd);
	exit(EXIT_SUCCESS);
}
