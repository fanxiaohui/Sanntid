typedef enum tag_elev_motor_direction { 
    DIRN_DOWN = -1,
    DIRN_STOP = 0,
    DIRN_UP = 1
} elev_motor_direction_t;

typedef enum tag_elev_lamp_type { 
    BUTTON_CALL_UP = 0,
    BUTTON_CALL_DOWN = 1,
    BUTTON_INSIDE = 2
} elev_button_type_t;


typedef struct {
   int floor_current;
   int destination;
   int reached_destination;
   elev_motor_direction_t direction;
   int new_floor_order;
   int queue[4];
}Elevator;

int N_CLIENTS=3;
Elevator elev_client[3];

//add_to_queue returns positive -> new order from outside
int cost_function(elev_button_type_t type, int floor){
  puts("Cost function: ");
  int direction;
  if(type==BUTTON_CALL_UP)
    direction = DIRN_UP;
  else
    direction = DIRN_DOWN;
  if(direction==DIRN_DOWN)
      printf("DIRECTION: DOWN, FLOOR: %d\n", floor);
    else
      printf("DIRECTION: UP, FLOOR: %d\n", floor);

  int delta=5; //Larger than possible delta FLOOR DIFFERANCE
  int client=-1; //Unavilable



  //Elevator is available at the same floor
    for(int i=0;i<N_CLIENTS;i++){
      if(elev_client[i].direction == DIRN_STOP && elev_client[i].floor_current==floor){
        client=i;
      }
    }
    if(client!=-1){
      return client;
      //Finished
    }
  if(floor==3){
   for(int i=0;i<N_CLIENTS;i++){
      if(elev_client[i].direction == DIRN_UP && elev_client[i].queue[floor]==1){
        if(abs(elev_client[i].floor_current-floor)<delta){
          delta=abs(elev_client[i].floor_current-floor);
          client =i;
        }   
      }
    }
    if(client!=-1){
      return client;
    }
    for(int i=0;i<N_CLIENTS;i++){
        if(elev_client[i].direction == DIRN_UP){
          if(abs(elev_client[i].floor_current-floor)<delta){
            delta=abs(elev_client[i].floor_current-floor);
            client =i;
          } 
        }
    }
    if(client!=-1){
      return client;
    }
  }

  else if(floor==0){
   for(int i=0;i<N_CLIENTS;i++){
      if(elev_client[i].direction == DIRN_DOWN && elev_client[i].queue[floor]==1){
        if(abs(elev_client[i].floor_current-floor)<delta){
          delta=abs(elev_client[i].floor_current-floor);
          client =i;
        }   
      }
    }
    if(client!=-1){
      return client;
    }
    for(int i=0;i<N_CLIENTS;i++){
      if(elev_client[i].direction == DIRN_DOWN){
        if(abs(elev_client[i].floor_current-floor)<delta){
          delta=abs(elev_client[i].floor_current-floor);
          client =i;
        }   
      }
    }
    if(client!=-1){
    }

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
    if(client!=-1){
      return client;
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
    if(client=!-1){
      return client;
    }
    //At this point I know that all elevators are running in the wrong direction
    // -> I assign client 1
    return client;
}

int main(){
  elev_client[0].floor_current=0;
  elev_client[0].direction=DIRN_STOP;

  elev_client[1].floor_current=1;
  elev_client[1].direction=DIRN_UP;

  elev_client[2].floor_current=3;
  elev_client[2].direction=DIRN_STOP;
  
  printf("Cost_function: %d\n", cost_function(BUTTON_CALL_UP,0));
}