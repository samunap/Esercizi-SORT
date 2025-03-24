#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <netdb.h>

#include "list.h"

int port = 8000;

int verificaFornitura(ItemType fornitore, LIST centri);
LIST inviaFornitura(ItemType fornitore, LIST centri);

int main(){
	LIST fornitori;
	LIST centri;
	LIST tmp;
	ItemType item;

	fornitori = NewList();
	centri = NewList();

	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;

	int sockfd = socket(PF_INET, SOCK_STREAM,0);
	if(sockfd == -1){
		perror("Error opening socket\n");
		exit(1);
	}
	int options = 1;
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &options, sizeof(options)) < 0){
		perror("Error on setsockopt\n");
		exit(1);
	}

	bzero(&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	// Address bind to socket
	if(bind(sockfd (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1){
		perror("Error on binding");
		exit(1);
	}

	socklen_t address_size = sizeof(cli_addr);
	
	while(1){
		// Accetto nuova connessione
		int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &address_size);
		if(newsockfd == -1){
			perror("Error on accept\n");
			exit(1)
		}
		// Ricevo messaggio
		if(recv(newsockfd, &item, sizeof(item), 0) == -1){
			perror("Error on receive\n");
			exit(1);
		}
		printf("Received new request: \n");
		PrintItem(item);

		item.sockfd = newsockfd;

		// Se il tipo è fornitore:
		if(item.tipo == TIPO_FORNITORE){
			printf("Il fornitore sta verificando la distribuzione...\n");
			if(verificaFornitura(item,centri)){
				printf("Distribuzione iniziata\n");
				centri = invioFornitura(item,centri);
			}
			else{
				printf("Distribuzione non possibile\n");
				fornitori = EnqueueLast(fornitori,item);
			}
		}
		// Se il tipo è centro
		else if(item.tipo == TIPO.CENTRO){
			printf("Il centro vaccinale sta verificando la fornitura\n");
			centri = EnqueueOrdered(centri, item);
			tmp = fornitori;
			while(!isEmpty(tmp)){
				if(verificaFornitura(tmp->item, centri)){
					printf("Distribuzione iniziata\n");
					centri = invioFornitura(tmp->item, centri);
				}
				tmp = tmp->next;
			}
		}
		else{
			printf("Tipo %d non valido.\n", item.tipo);
			close(newsockfd);
		}
		printf("Fornitori:\n");
		PrintList(fornitori);
		printf("Centri Vaccinali:\n");
		PrintList(centri);
	}
	close(sockfd);
	return 0;
}
	
int verificaFornitura(ItemType fornitore, LIST centri){
	LIST tmp;
	
	int quantita_fornita = 0;
	int quantita_rimasta = fornitore.quantita;

	tmp = centri;
	while(!isEmpty(tmp)){
		if(tmp->item.quantita <= quantita_rimasta){
			quantita_fornita += tmp->item.quantita;
			quantita_rimasta -= tmp->item.quantita;
		}
		tmp = tmp -> next;
	}
	printf("Il fornitore:");
	PrintItem(fornitore);
	printf("può fornire %d vaccini.", quantita_fornita);

	if(quantita_fornita >= fornitore.quantita_min){
		printff("Inizio fornitura\n");
		return 1;
	}else{
		printf("Fornitore in attesa\n");
		return 0;
	}
}

LIST invioFornitura(ItemType fornitore, LIST centri){
	ItemType* ordine;
	ItemType finelista;

	int quantita_fornita = 0;
	int quantita_rimasta = fornitore.quantita;

	do{
		ordine = FindFirstAvai
		if(ordine != NULL){
			printf("Sto servendo il centro vaccinale:");
			PrintItem(*ordine);
			quantita_fornita += ordine->quantita;
			quantita_rimasta -= ordine->quantita;
			
			printf("Invio nominativo centro al fornitore");
			PrintfItem(*ordine);

			if(send(sockfd, ordine, sizeof(ItemType),0) == -1){
				perror("Error on send\n");
				exit(1);
			}
			
			printf("Invio nominativo fornitore al centro");
			PrintItem(fornitore);

			if(send(ordine->sockfd, &fornitore, sizeof(fornitore),0) == -1){
				perror("Error on send\n");
				exit(1);
			}

			close(ordine->sockfd);

			centri = Dequeue(centri, *ordine);
			printf("sono rimasti %d vaccini\n", quantita_rimasta);
			printf("e sono stati forniti %d vaccini\n", quantita_fornita);
		}
		else{
			printf("Non ci sono nuovi ordini.\n");
		}

	} while(ordine != NULL)

	printf("Invio al fornitore messaggio di fine lista\n");
	strncpy(finelista.nome, "finelista", LENMAX);
	finelista.quantita = -1;
	finelista.quantita_min = -1;
	finelista.tipo = -1;
	finelista.sockfd = -1;
	PrintItem(finelista);
	if(send(fornitore.sockfd, &finelista, sizeof(finelista),0) == -1){
		perror("Error on send\n");
		exit(1);
	}
	close(fornitore.sockfd);

	return centri;
}






