#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8000
#define MAX_CONNECTIONS 20

int main(int argc, char *argv[]){
	int sockfd, newsockfd;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t address_size;
	int number, response;
	int pid;
	int opt = -1;

	// Create socket
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd == -1){
		perror("Error creating socket");
		exit(1);
	}

	// Set socket option to reuse address
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))){
		perror("Setsockopt failed");
		close(sockfd);
		exit(1);
	}

	// Clear address structure
	bzero(&serv_addr, sizeof(serv_addr));

	// Configure server address
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORT);
	
	// Bind socket to address
	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1){
		perror("Error binding socket");
		close(sockfd);
		exit(1);
	}

	// Set maximum connection queue
	if (listen(sockfd, MAX_CONNECTIONS) == -1){
		perror("Error on listen");
		close(sockfd);
		exit(1);
	}

	printf("Server started on port %d. Waiting for connections...\n", PORT);

	while(1){
		address_size = sizeof(cli_addr);

		// Accept new connection
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &address_size);
		if(newsockfd == -1){
			perror("Error on accept");
			continue;
		}
		
		printf("New connection from %s:%d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

		// Fork to handle connection
		if((pid = fork()) < 0){
			perror("Fork error");
			continue;
		}

		if(pid == 0){
			close(sockfd);
			int recv_result = recv(newsockfd, &number, sizeof(number), 0);
			if(recv_result <= 0){
				if(recv_result == 0){
					printf("Connection closed by client\n");
				} else{
					perror("Error receiving data");
				}
				close(newsockfd);
				exit(1);
			}

			// Add 1
			response = number + 1;
			printf("Child process %d: Received %d, sending %d\n", getpid(), number, response);

			// Send response back to client
			if(send(newsockfd, &response, sizeof(response), 0) == -1){
				perror("Error sending data");
			}

			close(newsockfd);
			exit(0);
		} else{
			close(newsockfd);
		}

	}
	return 0;
}
