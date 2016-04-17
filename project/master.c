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


void *master_connection_handler(void *socket_desc){ 

  int client_id = clients-1;
  int sock = *(int*)socket_desc;
  int read_size;
  char client_message[BUFSIZE], *message;

  sleep(1);
  client_message[0] = (clients-1)+'0';
  write(sock , client_message , strlen(client_message)); //Give client ID
  bzero(client_message,BUFSIZE); 

  //Set up timed socket
  struct timeval tv;
  tv.tv_sec = 11;
  tv.tv_usec = 0;
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,(char *)&tv,sizeof(tv)); 

  while( (read_size = recv(sock , client_message , BUFSIZE , 0)) > 0 ){
    Message message_send,message_receive;
    message_receive = deserialization(client_message);

    switch(message_receive.type){
      case BUTTON_CHECK: 
          if(add_to_queue(message_receive.button.type, message_receive.button.floor)){

            char server_message[BUFSIZE];
            for(int i = 0;i<6;i++){
              message_send.orders[i][0] = queue[i][0];
              message_send.orders[i][1] = queue[i][1];
            }

            message_send.ID = cost_function(message_receive.button.type, message_receive.button.floor);
            message_send.elevator.new_floor_order=message_receive.button.floor;

            pthread_t order_counter_thread;
            char order_and_ID[2];
            order_and_ID[1] = message_send.ID;

            //Starts counter on each order
            if(message_receive.button.type==BUTTON_CALL_UP){
              queue[message_receive.button.floor][1] = message_send.ID;
              queue[message_receive.button.floor][0] = 1;
              message_send.orders[message_receive.button.floor][1] = message_send.ID;
              order_and_ID[0] = message_receive.button.floor;
              pthread_create(&order_counter_thread, NULL, order_counter, (void*)order_and_ID);
            }
            else{
              queue[message_receive.button.floor+2][1] = message_send.ID;
              queue[message_receive.button.floor+2][0] = 1;
              message_send.orders[message_receive.button.floor+2][1] = message_send.ID;
              order_and_ID[0] = message_receive.button.floor+2;
              pthread_create(&order_counter_thread, NULL, order_counter, (void*)order_and_ID);
            }

            serialization(ELEV_ORDER, message_send, server_message);
            for(int i = 0;i<clients;i++){
              write(client_sockets[i], server_message, strlen(server_message));  
            }
          }    
        break;
      case ELEV_UPDATE: //Updates elevator location

        elev_client[client_id].floor_current = message_receive.elevator.floor_current;
        elev_client[client_id].direction = message_receive.elevator.direction;

        break;
      case ORDER_UPDATE: //Clears orders that are done and sets lamps
        
        elev_client[client_id].floor_current = message_receive.elevator.floor_current;
        elev_client[client_id].direction = message_receive.elevator.direction;

        for(int i = 0;i<6;i++){
          queue[i][0] &= message_receive.orders[i][0];
          if(!(queue[i][0]&&message_receive.orders[i][0]))
            queue[i][1] = -1;

        }
        puts("\nNew queue");
        for(int i = 0;i<6;i++){
          printf("Order: %d \t ID: %d\n",queue[i][0],queue[i][1]);

        }
        message_send.ID = -1;
        for(int i = 0;i<6;i++){
            message_send.orders[i][0] = queue[i][0];
            message_send.orders[i][1] = queue[i][1];
        }
        char server_message[BUFSIZE];
        serialization(ELEV_ORDER, message_send, server_message);
        for(int i = 0;i<clients;i++){
            write(client_sockets[i], server_message, strlen(server_message));  
        }
        break;
      case NETCHECK:
        break;
      case ID_UPDATE:
        client_id = message_receive.ID;
        break;  
    }    
    bzero(client_message,BUFSIZE);
  }
    
  if(read_size == 0){
      puts("Client disconnected, terminal exit");
      clients--;
      fflush(stdout);
      printf("#Clients: %d\n",clients);
      if(client_id == 0){
        system("gnome-terminal -x ./elevator 0");
      }
      else{ 
        if((clients+1)==2 && client_id == 1){//Client 1 disconnected, client 2 will now be client 1
          elev_client[1].floor_current = elev_client[2].floor_current;
          Message message_send;
          message_send.type = ID_UPDATE;
          message_send.ID = client_id;
          client_sockets[1] = client_sockets[2];
          char server_message[BUFSIZE];
          serialization(ID_UPDATE, message_send, server_message);
          write(client_sockets[1], server_message, strlen(server_message));
        }   
      }
      sleep(1);
      reallocate_orders(clients); 

      Message message_send; 
      char server_message[BUFSIZE];
      for(int i = 0;i<6;i++){
        message_send.orders[i][0] = queue[i][0];
        message_send.orders[i][1] = queue[i][1];
      }
      serialization(ORDER_UPDATE, message_send, server_message);
      for(int i = 0;i<clients;i++){
          write(client_sockets[i], server_message, strlen(server_message));  
      }
  }
  else if(read_size == -1){
      clients--;
      perror("Network connection lost");
      printf("Client id: %d", client_id);
      printf("#Clients: %d\n",clients); 
      if((clients+1)==2 && client_id == 1){//Client 1 disconnected, client 2 will now be client 1
        elev_client[1].floor_current = elev_client[2].floor_current;
        Message message_send;
        message_send.type = ID_UPDATE;
        message_send.ID = client_id;
        client_sockets[1] = client_sockets[2];
        char server_message[BUFSIZE];
        serialization(ID_UPDATE, message_send, server_message);
        write(client_sockets[1], server_message, strlen(server_message));
      }
      sleep(1);
      reallocate_orders(clients); 

      Message message_send; 
      char server_message[BUFSIZE];
      for(int i = 0;i<6;i++){
        message_send.orders[i][0] = queue[i][0];
        message_send.orders[i][1] = queue[i][1];
      }
      serialization(ORDER_UPDATE, message_send, server_message);
      for(int i = 0;i<clients;i++){
          write(client_sockets[i], server_message, strlen(server_message));  
      }
  }
       
  //Free the socket pointer
  free(socket_desc);
   
  return 0;
}
void check_clients(){
  sleep(3);
  while(1){
    char net_check[BUFSIZE];
    Message message_send;
    serialization(NETCHECK, message_send, net_check);
    if(clients){
      for(int i = 0;i<clients;i++){
        write(client_sockets[i],net_check, strlen(net_check));
      }
      puts("NETCHECK");  
      sleep(10);
    }
  if(clients == 0){
    exit(1);
    }
  }
} 

