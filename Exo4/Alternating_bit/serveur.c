#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "fonction_reseau.h"

int main(int argc, char* argv[]){

	if(argc != 4){
        	printf("Usage : %s  <ip_version> <filename> <local_port>\n", argv[0]);
       		exit(0);
	}

	//IPv4
	if(atoi(argv[1]) == 4)
		S_DOMAIN = AF_INET;

	//IPv6
	else if(atoi(argv[1]) == 6)
		S_DOMAIN = AF_INET6;

	//invalid enter
	else
		return -1;
	
    	struct sockaddr *dist_addr = malloc(sizeof(struct sockaddr));

	//Préparation de la socket
	int sock = S_openAndBindSocket(atoi(argv[3]));

	if(sock < 0){
		perror("S_openAndBindSocket");
		return -1;
	}

	//Ouverture fichier en écriture
	int output_fd = open(argv[2], O_TRUNC|O_CREAT|O_WRONLY, 0755);
	if(output_fd < 0){	
		perror("Open"); 
		exit(1); 		
	}

	int finished = 0;
	int i;
	// DATAGRAMME
	Distant_data d;
	d = malloc(sizeof(struct data_struct));
	d->seq_num = 0;
	// ACQUITTEMENT
	Ack a;
	a = malloc(sizeof(struct ack_struct));
	a->ack_num = 0;

	// PROTOCOLE
	while(finished == 0)
	{
		// RECEPTION DE MESSAGE
		if(S_receiveMessage(sock, dist_addr, d, sizeof(struct data_struct)) < 0){
			perror("S_receiveMessage\n");
			return -1;
		}

		// RECU LE BON MESSAGE --> copy that
		if(d->seq_num == a->ack_num){

			printf("Message n°%d arrivé à bon port :\n", d->seq_num);

			// ECRIRE MESSAGE RECU
			if(write(output_fd, d->buff.msg, d->buff.taille) < 0){
				perror("Write\n");
				return -1;
			}

			// PASSER AU PROCHAI ATTENDU
			a->ack_num = (a->ack_num + 1) % 2;

			// ENVOYER LE PROCHAIN ACK
			if(S_sendMessage(sock, dist_addr, a, sizeof(struct ack_struct)) < 0){
				perror("S_sendMessage\n");
				return -1;
			}
		}

		// BAD
		else{
			if(S_sendMessage(sock, dist_addr, a, sizeof(struct ack_struct)) < 0){
				perror("S_sendMessage\n");
				return -1;
			}
		}

		// FIN
		if(d->buff.taille < BUFFER_LENGTH){
			finished = 1;
			a->ack_num = ID;

			for(i = 0; i < 20; i++){
				if(S_sendMessage(sock, dist_addr, a, sizeof(struct ack_struct)) < 0){
					perror("S_sendMessage\n");
					return -1;
				}
			}
		}
	}	

	printf("Echange terminé !\n");

	close(output_fd);
	free(d);
	free(dist_addr);
	close(sock);

	return 0;
}
