#ifndef __FRESEAU_H
#define __FRESEAU_H

extern int S_DOMAIN;

int S_openAndBindSocket(int local_port);

int S_openSocket(void);

int S_distantAddress(char* IP_address, int port, struct sockaddr** dist_addr);

int S_receiveMessage(int sock_fd, struct sockaddr* dist_addr, void* msg, int length);

int S_sendMessage (int sock_fd, struct sockaddr* dist_addr, void* msg, int length);

#endif