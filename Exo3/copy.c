#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "fonction_reseau.h"

int main(int argc, char* argv[]) {
    
    if(argc < 5 || argc > 6){
        printf("Usage : %s  <domain: 4 --> IPv4, 6 --> IPv6> <filename> <distant_host> <distant_port> [<local_port>]\n", argv[0]);
        return -1;
    }

    struct sockaddr *dist_addr = malloc(sizeof(struct sockaddr));

    //Préparation de l'adresse distante
	/* ARGC = 5 */
	if(argc == 5)
	{
		if(atoi(argv[1]) == 4)
		{
			S_DOMAIN = AF_INET;
		}

		else if(atoi(argv[1]) == 6)
		{
			S_DOMAIN = AF_INET6;
		}

		else
		{
			return -1;
		}

    	if(S_distantAddress(argv[3], atoi(argv[4]), &dist_addr) == -1)
		{
        	perror("S_distantAddress\n");
        	return -1;
   		}
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
        if(sock == -1)
		{
            perror("S_openAndBindSocket\n");
            return -1;
        }
    }


    //Ouverture en lecture du fichier
    int input_fd = open(argv[2], O_RDONLY);
    if(input_fd < 0){      
        perror("Open\n"); 
        exit(1);    
    }

	/* ----------- SEND ---------- */
    int finished=0;
    Buffer buffer = malloc(sizeof(struct buffer));
    while(finished == 0){
        //Lecture du fichier
        int nb_lu = read(input_fd, buffer->msg, BUFFER_LENGTH);
        if(nb_lu == -1){
            perror("Read\n");
            return -1;
        }
        buffer->taille = nb_lu;
        //Fin de lecture
        if(nb_lu < BUFFER_LENGTH)
            finished = 1;
        
        //Envoi du buffer au serveur
        if(S_sendMessage(sock, dist_addr, buffer, sizeof(struct buffer)) == -1){
            perror("S_sendMessage\n");
            return -1;
        }
    }

    printf("Envoi du fichier complété !\n");

    close(input_fd);
    close(sock);
    free(dist_addr);
    free(buffer);

    return 0;
}
