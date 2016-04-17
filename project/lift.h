#ifndef LIFT_H
#define LIFT_H

#include <stdio.h>
#include <string.h> 
#include <stdlib.h> 
#include "elev.h"


int client_ID;

void* client_message_handler(void *msg_recv);
void button_check();
void elevator_thread();
void tcp_recieve(void *socket_desc);
void *tcp_send(void *transmit);
void check_network();

#endif
