


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
#include "main.h"
//#include "lift.h"
#include "elev.h"
#include "communication.h"


#define PORT 20017
//ssh gnome: ssh -X student@129.241.187.xxx
//#define SERVER "129.241.187.255"

char* BROADCASTIP;
int boss = -1;
int new_conn = 0;
char* serv_ip;
int optval = 1;

clients = 0;
int flag = 0;
connection = 0;
*master_socket;
int broadcast = 0;
int *sockets[2];
client_sockets[3];
char * client_ip[3];
struct recovery_local_orders{
  char* ip_table[3];
  int orders[3][4];
}recovery_local_orders;
char * client_ip[3];
int recv_client = 0;
my_ID; 
queue[6][2]={0};
Elevator elev_client[3];
int update_orders = 0;

pthread_mutex_t mutex;

void *tcp_connection_handler(void *socket_desc){ //Connection handler for TCP connections

  //Get the socket descriptor
  int client_id = clients-1;
  int sock = *(int*)socket_desc;
  int read_size;
  char client_message[BUFSIZE], *message;

  //Send some messages to the client
  //message = "Your ID is: " + (clients+'0');
  //write(sock , message , strlen(message));
  sleep(1);
  client_message[0] = (clients-1)+'0';
  write(sock , client_message , strlen(client_message));
  

  //write(sock , client_message , strlen(client_message)); //Give client ID
  bzero(client_message,BUFSIZE); 

  //Receive a message from client
  while( (read_size = recv(sock , client_message , BUFSIZE , 0)) > 0 ){
    broadcast = 0;
    Message message_send,message_receive;
    message_receive = deserialization(client_message);
    switch(message_receive.type){
      case BUTTON_CHECK: //ok
        
        printf("Reciever Button floor & type: ");
        printf("%d ",message_receive.button.floor);
        printf("%d \n",message_receive.button.type);
        switch(message_receive.button.type){
          case BUTTON_INSIDE:
            //Set order in queue?
            break;
          default:
            if(add_to_queue(message_receive.button.type, message_receive.button.floor)){

              char server_message[BUFSIZE];
              for(int i = 0;i<6;i++){
                message_send.orders[i][0] = queue[i][0];
                message_send.orders[i][1] = queue[i][1];
              }
            /*serialization(BUTTON_LAMP, message_send, lamp_message);
              for(int i = 0;i<clients;i++){
                write(client_sockets[i], lamp_message, strlen(lamp_message));
              }
              sleep(1);
              */
              /*
              if(elev_client[0].direction == DIRN_STOP){
                message_send.ID = 0;
              }
              else if(elev_client[1].direction == DIRN_STOP){
                message_send.ID = 1;
              }
              else if(elev_client[2].direction == DIRN_STOP){
                message_send.ID = 2;
              }
            */
              message_send.ID = cost_function(message_receive.button.type, message_receive.button.floor);
              printf("Which client gets the order: %d",message_send.ID);
              message_send.elevator.new_floor_order=message_receive.button.floor;
              if(message_receive.button.type==BUTTON_CALL_UP){
                queue[message_receive.button.floor][1] = message_send.ID;
                queue[message_receive.button.floor][0] = 1;
                message_send.orders[message_receive.button.floor][1] = message_send.ID;

              }
              else{
                queue[message_receive.button.floor+2][1] = message_send.ID;
                queue[message_receive.button.floor+2][0] = 1;
                message_send.orders[message_receive.button.floor+2][1] = message_send.ID;
              }

              serialization(ELEV_ORDER, message_send, server_message);
              for(int i = 0;i<clients;i++){
                write(client_sockets[i], server_message, strlen(server_message));  
              }
            } 
          }
              
        break;
      case ELEV_UPDATE: //ok
        //Only updates elevator location
      	printf("ELEV FLOOR UPDATE BY ELEVATOR #%d\n",client_id);

        elev_client[client_id].floor_current = message_receive.elevator.floor_current;
        elev_client[client_id].direction = message_receive.elevator.direction;

        for(int i = 0;i<clients;i++){
        	printf("Elevator #%d \t Floor current: %d \t Direction: %1d\n", i,elev_client[i].floor_current,(int)elev_client[i].direction); 
        }
        
      /*
        if(broadcast){
          char lamp_message[BUFSIZE];
          for(int i = 0;i<6;i++){
            message_send.lamps_outside[i] = queue[i][0];
          }
          serialization(BUTTON_LAMP, message_send, lamp_message);
          for(int i = 0;i<clients;i++){
            write(client_sockets[i], lamp_message, strlen(lamp_message));
          }
        }

        puts("\nNew queue");
        for(int i = 0;i<6;i++){
          printf("Order: %d \t ID: %d\n",queue[i][0],queue[i][1]);

        }
        puts("");
       */
        break;
      case ORDER_UPDATE:
      	//Clears orders that are done and sets lamps

      	elev_client[client_id].floor_current = message_receive.elevator.floor_current;
        elev_client[client_id].direction = message_receive.elevator.direction;
        for(int i = 0;i<4;i++){
          elev_client[client_id].queue[i]=message_receive.elevator.queue[i];
        }

        for(int i = 0;i<clients;i++){
        	printf("Elevator #%d \t Floor current: %d \t Direction: %1d\n", i,elev_client[i].floor_current,(int)elev_client[i].direction); 
        }

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
    }    
    bzero(client_message,BUFSIZE);
  }
    
  if(read_size == 0){
      puts("Client disconnected");
      clients--;
      fflush(stdout);
      for(int i=0;i<3;i++){
        if(recovery_local_orders.ip_table[i]=client_ip[client_id])
          for(int j=0;j<N_FLOORS;j++){
            recovery_local_orders.orders[i][j]=elev_client[client_id].queue[j];
          }
      }
      printf("#Clients: %d\n",clients); 
      if(client_id == 0){
        system("gnome-terminal -x ./test 0");
        
      }
      else{ 
      	if((clients+1)==2 && client_id == 1){//Client 1 disconnected, client 2 will now be client 1 
          elev_client[1].floor_current = elev_client[2].floor_current; 
          for(int i=0;i<N_FLOORS;i++){
            elev_client[1].queue[i]=elev_client[2].queue[i];
          }
      		Message message_send;
      		message_send.type = ID_UPDATE;
      		message_send.ID = client_id;
      		client_sockets[1] = client_sockets[2];
      		char server_message[BUFSIZE];
      		serialization(ID_UPDATE, message_send, server_message);
      		write(client_sockets[1], server_message, strlen(server_message));
      	}
      	//sleep(1);
      	reallocate_orders(clients);	
      }
  }
  else if(read_size == -1){
      clients--;
      perror("recv failed, handler");
      printf("#Clients: %d\n",clients); 
  }
       
  //Free the socket pointer
  free(socket_desc);
   
  return 0;
}

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
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( PORT );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("bind failed");
        return 1;
    }
    puts("bind done");

        //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
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
        //message[0] = clients + '0';
        //sleep(1);
        //write(new_socket , message , strlen(message)); //Give client ID [1,3]

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = new_socket;
        char *serv_ip =   inet_ntoa(server.sin_addr);
        char *client_ip = inet_ntoa(client.sin_addr);
        if(!strcmp(serv_ip,client_ip)){
          client_sockets[0]=new_socket;
          client_ip[0]=client_ip;
        }
        else
          client_sockets[clients-1] = new_socket;
          client_ip[clients-1]=client_ip;
        //*sockets[clients-1] = new_socket; 
        if( pthread_create( &sniffer_thread , NULL ,  tcp_connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return -1;
        }

        Message message_recovery;
        int sum=0;
        for(int i=0;i<clients;i++){
          if(!strcmp(recovery_local_orders.ip_table[i],client_ip)){
            for(int j=0;j<N_FLOORS;j++){
              if(recovery_local_orders.orders[i][j]){
                message_recovery.elevator.queue[j]=1;
                sum+=1;
              }
            }
          }   
        }
        if(sum){
          char transmit[BUFSIZE];
          serialization(RECOVER_LOCAL_ORDERS, message_recovery, transmit);
          write(client_sockets[clients-1], transmit, strlen(transmit));
        }
        puts("Handler assigned");
        //pthread_join( sniffer_thread , NULL);

        
    }
     
    if (new_socket<0)
    {
        perror("accept failed");
        return -1;

    } //Listens on TCP
}

