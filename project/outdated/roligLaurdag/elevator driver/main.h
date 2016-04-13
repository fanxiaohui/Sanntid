
extern int *master_socket;
extern int client_sockets[2];
#define BUFSIZE 1024
#include "lift.h"

void *tcp_connection_handler(void *socket_desc);
void tcp_listen();
void connection_init(void);
void* udp_listen();
void* udp_send();
void *tcp_send(void *transmit);
void tcp_recieve(void *socket_desc);
void client_init();
void* communication();
