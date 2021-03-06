#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "fonction_reseau.h"

int main(int argc, char* argv[]){

	if(argc != 4){
        printf("Usage : %s  <domain: 4 --> IPv4, 6 --> IPv6> <filename> <local_port>\n", argv[0]);
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
		return 1;
	
    struct sockaddr *dist_addr = malloc(sizeof(struct sockaddr));

	//Préparation de la socket
	int sock = S_openAndBindSocket(atoi(argv[3]));
	if(sock < 0){
		perror("S_openAndBindSocket");
		return -1;
	}

	//Ouverture fichier en écriture
	int output_fd = open(argv[2], O_TRUNC|O_CREAT|O_WRONLY, 0755);
	if(output_fd < 0)
	{	
		perror("Open"); 
		exit(1); 		
	}

	Buffer buf = malloc(sizeof(struct buffer));
	int finished = 0;
	while(finished == 0)
	{
		//Réception du message
		if(S_receiveMessage(sock, dist_addr, buf, sizeof(struct buffer)) == -1){
			perror("S_receiveMessage");
			return -1;
		}

		//Copie du fichier
		if(write(output_fd, buf->msg, buf->taille) == -1){
			perror("Write");
			return -1;	
		}
		//Fin de l'écriture
		if(buf->taille < BUFFER_LENGTH)
			finished = 1;
	}	

	printf("Echange terminé !\n");

	close(output_fd);
	free(buf);
	free(dist_addr);
	close(sock);

	return 0;
}
