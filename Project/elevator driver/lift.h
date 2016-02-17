

//elevator module interface:
#include "elev.h"

#define ELEVATOR 1
#define BACKUP 2
#define STATUS 3
#define BUTTON_INDICATOR 4

struct Elevator {
   int floor_current;
   int destination;
   int reached_destination=0;
   int direction=DIRN_STOP:
   int new_floor_order=0;
};
sturct Message {
	int type;
	Elevator elevator;
	floor_indicator[4][3];
}

//SLAVE:
//Shared variables
Message message;
Elevator elevator;
Communication communication;

void * communication(){
	if(network_connect()){
		
		while(1){
			if(network_recieve(&message)){
				if(message.type==ELEVATOR){
					if(message.elevator.new_floor_order){
						pthread_mutex_lock(&mutex);
							elevator.new_floor_order=message.elevator.new_floor_order;
						pthread_mutex_unlock(&mutex);
					}
				}
				else if(message.type==BACKUP){
					//Then message is to be saved to backup ...	
				}
				else if(message.tpye==STATUS){
					//STATUS - generate status message
					message.status.floor_current=elevator.floor_current;
					message.status.motor_power=elevator.direction;
					message.status.reached_destination=elevator_reached_destination;
					//Etc...
					communication_send_status(&elevator);
				}
				else if(message.type==BUTTON_INDICATOR){
					//Turn on lamps that matches the button press matrix
					for(int floor =0;floor<4;floor++){ //floor
						for(int command=0;command<3;command++){ //command type
							if(message.floor_indicator[command][floor]){
								elev_set_button_lamp(command,i);
							}
						}
					}
				}
			}
			if(communication.buttons_are_pressed){
				network_send(&button_state);
				pthread_mutex_lock(&mutex);
					communication.buttons_are_pressed=0;
				pthread_mutex_unlock(&mutex);
			}
		}
	}
}

// Init floor == 0, init direction == DIRN_STOP
void * elevator(){
	elev_init();

	elevator.direction=DIRN_STOP;
	while(1){
		if(elevator.new_floor_order){
			elevator.floor_current=elev_get_floor_sensor_signal();
			elevator.destination=elevator.new_floor_order;
			pthread_mutex_lock(&mutex);
				elevator.new_floor_order=0;
			pthread_mutex_unlock(&mutex);
			
			if(elevator.destination < elevator.floor_current) //Go downwards
				direction = DIRN_DOWN;
			else if (elevator.destination > elevator.floor_current) //Go upwards
				direction = DIRN_UP;
			elev_set_motor_direction(direction);
			elevator_reached_destination=0;
			while(floor__current!=elevator.destination){
				elevator.floor_current=elev_get_floor_sensor_signal();
				if(elevator.floor_current>=0){
					elev_set_floor_indelevator.icator(elevator.floor_current-direction,0);
					elev_set_floor_indicator(elevator.floor_current,1);
				}
				if(elevator.new_floor_order){
					elevator.destination=elevator.nelevator.ew_floor_order;
					pthread_mutex_lock(&mutex);
						elevator.new_floor_order=0;
					pthread_mutex_unlock(&mutex);
				}
			}
			elevator.direction=DIRN_STOP;
			elev_set_motor_direction(elevator.direction);
			elevator.reached_destination=1;
			elev_set_door_open_lamp(1);
			delay(5);
			elev_set_door_open_lamp(0);
		}
	}
}

void * buttons (){
	while (1){
		int[4][3] button_matrix={0};
		int button_is_pressed=0;
		for(int floor = 0;floor<4;floor++){ 	//floor
			for(int command = 0;command<3;command++){ //command
				if(elev_get_button_signal(command,floor)==1)	{
					button_is_pressed=1;
					button_matrix[i][command] = 1;
					//printf("Button: Floor: %d Command: %d\n",floor,command);
				}
			}
		}
		if(button_is_pressed){
			pthread_mutex_lock(&mutex);
				communication.buttons_are_pressed=1;
				communication.button_state=button_matrix;
			pthread_mutex_unlock(&mutex);
		}
	}		
	
}
