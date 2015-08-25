#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <time.h>
#include <sys/select.h>

#include "fonction_reseau.h"
#include "type_buffer.h"
#include "go_back_n.h"

int lecture(Buffer buffer, int input_fd){
    //Lecture du fichier
    int nb_lu = read(input_fd, buffer->msg, BUFFER_LENGTH);
    if(nb_lu == -1){
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
        printf("Usage : %s <ip_version> <filename> <distant_host> <distant_port> [local_port]\n", argv[0]);
        return -1;
    }   

    if(atoi(argv[1]) == 4)
        S_DOMAIN = AF_INET;
    else if(atoi(argv[1]) == 6)
        S_DOMAIN = AF_INET6;

    //Préparation de l'adresse distante
    struct sockaddr *dist_addr = malloc(sizeof(struct sockaddr));
    if(S_distantAddress(argv[3], atoi(argv[4]), &dist_addr) == -1){
        perror("S_distantAddress");
        return -1;
    }
    
    //Préparation de la socket
    int sock;
    //Sans local port
    if(argc == 5){
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
    int input_fd = open(argv[2], O_RDONLY);
    if(input_fd<0){      
        perror("Open"); 
        exit(1);    
    }

    //initialisation des variables pour GoBackN
    
    //Fenêtre d'envoi
    local_data fe[nFE];

    //Prochain Datagramme A Envoyer
    int pdae = 0;
    //Prochain Acquittement Attendu
    int paa = 1;

    int finished=0;
    struct buffer buffer;

    //Initialisation de la transmission
    for(int i=0; i<nFE; i++){
        finished = lecture(&buffer, input_fd);
        if(finished == -1){
            perror("Lecture");
            return -1;
        }
        //Remplissage data
        fe[i].buf = buffer;
        fe[i].seq = i;
        if(S_sendMessage(sock, dist_addr, &fe[i], sizeof(local_data)) == -1){
            perror("S_sendMessage");
            return -1;
        }
        if(finished == 0)
            pdae++;
    }
    //Variables nécessaires pour select 
    fd_set readfds;
    int nbd = sock+1;
    int res_select;
    Ack acquittement = malloc(sizeof(ack));
    //timer
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    int next_elt = 0; //Prochain élément à supprimer de la fenêtre
    int dernierData = -1;
    int no_more = 0;
    finished = 0;
    //Traiement du protocole
    while(finished == 0){
        //Vérification des acks ou sinon timeout
        FD_ZERO(&readfds); //vider readfds
        FD_SET(sock, &readfds);
        res_select = select(nbd, &readfds, NULL, NULL, &tv);
        if(res_select == -1){
            perror("Select");
            exit(1);
        }
        //Timeout renvoi de la fenêtre
        else if(res_select == 0){
            //renvoi fenetre
            for(int i=0; i<nFE; i++){
                if(S_sendMessage(sock, dist_addr, &fe[i], sizeof(local_data)) == -1){
                    perror("S_sendMessage");
                    return -1;
                }
            }   
        }
        //Sinon ack recu
        else if(FD_ISSET(sock, &readfds)){
            S_receiveMessage(sock, dist_addr, acquittement, sizeof(ack));
            //libérer place dans fenêtre
            for(int i=paa; i < acquittement->num_ack && no_more == 0; i++){
                if(lecture(&buffer, input_fd) == -1){
                    perror("Lecture");
                    return -1;
                }
                fe[next_elt].buf = buffer;
                fe[next_elt].seq = pdae;
                next_elt++;
                next_elt = next_elt%nFE;
                pdae++;
                paa++;
                if(buffer.taille < BUFFER_LENGTH){
                    no_more = 1;
                }
            }
            //envoi de la fenêtre
            for(int i=0; i<nFE; i++){
                if(S_sendMessage(sock, dist_addr, &fe[i], sizeof(local_data)) == -1){
                    perror("S_sendMessage");
                    return -1;
                }
            }
        }
        if(buffer.taille < BUFFER_LENGTH){
            dernierData = pdae;
        }
        if(dernierData == acquittement->num_ack){
            finished = 1;
        }
        //Remise à zéro du timeval
        tv.tv_sec = 2;
        tv.tv_usec = 0;
    }
    printf("Envoi du fichier complété !\n");

    //Fin de transmission
    
    close(input_fd);
    close(sock);
    free(dist_addr);
    free(acquittement);
    return 0;
}