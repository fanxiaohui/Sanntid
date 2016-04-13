

//elevator module interface:
#include "elev.h"

//MESSAGE TYPES
#define ELEVATOR 1
#define BUTTON_PRESS 2
#define  BUTTON_LAMP 3
#define FLOOR_INDICATOR 4
#define BUTTON_PRESSED 5
#define BUTTON_INDICATOR 6



struct Elevator {
   int floor_current;
   int destination;
   int reached_destination;
   int direction;
   int new_floor_order;
};

struct Button {
	int[4][3] matrix;
};

struct Message{
	int ID; 
	int type;
	Elevator elevator;
	Button button;	
};



//Example for deserialization of message

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
