#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT "8080"
#define BACKLOG 5
#define BUFFSIZE 1024
#define MAX_GUESS_NUM 1000

int main(int argc, char *argv[]){
	if(argc < 2){
		printf("Usage: ./argv[0] <port>\n");
		exit(EXIT_FAILURE);
	}
	
	int status, sd, new_sd, pid, bytes_received;
	struct addrinfo hints, *res, *p;
	struct sockaddr_in *h, addr;
	socklen_t addr_size;
	char dest[INET_ADDRSTRLEN];
	char buffer[BUFFSIZE];
	char guessnum[BUFFSIZE];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	status = getaddrinfo(NULL, PORT, &hints, &res);

	for(p = res; p != NULL; p = p->ai_next){
		struct sockaddr_in *ipv4 = (struct sockaddr_in *) p->ai_addr;
		struct in_addr *addr = &(ipv4->sin_addr);
		inet_ntop(p->ai_family, addr, dest, sizeof dest);
		
		printf("%s\n", dest);
		
		if((sd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
			perror("server: socket");
			continue;
		}
		if(bind(sd, p->ai_addr, p->ai_addrlen) == -1){
			close(sd);
			perror("server: bind");
			continue;
		}
		break;	
	}
	freeaddrinfo(res);
	
	listen(sd, BACKLOG);
	
	while(1){
		addr_size = sizeof addr;
		new_sd = accept(sd, (struct sockaddr *) &addr, &addr_size);
		if(new_sd == -1)
			continue;
		
		inet_ntop(addr.sin_family, &addr.sin_addr, dest, sizeof dest);
		printf("Got connection from: %s\n", dest);
		
		pid = fork();
		if(!pid){
			close(sd);
			srand(time(NULL));
			memset(guessnum, 0, BUFFSIZE);
			sprintf(guessnum, "%d", rand()%MAX_GUESS_NUM);

			while(1){
				memset(buffer, 0, BUFFSIZE);
				bytes_received = recv(new_sd, buffer, BUFFSIZE, 0);	
				if(bytes_received)
					printf("\t[client %s] - guess_%s\tright_%s\n", dest, buffer, guessnum);

				if(!strcmp(buffer, guessnum)){
					write(new_sd, guessnum, BUFFSIZE);
					break;
				}

				sprintf(buffer, "%s", "-1");
				write(new_sd, buffer, BUFFSIZE);
			}
			printf("Connection %s closed\n", dest);
			close(new_sd);
			exit(EXIT_SUCCESS);
		}
		signal(SIGCHLD, SIG_IGN);	
		close(new_sd);
	}
	exit(EXIT_SUCCESS);
}
