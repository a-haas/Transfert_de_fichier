#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "fonction_reseau.h"


//Lecture du fichier
int lecture(Buffer buffer, int fd){
	int nb_lu = read(fd, buffer->msg, BUFFER_LENGTH);
	if(nb_lu < 0){
		perror("READ\n");
		return -1;
	}

	buffer->taille = nb_lu;
	//Fin de lecture
	if(nb_lu < BUFFER_LENGTH)
		return 1;

	return 0;
}


int main(int argc, char* argv[]) {
    
	if(argc < 5 || argc > 6){
	        printf("Usage : %s <ip_version> <filename> <distant_host> <distant_port> [<local_port>]\n", argv[0]);
	        return -1;
	}

	struct sockaddr *dist_addr = malloc(sizeof(struct sockaddr));

	//Préparation de l'adresse distante
	if(atoi(argv[1]) == 4)
		S_DOMAIN = AF_INET;

	else if(atoi(argv[1]) == 6)
		S_DOMAIN = AF_INET6;

	else{
		printf("Domain invalid enter\n");
		return -1;
	}

	if(S_distantAddress(argv[3], atoi(argv[4]), &dist_addr) == -1){
	       	perror("S_distantAddress\n");
	       	return -1;
   	}
    
	//Préparation de la socket
	int sock;
	//Sans local port
	if(argc == 5){
		sock = S_openSocket();
		if(sock == -1){
			perror("S_openSocket\n");
			return -1;
		}
	}

	//Avec local port
	else{
		sock = S_openAndBindSocket(atoi(argv[5]));
		if(sock == -1){
			perror("S_openAndBindSocket\n");
			return -1;
		}
	}

	int finished = 0;

	//Ouverture en lecture du fichier
	int input_fd = open(argv[2], O_RDONLY);
	if(input_fd < 0){      
		perror("Open\n"); 
		exit(1);    
	}

	//DATAGRAMME
	Distant_data d;
	d = malloc(sizeof(struct data_struct));
	d->seq_num = 0;
	//ACQUITTEMENT
	Ack a;
	a = malloc(sizeof(struct ack_struct));
	a->ack_num = 0;
	//TIMER
	struct timeval tv;
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	//SELECT
	fd_set readfds;
	int res_select;
	int nbd = sock + 1;

	//Lecture premier message : init
	if(lecture(&d->buff, input_fd) < 0){
		perror("Lecture\n");
		return -1;
	}

	//Envoi premier message : init
	if(S_sendMessage(sock, dist_addr, d, sizeof(struct data_struct)) < 0){
		perror("S_sendMessage\n");
		return -1;
	}

	//Protocole
	while(finished == 0){

		FD_ZERO(&readfds);
		FD_SET(sock, &readfds);

		res_select = select(nbd, &readfds, NULL, NULL, &tv);

		// TESTER LA VALEUR DE RETOUR DE SELECT
		// ERREUR
		if(res_select < 0){
			perror("SELECT\n");
			return -1;
		}

		// TIMEOUT --> send again
		else if(res_select == 0){
			if(S_sendMessage(sock, dist_addr, d, sizeof(struct data_struct)) < 0){
				perror("S_sendMessage\n");
				return -1;
			}
		}

		// ACK RECU
		else if(FD_ISSET(sock, &readfds)){
		
			if(S_receiveMessage(sock, dist_addr, a, sizeof(struct ack_struct)) < 0){
				perror("S_recieveMessage\n");
				return -1;
			}

			// RECU LE BON ACK --> send the next one
			if((a->ack_num == ((d->seq_num + 1) % 2))){

				// LIRE LE PROCHAIN
				if(lecture(&d->buff, input_fd) < 0){
					perror("Lecture\n");
					return -1;
				}

				// PASSER AU PROCHAIN SEQ
				d->seq_num = (d->seq_num + 1) % 2;

				// ENVOYER
				if(S_sendMessage(sock, dist_addr, d, sizeof(struct data_struct)) < 0){
					perror("S_sendMessage\n");
					return -1;
				}
			}

			// THIS IS THE END
			else if(a->ack_num == ID){
				printf("All sent :D\n");
				finished = 1;
			}

			// RECU LE MEME ACK QUE LE SEQ --> send again
			else{
				if(S_sendMessage(sock, dist_addr, a, sizeof(struct data_struct)) == -1){
					perror("S_sendMessage\n");
					return -1;
				}
			}
		}

		// RESTART TIMER
		tv.tv_sec = 2;
		tv.tv_usec = 0;
	}

	close(input_fd);
	close(sock);
	free(dist_addr);
	free(d);

	return 0;
}
