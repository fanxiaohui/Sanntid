#ifndef CLIENT_H
#define CLIENT_H
//int client_ID;

void* client_message_handler(void *msg_recv);
void button_check();
void elevator_thread();
void check_network();

#endif