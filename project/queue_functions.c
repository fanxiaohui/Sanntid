
#include "main.h"
#include <math.h>
#include "elev.h"



int add_to_queue(elev_button_type_t type, int  floor){
  if(type == BUTTON_CALL_UP && !queue[floor][1]){
    queue[floor][1] = 1;
    return 1; 
  }
  else if(type == BUTTON_CALL_DOWN && !queue[floor+2][1]){
    queue[floor+2][1] = 1;
    return 1;
  }
  else
    return 0;
}




//add_to_queue returns positive -> new order from outside
int cost_function(elev_button_type_t type, int floor){
  int direction;
  if(type==BUTTON_CALL_UP)
    direction = DIRN_UP;
  else
    direction = DIRN_DOWN;
  printf("DIRN: %d, FLOOR: %d\n", direction, floor);

  int delta_elev_free=5;
  int delta_elev_running=5; //Larger than possible delta FLOOR DIFFERANCE
  int delta_elev_top=5;
  int delta_elev_bottom=5;
  int client_running=-1,client_bottom=-1,client_top=-1,client_free = -1;//Unavilable

  //Elevator is available
  for(int i=0;i<clients;i++){
    if(elev_client[i].direction == DIRN_STOP){
      if(abs(elev_client[i].floor_current-floor)<delta_elev_free){
        delta_elev_free=abs(elev_client[i].floor_current-floor);
        client_free = i;
      }
    } 
  }


if(floor==3){
 for(int i=0;i<clients;i++){
    if(elev_client[i].direction == DIRN_UP && elev_client[i].queue[floor]==1){
      if(abs(elev_client[i].floor_current-floor)<delta_elev_top){
        delta_elev_top=abs(elev_client[i].floor_current-floor);
        client_top = i;
      }   
    }
  }
  if(client_top==-1){
    for(int i=0;i<clients;i++){
        if(elev_client[i].direction == DIRN_UP){
          if(abs(elev_client[i].floor_current-floor)<delta_elev_top){
            delta_elev_top=abs(elev_client[i].floor_current-floor);
            client_top = i;
          } 
        }
    }
  }
}

else if(floor==0){
 for(int i=0;i<clients;i++){
    if(elev_client[i].direction == DIRN_DOWN && elev_client[i].queue[floor]==1){
      if(abs(elev_client[i].floor_current-floor)<delta_elev_bottom){
        delta_elev_bottom=abs(elev_client[i].floor_current-floor);
        client_bottom =i;
      }   
    }
  }
  if(client_bottom==-1){
    for(int i=0;i<clients;i++){
      if(elev_client[i].direction == DIRN_DOWN){
        if(abs(elev_client[i].floor_current-floor)<delta_elev_bottom){
          delta_elev_bottom=abs(elev_client[i].floor_current-floor);
          client_bottom =i;
        }   
      }
    }
  }
}

/*
//Elevator runs in the ordrered direction, and is stopping at the same floor
  for(int i=0;i<clients;i++){
    if(direction == DIRN_UP && elev_client[i].direction == direction && elev_client[i].queue[floor]==1){
      if(abs(elev_client[i].floor_current-floor)<delta){
        delta=abs(elev_client[i].floor_current-floor);
        client =i;
      }   
    }
  }
  if(client!=-1){
    return client;
  }

*/

//Elevator runs in the ordererd direction, but is not stopping at the same floor
  for(int i=0;i<clients;i++){
    if(direction == DIRN_UP && elev_client[i].direction == DIRN_UP && elev_client[i].floor_current<=floor){
      if(abs(elev_client[i].floor_current-floor)<delta_elev_running){
        delta_elev_running=abs(elev_client[i].floor_current-floor);
        client_running =i;
      }
    }
  }


  //Elevator runs in the ordererd direction, but is not stopping at the same floor
  for(int i=0;i<clients;i++){
    if(direction == DIRN_DOWN && elev_client[i].direction == DIRN_DOWN && elev_client[i].floor_current>=floor){
      if(abs(elev_client[i].floor_current-floor)<delta_elev_running){
        delta_elev_running=abs(elev_client[i].floor_current-floor);
        client_running =i;
      }
    }
  }


  //At this point I know that all elevators are running in the wrong direction
  // -> I assign client 1

  switch (floor){
    case 0:
      if(delta_elev_bottom<delta_elev_free){
        return client_bottom;
      }
      else 
        return client_free;
  }
  case 3:
    if(delta_ osv....)

  return 0;
}