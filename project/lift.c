#include "lift.h"
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "elev.h"
#include "main.h"
#include "communication.h"
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
int elev_orders[6][2];
//RECIEVING THREAD
void* client_message_handler(void *msg_recv){
	puts("Client message handler started");
	pthread_mutex_init(&mutex,NULL);
	char receive[BUFSIZE];
	bzero(receive,BUFSIZE);
	char *buf = (char*)msg_recv;
	for(int i = 0;i<BUFSIZE;i++){
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
			elev_set_button_lamp(BUTTON_CALL_UP, 0, message_receive.orders[0][0]);
			elev_set_button_lamp(BUTTON_CALL_UP, 1, message_receive.orders[1][0]);
			elev_set_button_lamp(BUTTON_CALL_UP, 2, message_receive.orders[2][0]);
			elev_set_button_lamp(BUTTON_CALL_DOWN, 1, message_receive.orders[3][0]);
			elev_set_button_lamp(BUTTON_CALL_DOWN, 2, message_receive.orders[4][0]);
			elev_set_button_lamp(BUTTON_CALL_DOWN, 3, message_receive.orders[5][0]);

			for(int i=0;i<6;i++){ //Global queue			 
				elev_orders[i][0] = message_receive.orders[i][0];
				elev_orders[i][1] = message_receive.orders[i][1];
				queue[i][0] = message_receive.orders[i][0];
				queue[i][1] = message_receive.orders[i][1];
			}

			printf("Message_recieve.ID: %d\n", message_receive.ID);
			printf("My ID: %d\n", my_ID);

			if(message_receive.ID==my_ID){ //Sets local queue based on ID
				puts("Order recieved");
				pthread_mutex_lock(&mutex);
				elevator.queue[message_receive.elevator.new_floor_order]=1;
				pthread_mutex_unlock(&mutex);
				//elevator.new_floor_order=1;
			}
			
		
			break;
		case ORDER_UPDATE:
			for(int i=0;i<6;i++){ //Global queue			 
				elev_orders[i][0] = message_receive.orders[i][0];
				elev_orders[i][1] = message_receive.orders[i][1];
				if(my_ID == message_receive.orders[i][1]){
					if(i == 0)
						elevator.queue[0] = 1;
					else if(i == 1 || i == 3)
						elevator.queue[1] = 1;
					else if(i == 2 || i == 4)
						elevator.queue[2] = 1;
					else if(i == 5)
						elevator.queue[3] = 1;
				}
			}
			printf("Elev queue: ");
			for(int i = 0;i<4;i++){
				printf("%d ",elevator.queue[i]);
			}
			printf("\n");

			break;
		case BUTTON_LAMP:
				puts("BUTTON LAMP");
				//for(int i = 0;i<6;i++){
					//printf("%d ",message_receive.lamps_outside[i]);
				//}
			    elev_set_button_lamp(BUTTON_CALL_UP, 0, message_receive.orders[0][0]);
				elev_set_button_lamp(BUTTON_CALL_UP, 1, message_receive.orders[1][0]);
				elev_set_button_lamp(BUTTON_CALL_UP, 2, message_receive.orders[2][0]);
				elev_set_button_lamp(BUTTON_CALL_DOWN, 1, message_receive.orders[3][0]);
				elev_set_button_lamp(BUTTON_CALL_DOWN, 2, message_receive.orders[4][0]);
				elev_set_button_lamp(BUTTON_CALL_DOWN, 3, message_receive.orders[5][0]);
			break;
		case ID_UPDATE:
			my_ID = message_receive.ID;
			break;
		case BACKUP:
			break;

	}

}
				
void elevator_thread(){
	Message message_send;
	//elev_init();
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
			for(int i=0;i<4;i++){
				message_send.elevator.queue[i]=elevator.queue[i];
			}
			message_send.elevator.direction = elevator.direction;
			message_send.elevator.floor_current = elevator.floor_current;
			send_message(ELEV_UPDATE,message_send);
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
					printf("Elevator queue updated: \t");
					sum = 0;
					for(int i=0;i<N_FLOORS;i++){
						message_send.elevator.queue[i]=elevator.queue[i];
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
					printf("Floor current: %d\n", message_send.elevator.floor_current);

					for(int i = 0;i<6;i++){
						message_send.orders[i][0] = elev_orders[i][0];
						message_send.orders[i][1] = elev_orders[i][1];
					}

					switch (elevator.floor_current){
						case 0:
							message_send.orders[0][0] = 0;
							message_send.orders[0][1] = my_ID;
							break;
						case 1:
							if(elev_orders[1][1] == my_ID){
								message_send.orders[1][0] = 0;
								message_send.orders[1][1] = my_ID;
							}
							else {
								message_send.orders[3][0] = 0;
								message_send.orders[3][1] = my_ID;
							}
							//puts("Case 1");
							break;
						case 2:
							if(elev_orders[2][1] == my_ID){
								message_send.orders[2][0] = 0;
								message_send.orders[2][1] = my_ID;
							}
							else {
								message_send.orders[4][0] = 0;
								message_send.orders[4][1] = my_ID;
							}
							//puts("Case 2");
							break;							
						case 3:
							message_send.orders[5][0] = 0;
							message_send.orders[5][1] = my_ID;
							break;
					}
					puts("ORDER UPDATE");
       				 for(int i = 0;i<6;i++){
          				printf("Order: %d \t ID: %d\n",message_send.orders[i][0], message_send.orders[i][1]);

        				}
					send_message(ORDER_UPDATE,message_send);
					floor_update_not_sent=0;
					elev_set_door_open_lamp(1);
				    elev_set_button_lamp(BUTTON_INSIDE,elevator.floor_current,0);
				    sleep(2);
				    elev_set_door_open_lamp(0);

				    elev_set_motor_direction(elevator.direction);
				}
				else if(floor_update_not_sent){
					message_send.elevator.direction = elevator.direction;
					message_send.elevator.prev_direction = elevator.direction;
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
		/*	char transmit[BUFSIZE];
			serialization(BUTTON_CHECK, message_send, transmit);
			pthread_t message_send_thread;
    		pthread_create(&message_send_thread, NULL , tcp_send, (void*)transmit);	
		*/
    		send_message(BUTTON_CHECK, message_send);
		}
		if(!connection){
			break;
		}

	} 			
}






