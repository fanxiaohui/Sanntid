#ifndef MAIN_H
#define MAIN_H
#include "lift.h"

extern int *master_socket;
extern int client_sockets[3];
extern int queue[6][2];
extern int connection;
extern Elevator elev_client[3];
extern int clients;
extern int my_ID;
#define BUFSIZE 25


void *tcp_connection_handler(void *socket_desc);
void tcp_listen();
void connection_init(void);
void* udp_listen();
void* udp_send();
void *tcp_send(void *transmit);
void tcp_recieve(void *socket_desc);
void client_init();
void* communication();
int add_to_queue(elev_button_type_t type, int  floor);
void boss_init();
#endif