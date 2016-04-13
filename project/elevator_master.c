//elevator_master


#include <pthread.h>
#include <semaphore.h>
#include "elev.h"
#include <stdlib.h>
#include <std::vector.h>

using namespace std;

int elevator_movement_state[number_of_elevators][3];
vector order_queue;


if(master){
	//connection, setup, initialization, etc...
	//So comes the elevator management
	while(1){

		message_recieve=network_recieve();
		if(message_recieve.type==BUTTONPRESS){
	       for(int floor = 0;floor<4;floor++){     //floor
		        for(int command = 0;command<3;command++){ //command
		            if(message_recieve.button_state[floor][command]){
		            		queue_add(message.ID,floor,command);
		            }
		        }
		    }            	
		}
		else if(message_recieve.type==STATUS){
			elevator_movement_state[message_recieve.ID][0]=message_recieve.floor_current;
			elevator_movement_state[message_recieve.ID][1]=message_recieve.direction;
			elevator_movement_state[message_recieve.ID][2]=message_recieve.destination;
		}
	}
}

else


//status på heisene


//cost function

/*
bruke vectorer til å pushe og poppe inn og ut nye bestillinger.
en vector for kø
en matrise for status for alle tilkoblede heiser


må ha execute funksjon som håndterer nye bestillinger, 
denne må tilordne hvem som skal utføre hva


*/

void queue_add(int ID, int floor, command){
	if(command==BUTTON_COMMAND){//inside elevator
		
	}
}