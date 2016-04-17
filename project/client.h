#ifndef CLIENT_H
#define CLIENT_H

void* client_message_handler(void *msg_recv); 
void button_check(); 		//Checks for buttonpresses, automatically adds internal orders to elevator queue
void elevator_control(); 	//Handles everything with the elevator
void check_network(); 		//If client has no network, this checks if network connection is restored

#endif