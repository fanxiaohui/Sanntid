#include "lift.h"
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "elev.h"
#include "main.h"
//SLAVE:
//Shared variables
/*
struct Message{
	int ID; 
	int type;
	Elevator elevator;
	Button button;
	int lamps_outside[6];	
};
*/


Elevator elevator;
//Button button;
pthread_mutex_t mutex;

//RECIEVING THREAD
void* message_handler(void *msg_recv){
	puts("aell");
	pthread_mutex_init(&mutex,NULL);
	char receive[BUFSIZE];
	//bzero(receive,BUFSIZE);
	char *buf = (char*)msg_recv;
	for(int i = 0;i<19;i++){
		receive[i] = buf[i];
	}
	/*
	for(int i = 0;i<19;i++){
		printf("Recieved: %d \n", receive[i]);
	}
	*/
	Message message_receive = deserialization(receive);

	switch(message_receive.type){
		case ELEV_ORDER:
			puts("ELEV ORDER");
			elev_set_button_lamp(BUTTON_CALL_UP, 0, message_receive.lamps_outside[0]);
			elev_set_button_lamp(BUTTON_CALL_UP, 1, message_receive.lamps_outside[1]);
			elev_set_button_lamp(BUTTON_CALL_UP, 2, message_receive.lamps_outside[2]);
			elev_set_button_lamp(BUTTON_CALL_DOWN, 1, message_receive.lamps_outside[3]);
			elev_set_button_lamp(BUTTON_CALL_DOWN, 2, message_receive.lamps_outside[4]);
			elev_set_button_lamp(BUTTON_CALL_DOWN, 3, message_receive.lamps_outside[5]);
			printf("Message_recieve.ID: %d\n", message_receive.ID);
			printf("My ID: %d\n", my_ID);
			if(message_receive.ID==my_ID){
				puts("Order recieved");
				pthread_mutex_lock(&mutex);
				elevator.queue[message_receive.elevator.new_floor_order]=1;
				pthread_mutex_unlock(&mutex);
				elevator.new_floor_order=1;
			}
			for(int i=0;i<N_FLOORS;i++){			
				printf("%d ", elevator.queue[i]);		
			}
			
			break;
		case BUTTON_CHECK:
			puts("BUTTON CHECKKI");

			break;
		case BUTTON_LAMP:
				puts("BUTTON LAMP");
				//for(int i = 0;i<6;i++){
					//printf("%d ",message_receive.lamps_outside[i]);
				//}
			    elev_set_button_lamp(BUTTON_CALL_UP, 0, message_receive.lamps_outside[0]);
				elev_set_button_lamp(BUTTON_CALL_UP, 1, message_receive.lamps_outside[1]);
				elev_set_button_lamp(BUTTON_CALL_UP, 2, message_receive.lamps_outside[2]);
				elev_set_button_lamp(BUTTON_CALL_DOWN, 1, message_receive.lamps_outside[3]);
				elev_set_button_lamp(BUTTON_CALL_DOWN, 2, message_receive.lamps_outside[4]);
				elev_set_button_lamp(BUTTON_CALL_DOWN, 3, message_receive.lamps_outside[5]);
			break;
		case ELEV_UPDATE:
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
	Message message_send;
	elev_init();
	elevator.floor_current = 0;
	elevator.direction=DIRN_STOP;

	message_send.elevator.direction = elevator.direction;
	message_send.elevator.floor_current = elevator.floor_current;
	char transmit[BUFSIZE];
	serialization(ELEV_UPDATE, message_send, transmit);
	pthread_t message_send_thread;
	pthread_create(&message_send_thread, NULL , tcp_send, (void*)transmit);
	int floor_update_not_sent=1;
	
	//elevator.new_floor_order=-1;
	elevator.reached_destination=-1;
	
	int sum=0;
	while(1){
		if(sum != 0){
			//puts("New order, elev thread");
			elevator.floor_current = elev_get_floor_sensor_signal();
			//elev_set_motor_direction(elevator.direction);
			int orders_up=0;
			int orders_down=0;
			if(elevator.direction == DIRN_STOP){
				for(int i=elevator.floor_current+1;i<N_FLOORS;i++){
					if(elevator.queue[i] == 1){
						orders_up++;
						puts("Order up");
					}

				}
				for( int i=elevator.floor_current-1;i>=0;i--){
					if(elevator.queue[i] == 1){
						orders_down++;
						puts("Order down");
					}
				}
				if (orders_up == 0 && orders_down == 0){
				elevator.direction=DIRN_STOP;
				}
				else if(orders_up>orders_down){
					elevator.direction=DIRN_UP;
					puts("DIRN UP");
				}
			
				else{
					elevator.direction=DIRN_DOWN;
					puts("DIRN DOWN");
				}
			}
			message_send.elevator.direction = elevator.direction;
			message_send.elevator.floor_current = elevator.floor_current;
			char transmit[BUFSIZE];
			serialization(ELEV_UPDATE, message_send, transmit);
			pthread_t message_send_thread;
			pthread_create(&message_send_thread, NULL , tcp_send, (void*)transmit);	

			elev_set_motor_direction(elevator.direction);
			
		}

		while(sum){
			elevator.floor_current=elev_get_floor_sensor_signal();
			if(elevator.floor_current!=-1){
				elev_set_floor_indicator(elevator.floor_current);
				if(elevator.queue[elevator.floor_current]==1){
					//stop
					elev_set_motor_direction(DIRN_STOP);
					elev_set_button_lamp(BUTTON_INSIDE,elevator.floor_current,0);

					elevator.queue[elevator.floor_current]=0;
					puts("\nElevator queue cleared");
					sum = 0;
					for(int i=0;i<N_FLOORS;i++){
						sum+=elevator.queue[i];
						printf("%d ", elevator.queue[i]);		
					}
					
					message_send.elevator.prev_direction = elevator.direction;	
					if(sum==0){
						puts("\nDIRN STOP");
						elevator.direction = DIRN_STOP;
					}
					else if(elevator.direction==DIRN_UP){
						sum=0;
						for(int i=elevator.floor_current;i<N_FLOORS;i++){
							sum+=elevator.queue[i];
						}
						if(sum==0)
							elevator.direction=DIRN_DOWN;
					}
					else if(elevator.direction==DIRN_DOWN){
						sum=0;
						for(int i=elevator.floor_current;i>=0;i--){
							sum+=elevator.queue[i];
						}
						if(sum == 0){
							elevator.direction=DIRN_UP;
						}
					}

					message_send.elevator.direction = elevator.direction;
					message_send.elevator.floor_current = elevator.floor_current;

					char transmit[BUFSIZE];
					serialization(ELEV_UPDATE, message_send, transmit);
					pthread_t message_send_thread;
					pthread_create(&message_send_thread, NULL , tcp_send, (void*)transmit);	
					floor_update_not_sent=0;
					elev_set_door_open_lamp(1);
				    elev_set_button_lamp(BUTTON_INSIDE,elevator.floor_current,0);
				    sleep(5);
				    elev_set_door_open_lamp(0);

				    elev_set_motor_direction(elevator.direction);
				}
				else if(floor_update_not_sent){
					message_send.elevator.direction = elevator.direction;
					message_send.elevator.floor_current = elevator.floor_current;
					char transmit[BUFSIZE];
					serialization(ELEV_UPDATE, message_send, transmit);
					pthread_t message_send_thread;
					pthread_create(&message_send_thread, NULL , tcp_send, (void*)transmit);	
					floor_update_not_sent=0;
				}
			}
			else
				floor_update_not_sent=1;
		}
	
		for(int i=0;i<N_FLOORS;i++){		
				sum+=elevator.queue[i];
		}

		if(!connection){
			break;
		}
	}
}

void button_check(){
	Message message_send;
	elev_button_type_t button_type;
	int floor;
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
			//message_send.type = BUTTON_CHECK;
			//SPAWN THREAD TO DO THE SENNDING
			char transmit[BUFSIZE];
			serialization(BUTTON_CHECK, message_send, transmit);
			pthread_t message_send_thread;
    		pthread_create(&message_send_thread, NULL , tcp_send, (void*)transmit);	

		}
		if(!connection){
			break;
		}

	} 			
}

