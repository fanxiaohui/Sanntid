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

char* BROADCASTIP;
char* serv_ip;
*master_socket;
connection=0;

pthread_mutex_t mutex;

void connection_init(){
    pthread_mutex_init(&mutex,NULL); 
    int sock,sock_r;
    struct sockaddr_in sa;
    int bytes_sent;
    int s = sizeof(sa);
    char buffer[1024];
    char buffer2[1024];
    char buffer3[1024];
    bzero(buffer3,1024);
    
    strcpy(buffer,"Noen gangstas her?");
    strcpy(buffer2,"Halla");
    clients = 0;
    int fd;
    struct ifreq ifr;
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
    ioctl(fd, SIOCGIFBRDADDR, &ifr);
    close(fd);

    char* BROADCASTIP = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_broadaddr)->sin_addr);
    printf("%s\n",BROADCASTIP);

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
    sa.sin_addr.s_addr = inet_addr(&BROADCASTIP);
    sa.sin_port = htons(PORT);

    for(int i = 0;i<10;i++){
        bytes_sent = sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr*)&sa, s);
        if (bytes_sent < 0) 
            network = 0;
        else{
            network = 1;
            printf("Packet %d sent \n",(i+1));
        }
    }
    close(sock); 

    //UDP packets sent, awaiting answers 
    if(network){
      sock_r = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
      ssize_t recsize;
      socklen_t fromlen;

      memset(&sa, 0, sizeof sa);
      sa.sin_family = AF_INET;
      sa.sin_addr.s_addr = htonl(INADDR_ANY);
      sa.sin_port = htons(PORT);
      fromlen = sizeof(sa);

      setsockopt(sock_r, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
    

      if (-1 == bind(sock_r, (struct sockaddr *)&sa, sizeof sa)) {
          perror("error bind failed2");
          close(sock);
          exit(EXIT_FAILURE);
      }
      sleep(1);
      //Set up timed socket
      struct timeval tv;
      tv.tv_sec = 2;  /* 2 Secs Timeout */
      tv.tv_usec = 0;
      setsockopt(sock_r, SOL_SOCKET, SO_RCVTIMEO,(char *)&tv,sizeof(tv)); 

      recsize = recvfrom(sock_r, (void*)buffer3, sizeof buffer3, 0, (struct sockaddr*)&sa, &fromlen);

         if(!strcmp(buffer3,buffer2))
         {
            serv_ip = inet_ntoa(sa.sin_addr);
            printf("Master discovered, i'm slave :(\n");
            connection = 1;
            client_init();
         }
         else
         {
            printf("Master not discovered\n");
            printf("Recieved: %s\n", buffer3);
            master_init();

         }
    
      close(sock_r);
     
    }
    else{
      puts("No network");
      pthread_t elev_thread,button_thread, check_network_thread;
      connection = 1;
      pthread_create(&elev_thread, NULL, elevator_control, NULL);
      pthread_create(&button_thread, NULL, button_check, NULL);
      pthread_create(&check_network_thread, NULL, check_network, NULL);
      pthread_join(elev_thread,NULL);
      pthread_join(check_network_thread, NULL);
      puts("Elevator Thread Killed");
      pthread_join(button_thread, NULL);
      puts("Button Check Thread Killed");

      connection_init();
    }
}

void client_init(){
    int sockfd, portno, n;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname = serv_ip;
    char buf[BUFSIZE];
    char server_reply[50];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
    (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(PORT);

    if (connect(sockfd, &serveraddr, sizeof(serveraddr)) < 0) 
      error("ERROR connecting");

    if(read(sockfd, server_reply , BUFSIZE) < 0){
        printf("recv failed\n");
      }
    else{
      printf("TCP connection established\n");
      printf("%s\n", server_reply);
    }
    if(read(sockfd, server_reply , BUFSIZE) < 0){
        printf("recv failed\n");
    }
    else{ //Gets client ID from master
      my_ID = atoi(server_reply);
      printf("My ID is: %d\n", my_ID);
    }

    pthread_t client_thread1,elev_thread,button_thread;

    master_socket = malloc(1);
    *master_socket = sockfd;

    pthread_create(&client_thread1, NULL, tcp_recieve, (void*) master_socket);
    pthread_create(&elev_thread, NULL, elevator_control, NULL);
    pthread_create(&button_thread, NULL, button_check, NULL);
    
    pthread_join(client_thread1, NULL);
    puts("TCP Recieve killed");
    pthread_join(elev_thread,NULL);
    puts("Elevator Thread Killed");
    pthread_join(button_thread, NULL);
    puts("Button Check Thread Killed");
    close(sockfd);

    //If connections is lost to master, clients with lowest ID's tries to initialize first
    if(my_ID == 0){
      connection_init();
    }
    else if(my_ID == 1){
      sleep(5);
      connection_init();
    }
    else if(my_ID = 2){
      sleep(10);
      connection_init();
    }
}

void master_init(){

  pthread_t tcp_listen_thread,udp_listen_thread,udp_send_thread,check_clients_thread;
  pthread_create( &udp_listen_thread, NULL, udp_listen, NULL);
  pthread_create( &udp_send_thread, NULL, udp_send, NULL);
  pthread_create( &tcp_listen_thread, NULL, tcp_listen, NULL);
  pthread_create( &check_clients_thread, NULL, check_clients, NULL);
  system("gnome-terminal -x ./elevator 0");

  int sum = 0;
  for(int i = 0;i<6;i++){
    sum+=queue[i][0];
    queue[i][1] = -1;
  }
  sleep(14);
  
  if(sum != 0){
  	reallocate_orders(clients);
  }

  pthread_join( udp_listen_thread , NULL);
  pthread_join( udp_send_thread , NULL);
  pthread_join( tcp_listen_thread , NULL);
}

int main(int argc , char *argv[])
{   
  
    
    connection_init();
     
    return 0;
}