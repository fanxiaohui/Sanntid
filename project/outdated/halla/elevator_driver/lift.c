#include "lift.h"
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "elev.h"
#include "main.h"
//SLAVE:
//Shared variables
Message message_send,message_recieve;
Elevator elevator;
//Button button;

pthread_mutex_t mutex;
//pthread_mutex_init(&mutex,NULL);

char transmit[9];

//RECIEVING THREAD
void* message_handler(void *message){
	puts("aell");
	//char *buf = (char *)message;
	//printf("Message recieved: %s",message);
	/*switch(message_recieve.type){
		case ELEVATOR:
			if(message_recieve.elevator.new_floor_order){
				pthread_mutex_lock(&mutex);
					elevator.new_floor_order=message_recieve.elevator.new_floor_order;
				pthread_mutex_unlock(&mutex);
			}
			if(message_recieve.elevator.reached_destination){
				if(message_recieve.elevator.destination==3) //TOP FLOOR
					elev_set_button_lamp(BUTTON_CALL_UP,message_recieve.elevator.destination,0);
				else if(message_recieve.elevator.destination==0)//BOTTOM FLOOR
					elev_set_button_lamp(BUTTON_CALL_UP,message_recieve.elevator.destination,0);
				else if(message_recieve.elevator.direction==-1)//GOING DOWN
					elev_set_button_lamp(BUTTON_CALL_DOWN,message_recieve.elevator.destination,0);	
				else if(message_recieve.elevator.direction==1)//GOING UP
					elev_set_button_lamp(BUTTON_CALL_UP,message_recieve.elevator.destination,0);		
			}
			break;		
		case BUTTON_LAMP: //Auto sets the elevator order button lamp, not the ones inside.
		    for (int f = 0; f < N_FLOORS; f++) {
		        for (elev_button_type_t b = 0; b < N_BUTTONS-1; b++){ 
		            elev_set_button_lamp(b, f, button.matrix[(int)b][f]);
		        }
     		}
 			break;
	}*/
}
				


// Init floor == 0, init direction == DIRN_STOP
void elevator_thread(){
	elev_init();
	elevator.direction=0;
	elevator.new_floor_order=-1;
	elevator.reached_destination=0;

	while(1){
		if(elevator.new_floor_order=!-1){
			elevator.floor_current=elev_get_floor_sensor_signal();
			elevator.destination=elevator.new_floor_order;
			pthread_mutex_lock(&mutex);
				elevator.new_floor_order=-1;
			pthread_mutex_unlock(&mutex);
			
			if(elevator.destination < elevator.floor_current){ //Go downwards
	        	elev_set_motor_direction(DIRN_DOWN);
	        	elevator.direction=-1; //DOWN
	        }
	       else if (elevator.destination > elevator.floor_current){ //Go upwards
	        	elev_set_motor_direction(DIRN_UP);
	        	elevator.direction=1;
	        }
	        elevator.reached_destination=0;
			while(elevator.floor_current!=elevator.destination){
				elevator.floor_current=elev_get_floor_sensor_signal();
				if(elevator.floor_current>=0){
					elev_set_floor_indicator(elevator.floor_current);
				}
				if(elevator.new_floor_order!=-1){
					elevator.destination=elevator.new_floor_order;
					pthread_mutex_lock(&mutex);
						elevator.new_floor_order=0;
					pthread_mutex_unlock(&mutex);
				}
			}
			elev_set_motor_direction(DIRN_STOP);
			
		    elevator.reached_destination=1; //Tells master the lift has arrived
		    //SPAWN THREAD TO DO THE SENNDING
		    elev_set_door_open_lamp(1);
		    elev_set_button_lamp(BUTTON_INSIDE,elevator.destination,0);
		    sleep(5);
		    elev_set_door_open_lamp(0);
		    elevator.direction=0;
		}
	}
}


void button_check(){
	elev_button_type_t button_type;
	int floor;
	message_send.button.button_type=-1;
	message_send.button.floor=-1;
	int button_is_pressed=0;
	puts("Button check");
	while (1){
		for(floor = 0;floor<N_FLOORS;floor++){ 	//floor
			for(button_type = 0;button_type<N_BUTTONS;button_type++){ //button_type
				if(elev_get_button_signal(button_type,floor)==1){
					if(button_type==BUTTON_INSIDE){ 
						elev_set_button_lamp(BUTTON_INSIDE,floor,1);
					}
					pthread_mutex_lock(&mutex);
					button_is_pressed=1;
					//sleep(1);
					message_send.button.floor=floor;
					message_send.button.button_type=button_type;
					pthread_mutex_unlock(&mutex);
					while(elev_get_button_signal(button_type,floor));
					break;
					//printf("Button: Floor: %d button_type: %d\n",floor,button_type);
				}
			}
			if(button_is_pressed)
				break;
		}
		if(button_is_pressed){ 
			
			puts("Button is pressed, creating thread\n");
			//while(elev_get_button_signal(button_type,floor));
			button_is_pressed=0;
		//SPAWN THREAD TO DO THE SENNDING
		// network_send(&button.matrix);
			serialization();
			pthread_t message_send_thread;
    		pthread_create(&message_send_thread, NULL , tcp_send, (void*)transmit);	
			message_send.button.floor=-1;
			message_send.button.button_type=-1;

		}

	} 		
	
}

void serialization(){
	message_send.ID = 7;
	
	transmit[0] = message_send.ID;
	transmit[1] = message_send.type;
	transmit[2] = message_send.elevator.floor_current;
	transmit[3] = message_send.elevator.destination;
	transmit[4] = message_send.elevator.reached_destination;
	transmit[5] = message_send.elevator.direction;
	transmit[6] = message_send.elevator.new_floor_order;
	transmit[7]=message_send.button.floor;
	transmit[8]=message_send.button.button_type;
	printf("Message serialized:\n");
	for(int z = 0;z<9;z++){
		printf("%d",transmit[z]);
	}
	printf("\n");
	printf("Button floor: %d\n", transmit[7]);
	printf("Button type : %d\n", transmit[8]);
}
/*struct Message deserialization(char recieve[17]){

	message_recieve.ID=recieve[0];
	message_recieve.type=recieve[1];
	message_recieve.elevator.floor_current=recieve[2];
	message_recieve.elevator.destination=recieve[3];
	message_recieve.elevator.reached_destination=recieve[4];
	message_recieve.elevator.direction=recieve[5];
	message_recieve.elevator.new_floor_order=recieve[6];
	int i=7;
	for(int floor = 0;floor<4;floor++){ 	//floor
		for(elev_button_type_t button_type = 0;button_type<3;button_type++){ //button_type
			message_recieve.button.matrix[floor][(int)button_type]=recieve[i];
			i++;
		}
	}
}

*/
//serialization



