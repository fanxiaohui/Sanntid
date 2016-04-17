#include <stdio.h>
#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#ifdef __WIN32__
# include <winsock2.h>
#else
# include <sys/socket.h>
#endif
#include <unistd.h>    //write
#include <arpa/inet.h> //inet_addr
#include <pthread.h> //for threading , link with lpthread
#include <limits.h>
#include <ifaddrs.h>
#define _GNU_SOURCE 
#include <netdb.h>
#include <linux/if_link.h>
#include <strings.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <semaphore.h>
#include <assert.h>

#include "elev.h"
#include "communication.h"
#include "initializations.h"
#include "master.h"
#include "client.h"


Elevator elevator;
int elev_orders[6][2];
int master_checkin = 0;
connection;
network;
pthread_mutex_t mutex;


void* client_message_handler(void *msg_recv){
	puts("Client message handler started");
	pthread_mutex_init(&mutex,NULL);
	char receive[BUFSIZE];
	bzero(receive,BUFSIZE);
	char *buf = (char*)msg_recv;
	for(int i = 0;i<BUFSIZE;i++){
		receive[i] = buf[i];
	}
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
			puts("\nNew queue");
        	for(int i = 0;i<6;i++){
          		printf("Order: %d \t ID: %d\n",queue[i][0],queue[i][1]);
       		 }

			printf("Message_recieve.ID: %d\n", message_receive.ID);
			printf("My ID: %d\n", my_ID);

			if(message_receive.ID==my_ID){ //Sets local queue based on ID
				puts("Order recieved");
				pthread_mutex_lock(&mutex);
				if(elevator.queue[elevator.floor_current]==0){
					elevator.queue[message_receive.elevator.new_floor_order]=2;
				}
				pthread_mutex_unlock(&mutex);
				//elevator.new_floor_order=1;
			}
			
		
			break;
		case ORDER_UPDATE:
			puts("ORDER UPDATE");
			for(int i=0;i<6;i++){ //Global queue			 
				elev_orders[i][0] = message_receive.orders[i][0];
				elev_orders[i][1] = message_receive.orders[i][1];
				if(my_ID == message_receive.orders[i][1]){
					if(i == 0){
						if(elevator.queue[0]==0){
							elevator.queue[0] = 1;
						}
					}
					else if(i == 1 || i == 3){
						if(elevator.queue[1]==0){
							elevator.queue[1] = 1;
						}
					}
					else if(i == 2 || i == 4){
						if(elevator.queue[2]==0){
							elevator.queue[2] = 1;
						}
					}
					else if(i == 5)
						if(elevator.queue[3]==0){
							elevator.queue[3] = 1;
						}
				}
			}
			printf("Elev queue: ");
			for(int i = 0;i<N_FLOORS;i++){
				printf("%d ",elevator.queue[i]);
			}
			printf("\n");

			break;
		case BUTTON_LAMP:
				puts("BUTTON LAMP");
				elev_set_button_lamp(BUTTON_CALL_UP, 0, message_receive.orders[0][0]);
				elev_set_button_lamp(BUTTON_CALL_UP, 1, message_receive.orders[1][0]);
				elev_set_button_lamp(BUTTON_CALL_UP, 2, message_receive.orders[2][0]);
				elev_set_button_lamp(BUTTON_CALL_DOWN, 1, message_receive.orders[3][0]);
				elev_set_button_lamp(BUTTON_CALL_DOWN, 2, message_receive.orders[4][0]);
				elev_set_button_lamp(BUTTON_CALL_DOWN, 3, message_receive.orders[5][0]);
				break;
		case ID_UPDATE:
			puts("ID UPDATE");
			my_ID = message_receive.ID;
			char id_upd[BUFSIZE];
			Message message_send_ID;
			message_send_ID.ID = my_ID;
			serialization(ID_UPDATE, message_send_ID, id_upd);
			pthread_t message_send_thread_ID;
			pthread_create(&message_send_thread_ID, NULL , tcp_send, (void*)id_upd);
			break;
		case NETCHECK:
			puts("NETCHECK");
			master_checkin = 1;
			char net_check[BUFSIZE];
			Message message_send;
			serialization(NETCHECK, message_send, net_check);
			pthread_t message_send_thread;
			pthread_create(&message_send_thread, NULL , tcp_send, (void*)net_check);
			sleep(6);
			master_checkin = 0;
			sleep(5);
			if(master_checkin == 0){
				puts("Master disconnected\n");
				connection = 0;
			}
			break;
	}
}
				
