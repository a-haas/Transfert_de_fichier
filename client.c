#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "fonction_reseau.h"
#include "type_buffer.h"

int main(int argc, char* argv[]) {
    
    if(argc < 4 || argc > 5){
        printf("Usage : %s <filename> <distant_host> <distant_port> [<local_port>]\n", argv[0]);
        return -1;
    }

    //Préparation de l'adresse distante
    struct sockaddr *dist_addr = malloc(sizeof(struct sockaddr));
    if(S_distantAddress(argv[2], atoi(argv[3]), &dist_addr) == -1){
        perror("S_distantAddress");
        return -1;
    }
    
    //Préparation de la socket
    int sock;
    //Sans local port
    if(argc == 4){
        sock = S_openSocket();
        if(sock == -1){
            perror("S_openSocket");
            return -1;
        }
    }
    //Avec local port
    else{
        sock = S_openAndBindSocket(atoi(argv[5]));
        if(sock == -1){
            perror("S_openAndBindSocket");
            return -1;
        }
    }

    //Ouverture en lecture du fichier
    int input_fd = open(argv[1], O_RDONLY);
    if(input_fd<0){      
        perror("Open"); 
        exit(1);    
    }

    int finished=0;
    Buffer buffer = malloc(sizeof(struct buffer));
    while(finished == 0){
        //Lecture du fichier
        int nb_lu = read(input_fd, buffer->msg, BUFFER_LENGTH);
        if(nb_lu == -1){
            perror("Read");
            return -1;
        }
        buffer->taille = nb_lu;
        //Fin de lecture
        if(nb_lu < BUFFER_LENGTH)
            finished = 1;
        
        //Envoi du buffer au serveur
        if(S_sendMessage(sock, dist_addr, buffer, sizeof(struct buffer)) == -1){
            perror("S_sendMessage");
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