void serialization(int message_type, Message message_send, char *transmit){
	//bzero(transmit,BUFSIZE);
	//message_send.ID = client_ID;
	message_send.type = message_type;
	//message_send.elevator.floor_current = -1;

	transmit[0] = message_send.ID;
	transmit[1] = message_send.type;
	transmit[2] = message_send.elevator.floor_current;
	transmit[3] = message_send.elevator.reached_destination;
	transmit[4] = message_send.elevator.prev_direction;
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
}

Message deserialization(char *msg_recv){
	for(int i = 0;i<BUFSIZE-1;i++){
		if(msg_recv[i] == '0')
			msg_recv[i] = 0;
	}
	Message message_receive;

	message_receive.ID=msg_recv[0];
	message_receive.type=msg_recv[1];
	message_receive.elevator.floor_current=msg_recv[2];
	message_receive.elevator.reached_destination=msg_recv[3];
	message_receive.elevator.prev_direction=msg_recv[4];
	message_receive.elevator.direction=msg_recv[5];
	message_receive.elevator.new_floor_order=msg_recv[6];
	message_receive.elevator.queue[0] = msg_recv[7];
	message_receive.elevator.queue[1] = msg_recv[8];
	message_receive.elevator.queue[2] = msg_recv[9];
	message_receive.elevator.queue[3] = msg_recv[10];
	message_receive.button.floor = msg_recv[11];
	message_receive.button.type = msg_recv [12];
	message_receive.lamps_outside[0] = msg_recv[13];
	message_receive.lamps_outside[1] = msg_recv[14];
	message_receive.lamps_outside[2] = msg_recv[15];
	message_receive.lamps_outside[3] = msg_recv[16];
	message_receive.lamps_outside[4] = msg_recv[17];
	message_receive.lamps_outside[5] = msg_recv[18];

	return message_receive;

}






