#include "lift.h"
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "elev.h"
#include "main.h"
//SLAVE:
//Shared variables
Message message_send,message_receive;
Elevator elevator;
//Button button;
client_ID = 96;
pthread_mutex_t mutex;
//pthread_mutex_init(&mutex,NULL);

char transmit[BUFSIZE];
char receive[BUFSIZE];

//RECIEVING THREAD
void* message_handler(void *msg_recv){
	puts("aell");
	//bzero(receive,BUFSIZE);
	char *buf = (char*)msg_recv;
	for(int i = 0;i<13;i++){
		receive[i] = buf[i];
	}

	for(int i = 0;i<13;i++){
		printf("Recieved: %d \n", receive[i]);
	}
	deserialization();

	switch(message_receive.type){
		case ELEV_ORDER:

			break;
		case BUTTON_CHECK:
			puts("BUTTON CHECKKI");
			elevator.queue[3]=1;
			elevator.direction=DIRN_UP;
			elevator.new_floor_order=1;
			break;
		case BUTTON_LAMP:
			    elev_set_button_lamp(BUTTON_CALL_UP, 0, message_receive.lamps_outside[0]);
				elev_set_button_lamp(BUTTON_CALL_DOWN, 1, message_receive.lamps_outside[1]);
				elev_set_button_lamp(BUTTON_CALL_UP, 1, message_receive.lamps_outside[1]);
				elev_set_button_lamp(BUTTON_CALL_DOWN, 2, message_receive.lamps_outside[2]);
				elev_set_button_lamp(BUTTON_CALL_UP, 2, message_receive.lamps_outside[2]);
				elev_set_button_lamp(BUTTON_CALL_DOWN, 3, message_receive.lamps_outside[3]);
			break;
		case ELEV_FLOOR_UPDATE:
			break;
		case BACKUP:
			break;

	}
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
				
void elevator_thread(){
	elev_init();
	elevator.floor_current = 1;
	elevator.direction=-1;
	elevator.new_floor_order=-1;
	elevator.reached_destination=-1;
	int sum=0;
	while(1){
		if(elevator.new_floor_order || sum != 0){
			elevator.new_floor_order=-1;
			sum = 0;
			for(int i=0;i<N_FLOORS;i++){
				elevator.queue[i]=message_receive.elevator.queue[i];		
				sum+=elevator.queue[i];
			}
			while(sum){
				
				elevator.floor_current=elev_get_floor_sensor_signal();
				if(elevator.floor_current=-1)
					elev_set_floor_indicator(elevator.floor_current);
				if(elevator.queue[elevator.floor_current]==1){
					//stop
					elev_set_motor_direction(DIRN_STOP);
					//spawn reached_floor thread with 

					serialization(ELEV_FLOOR_UPDATE);
					pthread_t message_send_thread;
    				pthread_create(&message_send_thread, NULL , tcp_send, (void*)transmit);	

					elev_set_door_open_lamp(1);
				    elev_set_button_lamp(BUTTON_INSIDE,elevator.destination,0);
				    sleep(5);
				    elev_set_door_open_lamp(0);
				    elevator.queue[elevator.floor_current]=0;
				    if(elevator.floor_current==0 || elevator.floor_current==3)
				    	break;
				}
				

				if(elevator.new_floor_order){
					for(int i=0;i<N_FLOORS;i++){
						elevator.queue[i]=message_receive.elevator.queue[i];		
					}
					elevator.new_floor_order = -1;
				}
				sum=0;
				if(elevator.direction==DIRN_UP){
					for(int i=elevator.floor_current;i<N_FLOORS;i++){
						sum+=elevator.queue[i];
					}
					if(sum!=0)
						elev_set_motor_direction(elevator.direction);
				}
				else if(elevator.direction==DIRN_DOWN){
					for(int i=elevator.floor_current;i>=0;i--){
						sum+=elevator.queue[i];
					}
					if(sum!=0)
						elev_set_motor_direction(elevator.direction);
				}
			}
			//Check for inside button press in the opposite direction
			for(int i=0;i<N_FLOORS;i++){		
				sum+=elevator.queue[i];
			}
			if(sum){
				if(elevator.direction==DIRN_UP)
					elevator.direction=DIRN_DOWN;
				else
					elevator.direction=DIRN_UP;
			}
		}
	}
}

void button_check(){
	elev_button_type_t button_type;
	int floor;
	message_send.button.type=-1;
	message_send.button.floor=-1;
	int button_is_pressed=0;
	puts("Button check");
	while (1){
		for(floor = 0;floor<N_FLOORS;floor++){ 	//floor
			for(button_type = 0;button_type<N_BUTTONS;button_type++){ //button_type
				if(elev_get_button_signal(button_type,floor)==1){
					if(button_type==BUTTON_INSIDE){ 
						elev_set_button_lamp(BUTTON_INSIDE,floor,1);
						elevator.queue[floor]=1;
						}
					pthread_mutex_lock(&mutex);
					button_is_pressed=1;
					//sleep(1);
					message_send.button.floor=floor;
					message_send.button.type=button_type;
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
			message_send.type = BUTTON_CHECK;
		//SPAWN THREAD TO DO THE SENNDING
			serialization(BUTTON_CHECK);
			pthread_t message_send_thread;
    		pthread_create(&message_send_thread, NULL , tcp_send, (void*)transmit);	
			message_send.button.floor=-1;
			message_send.button.type=-1;

		}

	} 			
}

void serialization(int message_type){
	bzero(transmit,BUFSIZE);
	message_send.ID = client_ID;
	message_send.type = message_type;
	message_send.elevator.floor_current = -1;
	transmit[0] = message_send.ID;
	transmit[1] = message_send.type;
	transmit[2] = message_send.elevator.floor_current;
	transmit[3] = message_send.elevator.destination;
	transmit[4] = message_send.elevator.reached_destination;
	transmit[5] = message_send.elevator.direction;
	transmit[6] = message_send.elevator.new_floor_order;
	transmit[7] = message_send.elevator.queue[0];
	transmit[8] = message_send.elevator.queue[1];
	transmit[9] = message_send.elevator.queue[2];
	transmit[10] = message_send.elevator.queue[3];
	transmit[11] = message_send.button.floor;
	transmit[12] = message_send.button.type;
	transmit[13] = message_send.lamps_outside[0];
	transmit[14] = message_send.lamps_outside[1];
	transmit[15] = message_send.lamps_outside[2];
	transmit[16] = message_send.lamps_outside[3];
	transmit[17] = message_send.lamps_outside[4];
	transmit[18] = message_send.lamps_outside[5];
	for(int i = 0;i<BUFSIZE-1;i++){
		if(transmit[i] == 0)
			transmit[i] = '0';
	}
	printf("\n");
	printf("Button floor: %d\n", transmit[11]);
	printf("Button type : %d\n", transmit[12]);
}

void deserialization(){
	for(int i = 0;i<BUFSIZE-1;i++){
		if(receive[i] == '0')
			receive[i] = 0;
	}
	message_receive.ID=receive[0];
	message_receive.type=receive[1];
	message_receive.elevator.floor_current=receive[2];
	message_receive.elevator.destination=receive[3];
	message_receive.elevator.reached_destination=receive[4];
	message_receive.elevator.direction=receive[5];
	message_receive.elevator.new_floor_order=receive[6];
	message_receive.elevator.queue[0] = receive[7];
	message_receive.elevator.queue[1] = receive[8];
	message_receive.elevator.queue[2] = receive[9];
	message_receive.elevator.queue[3] = receive[10];
	message_receive.button.floor = receive[11];
	message_receive.button.type = receive [12];
	message_receive.lamps_outside[0] = receive[13];
	message_receive.lamps_outside[1] = receive[14];
	message_receive.lamps_outside[2] = receive[15];
	message_receive.lamps_outside[3] = receive[16];
	message_receive.lamps_outside[4] = receive[17];
	message_receive.lamps_outside[5] = receive[18];



}






