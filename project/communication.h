#ifndef COMMUNICATION_H
#define COMMUNICATION_H

//#include "elev.h"
//MESSAGE TYPES
#define ELEV_ORDER 1
#define BUTTON_CHECK 2
#define BUTTON_LAMP 3
#define ELEV_UPDATE 4
#define BACKUP 5
#define ORDER_UPDATE 6
#define ID_UPDATE 7
#define RECOVER_LOCAL_ORDERS 8

typedef struct {
   int floor_current;
   int reached_destination;
   elev_motor_direction_t direction;
   elev_motor_direction_t prev_direction;
   int new_floor_order;
   int queue[4];
}Elevator;
typedef struct {
	int floor;
	int type;
}Button;
typedef struct {
	int ID; 
	int type;
	Elevator elevator;
	Button button;
	int orders[6][2];	
}Message;

extern int *master_socket;
extern int client_sockets[3];
extern int queue[6][2];
extern int connection;
extern Elevator elev_client[3];
extern int clients;
extern int my_ID;
#define BUFSIZE 25

// Hirearchy:
// Communication
//	->Network protocol

void serialization(int message_type, Message message_send, char *transmit);
Message deserialization(char *msg_recv);

//Move to network protocol, or only inside the network_module, not in header ... 
void* udp_listen();
void* udp_send();
void *tcp_send(void *transmit);
void tcp_recieve(void *socket_desc);
void tcp_listen();

//Move boss_init and client_init inside connection_init
int connection_init(void);
void boss_init();
void client_init();

//change name to master_messge_handler or make new function that does the master's logic operation?
void *tcp_connection_handler(void *socket_desc);
void* client_message_handler(void *msg_recv);
//void* client_message_handler(void * message_received);

void* communication();
void send_message(int message_type, Message message_send);








#endif