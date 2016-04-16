
#include <pthread.h> //for threading , link with lpthread
#include <semaphore.h>
#include <assert.h>
#include "main.h"
#include "lift.h"
#include "elev.h"
#include "communication.h"

int main(int argc , char *argv[])
{   
  
    

    int boss=connection_init();
    printf("Boss = %d\n", boss);
    if(boss == 1){

      

      //pthread_t tcp_listen_thread,udp_listen_thread,udp_send_thread,communication_thread,elev_thread;
      //pthread_create( &udp_listen_thread, NULL, udp_listen, NULL);
      //pthread_create( &udp_send_thread, NULL, udp_send, NULL);
      //pthread_create( &tcp_listen_thread, NULL, tcp_listen, NULL);
      //system("gnome-terminal -x ./test 0");

      //pthread_join( udp_listen_thread , NULL);
      //pthread_join( udp_send_thread , NULL);
      //pthread_join( tcp_listen_thread , NULL);

    }
    else{

      //client_init();
    }
    //pthread_t tcp_listen_thread,udp_listen_thread, connection_init_thread;
    //pthread_create( &connection_init_thread, NULL, connection_init, NULL);
    //pthread_create( &udp_listen_thread, NULL, udp_listen, NULL);
    //pthread_create( &tcp_listen_thread, NULL, tcp_listen, NULL);
     
    return 0;
}
