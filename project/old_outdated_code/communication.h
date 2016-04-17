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
#define NETCHECK 9

#define PORT 20017
#define BUFSIZE 25

typedef struct {
   int floor_current;
   elev_motor_direction_t direction;
   elev_motor_direction_t prev_direction;
   elev_motor_direction_t next_direction;
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
extern int optval;
extern int network;


// Hirearchy:
// Communication
//	->Network protocol

void serialization(int message_type, Message message_send, char *transmit);
Message deserialization(char *msg_recv);

void *tcp_connection_handler(void *socket_desc);
void check_clients();

void udp_listen();
void udp_send();
void tcp_listen();



#endif
