#ifndef __FRESEAU_H
#define __FRESEAU_H

int S_openAndBindSocket(int local_port);

int S_openSocket(void);

int S_distantAddress(char* IP_address, int port, struct sockaddr** dist_addr);

int S_receiveMessage(int sock_fd, struct sockaddr* dist_addr, void* msg, socklen_t length);

int S_sendMessage (int sock_fd, struct sockaddr* dist_addr, void* msg, socklen_t length);

#endif