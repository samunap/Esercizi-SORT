#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <netdb.h>

#include "list.h"

char *host_name = "127.0.0.1"; /* local host */
int port = 8000;

int main(int argc, char *argv[]){
	ItemType msg;

	if (argc < 4){
		printf("Usage: %s nome quantita quantita_min\n", argv[0]);
		return -1;
	}

	// Creazione messaggio
	strncpy(msg.nome, argv[1], LENMAX);
	msg.quantita = atoi(argv[2]);
	msg.quantita_min = -1;
	msg.tipo = TIPO_FORNITORE;
	msg.sockfd = -1;

	if(msg.quantita < msg.quantita_min){
		printf("quantita minima non valida: %d > %d!\n", msg.quantita_min, msg.quantita);
		return -1;
	}

	
	struct sockaddr_in serv_addr;
	struct hostent* serer;

	if( (server = gethostbyname(host_name)) == 0){
		perror("Error resolving local host\n");
		exit(1);
	}

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = ((struct in_addr *)(server->h_addr))->s_addr;
	serv_addr.sin_port = htons(port);

	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if(sockfd == -1){
		perror("Error opening socket\n");
		exit(1);
	}

	if(connect(sockfd,(void*)&serv_addr, sizeof(serv_addr)) == -1){
		perror("Error connecting to socket\n");
		exit(1);
	}

	// Invio fornitura
	printf("Sending availability of supply\n");
	PrintItem(msg);

	if(send(sockfd, &msg, sizeof(msg),0) == -1){
		perror("Error on send");
		exit(1);
	}

	// Aspetto gli ordini
	printf("Waiting for orders:\n");

	msg.quantita = 0;
	while(msg.quantita >= 0){
		if(recv(sockfd, &msg, sizeof(msg), 0) == -1){
			perror("Error in receiving reponse from server\n");
			exit(1);
		}
		if(msg.quantita >= 0){
			printf("order: ");
			PrintItem(msg);
		}
		else{
			printf("end of order\n");
		}
	}

	printf("Delivery completed!\n");



	close(sockfd);

	return 0;

}
