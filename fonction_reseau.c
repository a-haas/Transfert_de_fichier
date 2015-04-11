#include <stdlib.h>
#include <arpa/inet.h>

int S_openAndBindSocket(int local_port){
	//Création de la socket
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == -1){
		return -1;
	}
	//Binding
	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(local_port);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(sock, (struct sockaddr*)&sa, sizeof(struct sockaddr_in)) == -1)
		return -1;
	return sock;
}

int S_openSocket(void){
	return socket(AF_INET, SOCK_DGRAM, 0);
}

int S_distantAddress(char* IP_address, int port, struct sockaddr** dist_addr){
    struct sockaddr_in* sa = malloc(sizeof(struct sockaddr_in)); 
  	sa->sin_family = AF_INET;
    sa->sin_port = htons(port);
    if(inet_pton(AF_INET, IP_address, &sa->sin_addr.s_addr) == -1)
    	return -1;
    *dist_addr = (struct sockaddr*) sa;
    return 0;
}

int S_receiveMessage(int sock_fd, struct sockaddr* dist_addr, void* msg, int length){
	socklen_t fromlen = sizeof(struct sockaddr_in);
	return recvfrom(sock_fd, msg, length, 0, dist_addr, &fromlen);
}

int S_sendMessage (int sock_fd, struct sockaddr* dist_addr, void* msg, int length){
	sendto(sock_fd, msg, length, 0, dist_addr, sizeof(struct sockaddr_in));
	return 0;
}