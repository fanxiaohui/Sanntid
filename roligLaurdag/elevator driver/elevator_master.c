//elevator_master

#include "lift.h"
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "elev.h"
#include "main.h"

//The big cost function // ------------------


int queue[6][1];


switch(messeage_receive.type){
	case BUTTON_PRESS:
		switch(messeage_receive.button.type){
			case INSIDE:
				//call some function
				break;
			default:
				if(add_to_queue(& messeage_receive.button.type, & messeage_receive.button.floor))
					handler(cost_function(& messeage_receive));
				break;
		}

		}
		
}
esseage_receive
int add_to_queue(&elev_button_type_t type, & int  floor){
	if(type == BUTTON_CALL_UP && queue[floor]){
		return 0; 
	}
	else if(type == BUTTON_CALL_DOWN && queue[floor+2]){
		return 0,
	}
	else
		return 1;
}
cost_function(&messeage_receive);
Message cost_function(&Message messeage_receive){
	//If already in queue, exit
	//If not in queue, add it, designate it to an elevator and update the queue list

	//BUTTON_PRESS INSIDE: If this new floor is in the queue list and the same direction, designate it to this elevator

	for(int i =0;i<6;i++){
		if(messeage_receive.queue[i])
	}

}





//if elevator inside is going to outside ordered floor -> assing it
//if elevator is free -> assign it
vector Elevator elev_client;
elev_client.pushback

struct Elevator elev_client[3];


//add_to_queue returns positive -> new order from outside
int cost_function(& Message message_recv){
	int direction=messeage_recv.button.type;
	int floor = messeage_recv.button.floor;
	int delta=5; //Larger than possible delta FLOOR DIFFERANCE
	int client=-1; //Unavilable


//Elevator is available at the same floor
	for(int i=0;i<N_CLIENTS;i++){
		if(elev_client[i].direction == DIRN_STOP && elev_client[i]==floor){
		client=i;
	}
	if(client!=-1){
		return client;
		//Finished
	}


//Elevator runs in the ordrered direction, and is stopping at the same floor
	for(int i=0;i<N_CLIENTS;i++){
		if(elev_client[i].direction == direction && elev_client[i].queue[floor]==1){
			if(abs(elev_client[i].floor_current-floor)<delta){
				delta=abs(elev_client[i].floor_current-floor);
				client =i;
			}		
		}
	}
	if(client!=-1){
		return client;
		//Finished
	}


//Elevator runs in the ordererd direction, but is not stopping at the same floor
	for(int i=0;i<N_CLIENTS;i++){
		if(elev_client[i].direction == direction){
			if(abs(elev_client[i].floor_current-floor)<delta){
				delta=abs(elev_client[i].floor_current-floor);
				client =i;
			}
		}
	}
	if(client){
		return client;
		//Finished
	}

//Elevator is available, but at a different floor
	for(int i=0;i<N_CLIENTS;i++){
		if(elev_client[i].direction == DIRN_STOP){
			if(abs(elev_client[i].floor_current-floor)<delta){
				delta=abs(elev_client[i].floor_current-floor);
				client =i;
			}
		}	
	}
	if(client){
		return client;
		//Finished
	}
	//At this point I know that all elevators are running in the wrong direction
	// -> I assign client 1
	client =1;
	return client;
}



//recieve BUTTON PRESS OUTSIDE:

int add_to_queue(){
	int temp =	messeage_receive.button.floor + messeage_receive.button.type


	return if 
}