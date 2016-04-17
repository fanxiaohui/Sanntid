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

int new_conn = 0;
optval = 1;
clients = 0;
int flag = 0;
optval;
connection;
network;
*master_socket;
client_sockets[3];
int recv_client = 0;
my_ID; 
queue[6][2]={0};
Elevator elev_client[3];
network = 1;
pthread_mutex_t mutex;



void tcp_listen(){
    int socket_desc , new_socket , c , *new_sock;
    struct sockaddr_in server , client;
    char message[BUFSIZE];

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    setsockopt(socket_desc, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( PORT );
     
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("bind failed");
        return 1;
    }

    listen(socket_desc , 3);
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while( (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");
        clients++;
        printf("#Clients: %d\n",clients); 
        //Reply to the client
        strcpy(message,"Hello Client");
        write(new_socket , message , strlen(message));
        bzero(message,strlen(message));

        //Assign handler to new client
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = new_socket;
        client_sockets[clients-1] = new_socket;
        if( pthread_create( &sniffer_thread , NULL , master_connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
        puts("Handler assigned");
    }
     
    if (new_socket<0)
    {
        perror("accept failed");
        return -1;

    } 
}

void *tcp_send(void *transmit){
  char *buf = (char*)transmit;
  char msg[BUFSIZE];
  bzero(msg,BUFSIZE);
  for(int i = 0;i<BUFSIZE;i++){
    msg[i] = buf[i];
  } 
  if(network)
      write(*master_socket, msg, strlen(msg));
}

void tcp_recieve(void *socket_desc){
  int sock = *(int*)socket_desc;
  //Set up timed socket
  struct timeval tv;
  tv.tv_sec = 11; 
  tv.tv_usec = 0;
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,(char *)&tv,sizeof(tv)); 
  char buf[BUFSIZE];
  char empty[BUFSIZE];
  bzero(empty, BUFSIZE);
  bzero(buf, BUFSIZE);
  while(connection){
    if(connection)
      recv(sock, buf , BUFSIZE,0);
    if(!strcmp(buf,empty)){
        puts("Recv failed, master unavailable");
        connection = 0;
      }
    else{
      char msg[BUFSIZE];
      strcpy(msg,buf);
      for(int i = 0;i<BUFSIZE;i++){
          if(buf[i] == '0'){
            buf[i] = 0;
          }
       }
    strcpy(msg,buf);
    pthread_t client_message_handler_thread;
    pthread_create(&client_message_handler_thread, NULL , client_message_handler, (void*)msg);
    }
    bzero(buf, BUFSIZE);
  }
  free(socket_desc);    
}

void udp_listen(){
  int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  struct sockaddr_in sa; 
  char buffer[1024];
  char buffer2[1024];
  
  strcpy(buffer2, "Noen gangstas her?");
  ssize_t recsize;
  socklen_t fromlen;

  memset(&sa, 0, sizeof sa);
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = htonl(INADDR_ANY);
  sa.sin_port = htons(PORT);
  fromlen = sizeof(sa);
  setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
  if (-1 == bind(sock, (struct sockaddr *)&sa, sizeof sa)) {
    perror("error bind failed");
    close(sock);
    exit(EXIT_FAILURE);
  }

  for (;;) {
    recsize = recvfrom(sock, (void*)buffer, sizeof buffer, 0, (struct sockaddr*)&sa, &fromlen);
    if (recsize < 0) {
      exit(EXIT_FAILURE);
     }

    if(!strcmp(buffer2,buffer)){
        printf("New connection discovered\n");
        printf("Recieved: %.*s\n", (int)recsize, buffer);
        pthread_mutex_lock(&mutex);
        new_conn = 1;
        pthread_mutex_unlock(&mutex);
        bzero(buffer, 1024);
      }
    }
    return NULL;
}

void udp_send(){
  int sock;
  struct sockaddr_in sa;
  int bytes_sent;
  int s = sizeof(sa);
  char buffer[200];
  strcpy(buffer, "Halla");

  //Get broadcast address
  int fd;
  struct ifreq ifr;
  fd = socket(AF_INET, SOCK_DGRAM, 0);
  ifr.ifr_addr.sa_family = AF_INET;
  strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
  ioctl(fd, SIOCGIFBRDADDR, &ifr);
  close(fd);
  char* BROADCASTIP = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_broadaddr)->sin_addr);

  //Set up socket
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  int broadcastEnable=1;
  int ret=setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));

  if (-1 == sock) {
      printf("Error Creating Socket");
      exit(EXIT_FAILURE);
    }
 
  memset(&sa, 0, sizeof sa);
  
  sa.sin_family = AF_INET;
  sa.sin_port = htons(PORT);

  if(inet_aton(BROADCASTIP, &sa.sin_addr) ==0)
   {
    exit(1);
   }

  for(;;){
    if(new_conn == 1){
      for(int i = 0;i<10;i++){
        bytes_sent = sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr*)&sa, s);
        if (bytes_sent < 0) {
          exit(EXIT_FAILURE);
        }    
      }
      printf("Packets sent \n");
      pthread_mutex_lock(&mutex);
      new_conn = 0;
      pthread_mutex_unlock(&mutex);
      printf("Waiting for new connections\n");
    }
  }
  close(sock);

  return NULL;
}

void serialization(int message_type, Message message_send, char *transmit){
  message_send.type = message_type;

  transmit[0] = message_send.ID;
  transmit[1] = message_send.type;
  transmit[2] = message_send.elevator.floor_current;
  transmit[3] = message_send.elevator.next_direction;
  transmit[4] = message_send.elevator.prev_direction;
  transmit[5] = message_send.elevator.direction;
  transmit[6] = message_send.elevator.new_floor_order;
  transmit[7] = message_send.elevator.queue[0];
  transmit[8] = message_send.elevator.queue[1];
  transmit[9] = message_send.elevator.queue[2];
  transmit[10] = message_send.elevator.queue[3];
  transmit[11] = message_send.button.floor;
  transmit[12] = message_send.button.type;

  for(int i = 0;i<6;i++){
    transmit[13+i] = message_send.orders[i][0];
    transmit[19+i] = message_send.orders[i][1];
  }

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
  message_receive.elevator.next_direction=msg_recv[3];
  message_receive.elevator.prev_direction=msg_recv[4];
  message_receive.elevator.direction=msg_recv[5];
  message_receive.elevator.new_floor_order=msg_recv[6];
  message_receive.elevator.queue[0] = msg_recv[7];
  message_receive.elevator.queue[1] = msg_recv[8];
  message_receive.elevator.queue[2] = msg_recv[9];
  message_receive.elevator.queue[3] = msg_recv[10];
  message_receive.button.floor = msg_recv[11];
  message_receive.button.type = msg_recv [12];

  for(int i = 0;i<6;i++){
    message_receive.orders[i][0] = msg_recv[13+i];
    message_receive.orders[i][1] = msg_recv[19+i];
  }
  return message_receive;
}