void* udp_listen(){
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
      //fprintf(stderr, "%s\n", strerror(errno));
      exit(EXIT_FAILURE);
     }

    if(!strcmp(buffer2,buffer)){
        //New connection
        printf("New connection discovered\n");
        printf("Recieved: %.*s\n", (int)recsize, buffer);
        pthread_mutex_lock(&mutex);
        new_conn = 1;
        pthread_mutex_unlock(&mutex);
        strcpy(buffer, "asdf");
      }

    //printf("recsize: %zu\n ", recsize);
        //sleep(0.1);
    
    }
    return NULL; //Listens on UDP
}
void* udp_send(){
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
  /* I want to get an IPv4 IP address */
  ifr.ifr_addr.sa_family = AF_INET;
  /* I want IP address attached to "eth0" */
  strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
  ioctl(fd, SIOCGIFBRDADDR, &ifr);
  close(fd);
  char* BROADCASTIP = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_broadaddr)->sin_addr);

  //Set up socket
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  int broadcastEnable=1;
  int ret=setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));

  if (-1 == sock) {
      /* if socket failed to initialize, exit */
      printf("Error Creating Socket");
      exit(EXIT_FAILURE);
    }
 
  /* Zero out socket address */
  memset(&sa, 0, sizeof sa);
  
  /* The address is IPv4 */
  sa.sin_family = AF_INET;
  sa.sin_port = htons(PORT);
   /* IPv4 adresses is a uint32_t, convert a string representation of the octets to the appropriate     value */
  //sa.sin_addr.s_addr = inet_addr("129.241.187.255");
  //sa.sin_addr.s_addr = hton1(INADDR_ANY);
  
    
  /* sockets are unsigned shorts, htons(x) ensures x is in network byte order, set the port to 7654 */
  //sa.sin_port = htons(PORT);

  if(inet_aton(BROADCASTIP, &sa.sin_addr) ==0)
   {
    exit(1);
   }

  for(;;){
    if(new_conn == 1){
          //Sends over message, "Hello"
      for(int i = 0;i<10;i++){
        bytes_sent = sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr*)&sa, s);
        if (bytes_sent < 0) {
          //printf("Error sending packet: %s\n", strerror(errno));
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
  close(sock); /* close the socket */

  return NULL; //Broadcasts on UDP
}
void *tcp_send(void *transmit){
	//Get the ,socket descriptor

	char *buf = (char*)transmit;
   //strcpy(buf,transmit);
   // printf("In TCP SEND: Button floor: %d\n", buf[7]);
     //	printf("In TCP SEND: Button type : %d\n", buf[8]);

	 char buf2[BUFSIZE];
	 bzero(buf2,BUFSIZE);
	 for(int i = 0;i<BUFSIZE;i++){
		buf2[i] = buf[i];
	 }
    //char buf[9];
    //buf = transmit;
    //send the message line to the server 
    write(*master_socket, buf2, strlen(buf2));
}
void tcp_recieve(void *socket_desc){
  //Get the socket descriptor
  int sock = *(int*)socket_desc;
  char buf[BUFSIZE];
  char empty[BUFSIZE];
  bzero(empty, BUFSIZE);
  bzero(buf, BUFSIZE);
  //strcpy(empty," ");
  while(connection){
    
    if(connection)
    	recv(sock, buf , BUFSIZE,0);
    if(!strcmp(buf,empty)){
        printf("recv failed\n");
        connection = 0;
      }
    else{
      char msg[BUFSIZE];
      strcpy(msg,buf);
    	printf("Server message:\t");
    	for(int i = 0;i<BUFSIZE;i++){
        if(buf[i] == '0')
          buf[i] = 0;
  			 printf("%d ",buf[i]);
  		  }
		  printf("\n");
		  //char msg[BUFSIZE];
		  strcpy(msg,buf);
    //  Message message_received = deserialization(buf);
		  pthread_t client_message_handler_thread;
	    pthread_create(&client_message_handler_thread, NULL , client_message_handler, (void*)msg);
     //pthread_create(&client_message_handler_thread, NULL , client_message_handler, (void *)&message_received);	
	  }
    bzero(buf, BUFSIZE);
  }
  free(socket_desc);    
}
int connection_init(void){

    pthread_mutex_init(&mutex,NULL); //PLASSERTE DENNE HER FORDI DET ER LETTVINDT
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

   /* create an Internet, datagram, socket using UDP */
      //sock= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

      //sock = socket(AF_INET, SOCK_DGRAM, 0);
      //int broadcastEnable=1;
      //int ret=setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
      //setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
      //if (-1 == sock) {
        /* if socket failed to initialize, exit */
        //printf("Error Creating Socket");
        //exit(EXIT_FAILURE);
      //}

      /* Zero out socket address */
      //memset(&sa, 0, sizeof sa);

      /* The address is IPv4 */
      //sa.sin_family = AF_INET;
      //sa.sin_port = htons(PORT);
      /* IPv4 adresses is a uint32_t, convert a string representation of the octets to the appropriate     value */
      //sa.sin_addr.s_addr = inet_addr("129.241.187.255");
      //sa.sin_addr.s_addr = hton1(INADDR_ANY);


    /* sockets are unsigned shorts, htons(x) ensures x is in network byte order, set the port to 7654 */
      //sa.sin_port = htons(PORT);

      //if(inet_aton(SERVER, &sa.sin_addr) ==0)
      //{
      //exit(1);
    //}
  
    //NEW________
    int fd;
    struct ifreq ifr;
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;
    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
    ioctl(fd, SIOCGIFBRDADDR, &ifr);
    close(fd);

    char* BROADCASTIP = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_broadaddr)->sin_addr);
    printf("%s",BROADCASTIP);

    //Set up socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    int broadcastEnable=1;
    int ret=setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
    if (-1 == sock) {
        /* if socket failed to initialize, exit */
        printf("Error Creating Socket");
        exit(EXIT_FAILURE);
      }
 
    /* Zero out socket address */
    memset(&sa, 0, sizeof sa);
  
    /* The address is IPv4 */
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr(&BROADCASTIP);
    sa.sin_port = htons(PORT);

    for(int i = 0;i<10;i++){
        bytes_sent = sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr*)&sa, s);
        if (bytes_sent < 0) {
                printf("Error sending packet\n");
                exit(EXIT_FAILURE);

    }
        printf("Packet %d sent \n",(i+1));
    }
    close(sock); /* close the socket */

    //UDP packets sent, awaiting answers 

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
      /*if (recsize < 0) {
        //fprintf(stderr, "%s\n", strerror(errno));
        exit(EXIT_FAILURE);
       }*/
       if(!strcmp(buffer3,buffer2))
       {
          boss = 0;
          serv_ip = inet_ntoa(sa.sin_addr);
          printf("Master discovered, i'm slave :(\n");
          connection = 1;
          client_init();
       }
       else
       {
          printf("Master not discovered\n");
          printf("Recieved: %s\n", buffer3);
          boss_init();

       }
      //printf("recsize: %zu\n ", recsize);
      //sleep(0.1);
      //printf("datagram: %.*s\n", (int)recsize, buffer);
  
    if(boss == -1){
        boss = 1;
    } 
    close(sock_r);  //Checks if there exists a boss through UDP, if not set self to boss
    return boss;
}