void elevator_thread(){
	int sum=0;
	int stop=0;
	Message message_send;
 	elevator.queue[0] = 0;
	elevator.queue[1] = 0;
	elevator.queue[2] = 0;
	elevator.queue[3] = 0;
 	FILE *fp;
   	char buff[255];

   fp = fopen("local_order_queue.txt", "a+");
	for(int i=0;i<N_FLOORS;i++){
	   fscanf(fp, "%s", buff);
	   printf("%d : %s\n", i, buff );
	   if(buff[0] == '1'){
		   elevator.queue[i]=1;
		   sum++;
		}
		else
			elevator.queue[i]=0;
   	}
   fclose(fp);
	
	if(!sum){
		elev_init();
		elevator.floor_current = 0;
		elevator.direction=DIRN_STOP;
	}
	message_send.elevator.direction = elevator.direction;
	message_send.elevator.floor_current = elevator.floor_current;
	
	char transmit[BUFSIZE];
	serialization(ELEV_UPDATE, message_send, transmit);
	pthread_t message_send_thread;
	pthread_create(&message_send_thread, NULL , tcp_send, (void*)transmit);
	
	int floor_update_not_sent=1;
	while(1){
		if(sum != 0){
			elevator.floor_current = elev_get_floor_sensor_signal();
			int orders_up=0;
			int orders_down=0;
			if(elevator.direction == DIRN_STOP){
				for(int i=elevator.floor_current+1;i<N_FLOORS;i++){
					if(elevator.queue[i] != 0){
						orders_up++;
						puts("Order up");
					}

				}
				for( int i=elevator.floor_current-1;i>=0;i--){
					if(elevator.queue[i] != 0){
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
			for(int i=0;i<N_FLOORS;i++){
				message_send.elevator.queue[i]=elevator.queue[i];
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
				if((elevator.floor_current==0 && elevator.direction==DIRN_DOWN) || (elevator.floor_current==N_FLOORS&&elevator.direction==DIRN_UP)){
					elevator.direction=DIRN_STOP;
					elev_set_motor_direction(elevator.direction);
					stop=1;
				}
				if(elevator.queue[elevator.floor_current]==1){
					stop=1;
				}
				else if(elevator.queue[elevator.floor_current]==2){
					sum=0;
					if(elevator.direction==DIRN_UP){
						for(int i=elevator.floor_current;i<N_FLOORS;i++){
							if(elevator.queue[i]==1){
								sum++;
							}
						}
						switch (elevator.floor_current){
							case 1:
								if(elev_orders[elevator.floor_current][1]==my_ID){
									stop=1;
									break;
								}
							case 2:
							if(elev_orders[elevator.floor_current][1]==my_ID){
									stop=1;
									break;
								}
							default:
								if(sum==0){
									stop=1;
								}
						}
					}
					else if(elevator.direction==DIRN_DOWN){
						for(int i=elevator.floor_current;i>=0;i--){
							if(elevator.queue[i]==1){
								sum++;
							}
						}
						switch (elevator.floor_current){
							case 1:
								if(elev_orders[elevator.floor_current+2][1]==my_ID){
									stop=1;
									break;
								}
							case 2:
							if(elev_orders[elevator.floor_current+2][1]==my_ID){
									stop=1;
									break;
								}
							default:
								if(sum==0){
									stop=1;
								}
						}
					}					
				}
				if(stop){
					stop=0;
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

					FILE *fp;
			   		fp = fopen("local_order_queue.txt", "w+");
			   		fprintf(fp,"%d %d %d %d", elevator.queue[0], elevator.queue[1], elevator.queue[2], elevator.queue[3] );
			   		fclose(fp);
					
					message_send.elevator.prev_direction = elevator.direction;	
					elevator.direction = DIRN_STOP;
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
							elevator.next_direction = DIRN_UP;
							break;
						case 1:
							if(elev_orders[1][1] == my_ID){
								message_send.orders[1][0] = 0;
								message_send.orders[1][1] = my_ID;
								elevator.next_direction = DIRN_UP;
							}
							else {
								message_send.orders[3][0] = 0;
								message_send.orders[3][1] = my_ID;
								elevator.next_direction = DIRN_DOWN;
							}
							break;
						case 2:
							if(elev_orders[2][1] == my_ID){
								message_send.orders[2][0] = 0;
								message_send.orders[2][1] = my_ID;
								elevator.next_direction = DIRN_UP;
							}
							else {
								message_send.orders[4][0] = 0;
								message_send.orders[4][1] = my_ID;
								elevator.next_direction = DIRN_DOWN;
							}
							break;							
						case 3:
							message_send.orders[5][0] = 0;
							message_send.orders[5][1] = my_ID;
							elevator.next_direction = DIRN_DOWN;
							break;
					}
					puts("ORDER UPDATE");
       				for(int i = 0;i<6;i++){
          				printf("Order: %d \t ID: %d\n",message_send.orders[i][0], message_send.orders[i][1]);
        			}
					
					char transmit[BUFSIZE];
					serialization(ORDER_UPDATE, message_send, transmit);
					pthread_t message_send_thread;
					pthread_create(&message_send_thread, NULL , tcp_send, (void*)transmit);	
					
					floor_update_not_sent=0;
					elev_set_door_open_lamp(1);
				    elev_set_button_lamp(BUTTON_INSIDE,elevator.floor_current,0);
				    sleep(2);
				    elev_set_door_open_lamp(0);
				    sum = 0;
				    switch (elevator.next_direction){
				    	case DIRN_DOWN:
				    		for(int i = elevator.floor_current;i>=0;i--){
				    			sum += elevator.queue[i];
				    		}
				    		if(sum!=0){
				    			elevator.direction = DIRN_DOWN;
				    		}
				    		else {
				    			for(int i = elevator.floor_current;i<N_FLOORS;i++){
				    				sum+=elevator.queue[i];
				    			}
				    			if(sum!=0)
				    				elevator.direction = DIRN_UP;
				    		}
				    		break;
				    	case DIRN_UP:
				    		for(int i = elevator.floor_current;i<N_FLOORS;i++){
				    			sum += elevator.queue[i];
				    		}
				    		if(sum!=0){
				    			elevator.direction = DIRN_UP;
				    		}
				    		else {
				    			for(int i = elevator.floor_current;i>=0;i--){
				    				sum+=elevator.queue[i];
				    			}
				    			if(sum!=0)
				    				elevator.direction = DIRN_DOWN;
				    		}
				    		break;
				    }
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
	puts("Button check thread started");
	while (1){
		for(floor = 0;floor<N_FLOORS;floor++){ 
			for(button_type = 0;button_type<N_BUTTONS;button_type++){ 
				if(elev_get_button_signal(button_type,floor)==1){
					if(button_type==BUTTON_INSIDE){ 
						elev_set_button_lamp(BUTTON_INSIDE,floor,1);
						elevator.queue[floor]=1;
						}
					pthread_mutex_lock(&mutex);
					button_is_pressed=1;
					message_send.button.floor=floor;
					message_send.button.type=button_type;
					pthread_mutex_unlock(&mutex);
					while(elev_get_button_signal(button_type,floor));
					break;
				}
			}
			if(button_is_pressed)
				break;
		}
		if(button_is_pressed){ 
			
			puts("Button is pressed, creating thread\n");
			button_is_pressed=0;
			char transmit[BUFSIZE];
			serialization(BUTTON_CHECK, message_send, transmit);
			pthread_t message_send_thread;
    		pthread_create(&message_send_thread, NULL , tcp_send, (void*)transmit);	
			if(message_send.button.type==BUTTON_INSIDE){
				FILE *fp;
			   fp = fopen("local_order_queue.txt", "w+");
			   fprintf(fp,"%d %d %d %d", elevator.queue[0], elevator.queue[1], elevator.queue[2], elevator.queue[3] );
			   fclose(fp);
    		}
		}
		if(!connection){
			break;
		}

	} 			
}


void check_network(){
	
	while(!network){
		sleep(10);
		int fd;
	    struct ifreq ifr;
	    fd = socket(AF_INET, SOCK_DGRAM, 0);
	    // Get IPv4 address attached to "eth0" 
	    ifr.ifr_addr.sa_family = AF_INET;
	    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
	    ioctl(fd, SIOCGIFBRDADDR, &ifr);
	    close(fd);

	    char* no_network;
	    no_network = "0.0.0.0";
	    char* BROADCASTIP = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_broadaddr)->sin_addr);
	    printf("%s\n",BROADCASTIP);
	    if(strcmp(no_network,BROADCASTIP)){
	    	puts("Found network");
	    	network = 1;
	    	connection = 0;
	    }
	    sleep(10);
	}
}

