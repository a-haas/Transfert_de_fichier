#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>

int S_DOMAIN = 0;

int S_openAndBindSocket(int local_port){
	//Création de la socket
	int sock = socket(S_DOMAIN, SOCK_DGRAM, 0);
	if(sock == -1){
		return -1;
	}
	//Binding
	struct sockaddr_in6 sa6;
	struct sockaddr_in sa;

	if(S_DOMAIN == AF_INET)
	{
		sa.sin_family = S_DOMAIN;
		sa.sin_port = htons(local_port);
		sa.sin_addr.s_addr = htonl(INADDR_ANY);
	
		if(bind(sock, (struct sockaddr*)&sa, sizeof(struct sockaddr_in)) == -1)
			return -1;
	}

	else
	{
		sa6.sin6_family = S_DOMAIN;
		sa6.sin6_port = htons(local_port);
		sa6.sin6_addr = in6addr_any;
	
		if(bind(sock, (struct sockaddr*)&sa6, sizeof(struct sockaddr_in6)) == -1)
			return -1;
	}

	return sock;
}

int S_openSocket(void){
	return socket(S_DOMAIN, SOCK_DGRAM, 0);
}

int S_distantAddress(char* IP_address, int port, struct sockaddr** dist_addr){
    struct sockaddr_in* sa;
	struct sockaddr_in6* sa6;

	if(S_DOMAIN == AF_INET)
	{
		sa = malloc(sizeof(struct sockaddr_in)); 
  		sa->sin_family = AF_INET;
   		sa->sin_port = htons(port);
    	if(inet_pton(S_DOMAIN, IP_address, &sa->sin_addr.s_addr) == -1)
    		return -1;
    	*dist_addr = (struct sockaddr*) sa;
	}

	else
	{
		sa6 = malloc(sizeof(struct sockaddr_in6));
		sa6->sin6_family = AF_INET6;
		sa6->sin6_port = htons(port);
		if(inet_pton(S_DOMAIN, IP_address, &sa6->sin6_addr.s6_addr) == -1)
			return -1;
		*dist_addr = (struct sockaddr*) sa6;
	}

    return 0;
}

int S_receiveMessage(int sock_fd, struct sockaddr* dist_addr, void* msg, int length){
	socklen_t fromlen;

	if(S_DOMAIN == AF_INET)
	{
		fromlen = sizeof(struct sockaddr_in);
	}
	else
	{
		fromlen = sizeof(struct sockaddr_in6);
	}

	return recvfrom(sock_fd, msg, length, 0, dist_addr, &fromlen);
}

int S_sendMessage (int sock_fd, struct sockaddr* dist_addr, void* msg, int length){
	if(S_DOMAIN == AF_INET)
		sendto(sock_fd, msg, length, 0, dist_addr, sizeof(struct sockaddr_in));

	else	
		sendto(sock_fd, msg, length, 0, dist_addr, sizeof(struct sockaddr_in6));

	return 0;
}
