#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/select.h>

#include "fonction_reseau.h"
#include "type_buffer.h"
#include "go_back_n.h"

int main(int argc, char* argv[]){

	if(argc != 4){
        printf("Usage : %s <ip_version> <filename> <local_port>\n", argv[0]);
        exit(0);
    }

    if(atoi(argv[1]) == 4)
        S_DOMAIN = AF_INET;
    else if(atoi(argv[1]) == 6)
        S_DOMAIN = AF_INET6;
	
    struct sockaddr *dist_addr = malloc(sizeof(struct sockaddr));

	//Préparation de la socket
	int sock = S_openAndBindSocket(atoi(argv[3]));
	if(sock < 0){
		perror("S_openAndBindSocket");
		return -1;
	}

	//Ouverture fichier en écriture
	int output_fd = open(argv[2], O_TRUNC|O_CREAT|O_WRONLY, 0755);
	if(output_fd<0){	
		perror("Open"); 
		exit(1); 		
	}

	//variables réception
	Distant_data data = malloc(sizeof(local_data));
	Ack acquittement = malloc(sizeof(ack));
	int DA = 0;
	fd_set readfds;
	int nbd = sock+1;
	int res_select =0;
	struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
	int finished=0;
	while(finished == 0){
		//Réception du message
		if(S_receiveMessage(sock, dist_addr, data, sizeof(local_data)) == -1){
			perror("S_receiveMessage");
			return -1;
		}
		// fonction réception
		if(data->seq == DA){
			printf("Message n°%d arrivé à bon port !\n", data->seq);
			DA++;
			//Copie du fichier
			if(write(output_fd, data->buf.msg, data->buf.taille) == -1){
				perror("Write");
				return -1;	
			}
			//Fin de l'écriture
			if(data->buf.taille < BUFFER_LENGTH)
				finished = 1;
		}
		//Send ack
		acquittement->num_ack = DA;
		if(S_sendMessage(sock, dist_addr, acquittement, sizeof(ack)) == -1){
               perror("S_sendMessage");
               return -1;
        }
        res_select = 0;
		while(res_select == 0){
			FD_ZERO(&readfds); //vider readfds
			FD_SET(sock, &readfds);
		    res_select = select(nbd, &readfds, NULL, NULL, &tv);
		    if(res_select == -1){
		   	   	perror("Select");
		       	exit(1);
		   	}
		    //Timeout ack 
		    else if(res_select == 0){
				if(S_sendMessage(sock, dist_addr, acquittement, sizeof(ack)) == -1){
	               	perror("S_sendMessage");
	               	return -1;
	            }
	        }
	        //Remise à zéro du timeval
	        tv.tv_sec = 2;
	        tv.tv_usec = 0;
    	}    	
	}
	//Fin
	for(int i=0; i<20; i++){
		if(S_sendMessage(sock, dist_addr, acquittement, sizeof(ack)) == -1){
		    perror("S_sendMessage");
	       	return -1;
	    }	
	}
	printf("Echange du fichier terminé ! \n");
	close(output_fd);
	free(data);
	free(dist_addr);
	free(acquittement);
	close(sock);

	return 0;
}