int add_to_queue(elev_button_type_t type, int  floor){
  if(type == BUTTON_CALL_UP && !queue[floor][0]){
    queue[floor][0] = 1;
    return 1; 
  }
  else if(type == BUTTON_CALL_DOWN && !queue[floor+2][0]){
    queue[floor+2][0] = 1;
    return 1;
  }
  else
    return 0;
}

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

  //Order for floor 3
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

  //Order for floor 0
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

  if((client_top == -1) && (client_free == -1) && (client_bottom == -1) && (client_running == -1))
  {
    //All elevators are running in the wrong direction - assign to client 0
    return 0;
  }

  //Finds smallest delta
  switch (floor){
    case 0:
      if(delta_elev_bottom<delta_elev_free){
        return client_bottom;
      }
      else 
        return client_free;
      break;
    case 3:
      if(delta_elev_top<delta_elev_free){
        return client_top;
      }
      else
       return client_free;
      break;
    default:
      if(delta_elev_running<delta_elev_free){
        return client_running;
      }
      else
        return client_free;
    break;
  } 
}

void reallocate_orders(int number_of_clients){
  if(number_of_clients == 1){
    for(int i = 0;i<6;i++){
      if(queue[i][0] == 1){
        queue[i][1] = 0;
      }
    }
  }
  else if(number_of_clients == 2){
    for(int i = 0;i<3;i++){
      if(queue[i][0] == 1)
        queue[i][1] = 0;
    }
    for(int i = 3;i<6;i++){
      if(queue[i][0] == 1){
        queue[i][1] = 1;
      }
    }
  }

}

void *order_counter(void *x){
  char *buf = (char*)x;
  char server_message[BUFSIZE];
  Message message_send;
  char order_and_ID[2];
  for(int i = 0;i<2;i++){
    order_and_ID[i] = buf[i];
  }

  sleep(10*(4-clients)); //If order are done in this time, reassign

  if(queue[order_and_ID[0]][0]!=0 && queue[order_and_ID[0]][1]==order_and_ID[1]){//Check if order is not taken and ID is correct
    printf("Client: %d did not finish order", order_and_ID[1]);
    if(clients == 1){
      queue[order_and_ID[0]][1] = 0;
      for(int i = 0;i<6;i++){
        message_send.orders[i][0] = queue[i][0];
        message_send.orders[i][1] = queue[i][1]; 
      }
    }
    else{
      if(queue[order_and_ID[0]][1] == 0){
        queue[order_and_ID[0]][1] = 1;
        for(int i = 0;i<6;i++){
          message_send.orders[i][0] = queue[i][0];
          message_send.orders[i][1] = queue[i][1]; 
        }  
      }
      else{
        queue[order_and_ID[0]][1] = 0;
        for(int i = 0;i<6;i++){
          message_send.orders[i][0] = queue[i][0];
          message_send.orders[i][1] = queue[i][1]; 
        }  
      }
    }
    serialization(ORDER_UPDATE, message_send, server_message);
    write(client_sockets[0], server_message, strlen(server_message));   
  }
}

