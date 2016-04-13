

//elevator module interface:
#ifndef LIFT_H
#define LIFT_H

#include <stdio.h>
#include <string.h> 
#include <stdlib.h> 
#include "elev.h"
//MESSAGE TYPES
#define ELEV_ORDER 1
#define BUTTON_CHECK 2
#define BUTTON_LAMP 3
#define ELEV_FLOOR_UPDATE 4
#define BACKUP 5
int client_ID;
typedef struct {
   int floor_current;
   int destination;
   int reached_destination;
   elev_motor_direction_t direction;
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
	int lamps_outside[6];	
}Message;


void* message_handler(void *msg_recv);
void button_check();
void elevator_thread();
void serialization(int message_type);
void deserialization();

//Example for deserialization of message
/*
char[17] recieve=tcp_recieve();
message.ID=recieve[0];
message.type=recieve[1];
message.elevator.floor_current=recieve[2];
message.elevator.destination=recieve[3];
message.elevator.reached_destination=recieve[4];
message.elevator.direction=recieve[5];
message.elevator.new_floor_order=recieve[6];
int i=7;
for(int floor = 0;floor<4;floor++){ 	//floor
	for(elev_button_type_t button_type = 0;button_type<3;button_type++){ //button_type
		message.button.matrix[floor][(int)button_type]=recieve[i];
		i++;
	}
}

//serialization
char[17] transmit;
transmit[0] = message.ID;
transmit[1] = message.type;
transmit[2] = message.elevator.floor_current;
transmit[3] = message.elevator.destination;
transmit[4] = message.elevator.reached_destination;
transmit[5] = message.elevator.direction;
transmit[6] = message.elevator.new_floor_order;
int i=7;
for(int floor = 0;floor<4;floor++){ 	//floor
	for(elev_button_type_t button_type = 0;button_type<3;button_type++){ //button_type
		transmit[i] = message.button.matrix[floor][(int)button_type];
		i++;
	}
}
*/
#endif