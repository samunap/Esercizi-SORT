#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/wait.h>

#define INSTANCES 5
#define SERVER_PORT 8000
#define SERVER_HOST "127.0.0.1"

void client_instance(int instance_id){
	int sockfd;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	int number, response;

	// Create unique seed based on time and instance id
	srand(time(NULL) + instance_id);
	// Random number
	number = rand() % 1000;
	
	// Get server information
	server = gethostbyname(SERVER_HOST);
	if(server == NULL){
		fprintf(stderr, "Client %d: Error resolving host\n", instance_id);
		exit(1);
	}

	// Create socket
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if(sockfd == -1){
		perror("Error creating socket");
		exit(1);
	}

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = ((struct in_addr *)(server->h_addr))->s_addr;
	serv_addr.sin_port = htons(SERVER_PORT);

	// Connect to server
	if(connect(sockfd, (void*)&serv_addr, sizeof(serv_addr)) == -1){
		perror("Error connection to server");
		close(sockfd);
		exit(1);
	}

	printf("Client instance %d (PID: %d): Connected to server, sending number %d\n", instance_id, getpid(), number);
	// Send number to server
	if(send(sockfd, &number, sizeof(number),0) == -1) {
		perror("Error sending data");
		close(sockfd);
		exit(1);
	}
	// Receive response
	if(recv(sockfd, &response, sizeof(response),0) == -1){
		perror("Error receiving data");
		close(sockfd);
		exit(1);

	}

	printf("Client instance %d (PID: %d): Sent: %d, Received %d\n", instance_id, getpid(), number, response);
	close(sockfd);

}

int main(){
	int i, status;
	pid_t pid;
	printf("Starting %d client istances...\n", INSTANCES);

	// Create multiple client istances using fork
	for(i=0; i<INSTANCES; i++){
		pid = fork();

		if(pid < 0){
			// Fork failed
			perror("Fork failed");
			exit(1);
		} else if (pid == 0){
			// Child
			client_instance(i+1);
			exit(0);
		} else {
			// Parent
			printf("Started client instance %d (PID: %d)\n", i+1, pid);
			sleep(1); // Sleep for different seeds for random number gen
		}
	}

	// Wait all childs to finish
	for(i=0; i<INSTANCES; i++){
		wait(&status);
	}
	printf("All client instances completed\n");
	return 0;
}