void client_init(){
    int sockfd, portno, n;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname = serv_ip;
    char buf[BUFSIZE];
    char server_reply[50];

    // check command line arguments 
    //hostname = argv[1];
    //portno = atoi(argv[2]);

    // socket: create the socket 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
    //gethostbyname: get the server's DNS entry 
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

     //build the server's Internet address 
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
    (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(PORT);

    // connect: create a connection with the server 
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
    else{
      my_ID = atoi(server_reply);
      printf("My ID is: %d\n", my_ID);
    }
    //sleep(1);
    
    /*for(;;){

      printf("Please enter msg: ");
      fgets(buf, BUFSIZE, stdin); 
      n = write(sockfd, buf, strlen(buf));
      bzero(buf, BUFSIZE);

      bzero(server_reply, 6000); //Zero out recievebuffer
      if(read(sockfd, server_reply , BUFSIZE) < 0){
        printf("recv failed\n");
      }
      printf("Server reply: %s\n", server_reply);
      
    }
    */
    //Client threads
	 //pthread_create( &sniffer_thread , NULL ,  tcp_connection_handler , (void*) new_sock) < 0)
    //void *tcp_connection_handler(void *socket_desc) //Connection handler for TCP connections
	 //{
    //Get the socket descriptor
    //int sock = *(int*)socket_desc;
    //puts("yolo");
    pthread_t client_thread1,elev_thread,button_thread;

    master_socket = malloc(1);
    *master_socket = sockfd;

    //pthread_create(&client_thread1, NULL, tcp_send, (void*) master_socket);
    pthread_create(&client_thread1, NULL, tcp_recieve, (void*) master_socket);
    pthread_create(&elev_thread, NULL, elevator_thread, NULL);
    pthread_create(&button_thread, NULL, button_check, NULL);
    pthread_join(client_thread1, NULL);
    puts("TCP Recieve Thread Killed");
    //pthread_join(client_thread2, NULL);
    pthread_join(elev_thread,NULL);
    puts("Elevator Thread Killed");
    pthread_join(button_thread, NULL);
    puts("Button Check Thread Killed");
    close(sockfd); //Makes TCP connection to server,
    if(my_ID == 1){
      connection_init();
    }
    else if(my_ID == 2){
      sleep(5);
      connection_init();
    }
    else if(my_ID = 3){
      sleep(10);
      connection_init();
    }
}

void boss_init(){
  boss = 1;
  printf("Boss = %d\n",boss);
  pthread_t tcp_listen_thread,udp_listen_thread,udp_send_thread,communication_thread,elev_thread;
  pthread_create( &udp_listen_thread, NULL, udp_listen, NULL);
  pthread_create( &udp_send_thread, NULL, udp_send, NULL);
  pthread_create( &tcp_listen_thread, NULL, tcp_listen, NULL);
  system("gnome-terminal -x ./test 0");


  sleep(14);
  int sum;
  for(int i = 0;i<6;i++){
  	sum+=queue[i][0];
  }
  if(sum != 0){
  	reallocate_orders(clients);
  }

  pthread_join( udp_listen_thread , NULL);
  pthread_join( udp_send_thread , NULL);
  pthread_join( tcp_listen_thread , NULL);
}


void serialization(int message_type, Message message_send, char *transmit){
  message_send.type = message_type;
  //bzero(transmit,BUFSIZE);
  //message_send.ID = client_ID;
  transmit[0] = message_send.ID;
  transmit[1] = message_send.type;
  transmit[2] = message_send.elevator.floor_current;
  transmit[3] = message_send.elevator.reached_destination;
  transmit[4] = message_send.elevator.prev_direction;
  transmit[5] = message_send.elevator.direction;
  transmit[6] = message_send.elevator.new_floor_order;
  transmit[7] = message_send.elevator.queue[0];
  transmit[8] = message_send.elevator.queue[1];
  transmit[9] = message_send.elevator.queue[2];
  transmit[10] = message_send.elevator.queue[3];
  transmit[11] = message_send.button.floor;
  transmit[12] = message_send.button.type;
  //Mekke for-løkke, tilsvarende for deserialization
  transmit[13] = message_send.orders[0][0];
  transmit[14] = message_send.orders[1][0];
  transmit[15] = message_send.orders[2][0];
  transmit[16] = message_send.orders[3][0];
  transmit[17] = message_send.orders[4][0];
  transmit[18] = message_send.orders[5][0];
  transmit[19] = message_send.orders[0][1];
  transmit[20] = message_send.orders[1][1];
  transmit[21] = message_send.orders[2][1];
  transmit[22] = message_send.orders[3][1];
  transmit[23] = message_send.orders[4][1];
  transmit[24] = message_send.orders[5][1];

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
  message_receive.elevator.reached_destination=msg_recv[3];
  message_receive.elevator.prev_direction=msg_recv[4];
  message_receive.elevator.direction=msg_recv[5];
  message_receive.elevator.new_floor_order=msg_recv[6];
  message_receive.elevator.queue[0] = msg_recv[7];
  message_receive.elevator.queue[1] = msg_recv[8];
  message_receive.elevator.queue[2] = msg_recv[9];
  message_receive.elevator.queue[3] = msg_recv[10];
  message_receive.button.floor = msg_recv[11];
  message_receive.button.type = msg_recv [12];
  message_receive.orders[0][0] = msg_recv[13];
  message_receive.orders[1][0] = msg_recv[14];
  message_receive.orders[2][0] = msg_recv[15];
  message_receive.orders[3][0] = msg_recv[16];
  message_receive.orders[4][0] = msg_recv[17];
  message_receive.orders[5][0] = msg_recv[18];
  message_receive.orders[0][1] = msg_recv[19];
  message_receive.orders[1][1] = msg_recv[20];
  message_receive.orders[2][1] = msg_recv[21];
  message_receive.orders[3][1] = msg_recv[22];
  message_receive.orders[4][1] = msg_recv[23];
  message_receive.orders[5][1] = msg_recv[24];
  return message_receive;

}

void send_message(int message_type, Message message_send){
  message_send.type = message_type;
  char transmit[BUFSIZE];
  transmit[0] = message_send.ID;
  transmit[1] = message_send.type;
  transmit[2] = message_send.elevator.floor_current;
  transmit[3] = message_send.elevator.reached_destination;
  transmit[4] = message_send.elevator.prev_direction;
  transmit[5] = message_send.elevator.direction;
  transmit[6] = message_send.elevator.new_floor_order;
  transmit[7] = message_send.elevator.queue[0];
  transmit[8] = message_send.elevator.queue[1];
  transmit[9] = message_send.elevator.queue[2];
  transmit[10] = message_send.elevator.queue[3];
  transmit[11] = message_send.button.floor;
  transmit[12] = message_send.button.type;
  //Mekke for-løkke, tilsvarende for deserialization
  transmit[13] = message_send.orders[0][0];
  transmit[14] = message_send.orders[1][0];
  transmit[15] = message_send.orders[2][0];
  transmit[16] = message_send.orders[3][0];
  transmit[17] = message_send.orders[4][0];
  transmit[18] = message_send.orders[5][0];
  transmit[19] = message_send.orders[0][1];
  transmit[20] = message_send.orders[1][1];
  transmit[21] = message_send.orders[2][1];
  transmit[22] = message_send.orders[3][1];
  transmit[23] = message_send.orders[4][1];
  transmit[24] = message_send.orders[5][1];

  for(int i = 0;i<BUFSIZE-1;i++){
    if(transmit[i] == 0)
      transmit[i] = '0';
   }
  printf("\n");
  
  pthread_t message_send_thread;
  pthread_create(&message_send_thread, NULL , tcp_send, (void*)transmit); 
}
