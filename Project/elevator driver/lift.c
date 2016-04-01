#include "lift.h"
#include <pthread.h>
#include <semaphore.h>

//SLAVE:
//Shared variables
Message message;
Elevator elevator;
Button button;




//RECIEVING THREAD
void  communication(){
	while(1){
		if(network_recieve(&message)){
			switch(message.type){
				case ELEVATOR:
					if(message.elevator.new_floor_order){
						pthread_mutex_lock(&mutex);
							elevator.new_floor_order=message.elevator.new_floor_order;
						pthread_mutex_unlock(&mutex);
					}
					if(message.elevator.reached_destination){
						if(message.elevator.destination==3) //TOP FLOOR
							elev_set_button_lamp(BUTTON_CALL_UP,message.elevator.destination,0);
						else if(message.elevator.destination==0)//BOTTOM FLOOR
							elev_set_button_lamp(BUTTON_CALL_UP,message.elevator.destination,0);
						else if(message.elevator.direction==-1)//GOING DOWN
							elev_set_button_lamp(BUTTON_CALL_DOWN,message.elevator.destination,0);	
						else if(message.elevator.direction==1)//GOING UP
							elev_set_button_lamp(BUTTON_CALL_UP,message.elevator.destination,0);		
					}
					break;		
				case BUTTON_LAMP: //Auto sets the elevator order button lamp, not the ones inside.
				    for (int f = 0; f < N_FLOORS; f++) {
				        for (elev_button_type_t b = 0; b < N_BUTTONS-1; b++){ 
				            elev_set_button_lamp(b, f, button.matrix[(int)b][f]);
				        }
		     		}
		 			break;
		 	}
		 }
	}
}
				


// Init floor == 0, init direction == DIRN_STOP
void  elevator(){
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
			while(floor__current!=elevator.destination){
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


void  buttons(){
	button.matrix={0};
	button_is_pressed=0;
	while (1){
		for(int floor = 0;floor<4;floor++){ 	//floor
			for(elev_button_type_t button_type = 0;button_type<3;button_type++){ //button_type
				if(elev_get_button_signal(button_type,floor)==1){
					if(button_type==BUTTON_INSIDE){ 
						elev_set_button_lamp(BUTTON_INSIDE,floor,1);
					}
					pthread_mutex_lock(&mutex);
					button_is_pressed=1;
					button.matrix[floor][(int)button_type] = 1;
					pthread_mutex_unlock(&mutex);
					//printf("Button: Floor: %d button_type: %d\n",floor,button_type);
				}
			}
		}
		if(button_is_pressed){ 
		//SPAWN THREAD TO DO THE SENNDING
			network_send(&button.matrix);
			button_matrix={0};
		}

	} 		
	
}





