


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
#include "lift.h"
#include "elev.h"


#define PORT 20017

//#define SERVER "129.241.187.255"

char* BROADCASTIP;
int boss = -1;
int new_conn = 0;
char* serv_ip;
int optval = 1;

int clients = 0;
int flag = 0;
int connection = 1;
*master_socket;
int broadcast = 0;
int *sockets[2];
client_sockets[3];
int recv_client = 0;

queue[6][2]={0};


pthread_mutex_t mutex;
// gcc -std=gnu11 -Wall -g -o udp udp_send_recv.c -lpthread
/*
 * This will handle connection for each client
 * */

void *tcp_connection_handler(void *socket_desc) //Connection handler for TCP connections
{
    //Get the socket descriptor
    int client_id = clients-1;
    int sock = *(int*)socket_desc;
    int read_size;
    char client_message[BUFSIZE];

    //Send some messages to the client
    //message = "Greetings! I am your connection handler\n";
    //write(sock , message , strlen(message));
     
    bzero(client_message,BUFSIZE); 
    //Receive a message from client
    while( (read_size = recv(sock , client_message , BUFSIZE , 0)) > 0 )
    {
    
    broadcast = 0;
    Message message_send,message_receive;

    message_receive = deserialization(client_message);
   

    //deserialization( client_message);
    switch(message_receive.type){
      case BUTTON_CHECK: //ok
        
        printf("Reciever Button floor & type: ");
        printf("%d ",message_receive.button.floor);
        printf("%d \n",message_receive.button.type);
        switch(message_receive.button.type){
          case BUTTON_INSIDE:
            puts("Button inside");
            //Set order in queue?
            break;
          default:
            puts("Button up/down");
            if(add_to_queue(message_receive.button.type, message_receive.button.floor)){
              puts("Added to queue");
              //send lamp
              char lamp_message[BUFSIZE], server_message[BUFSIZE];
              for(int i = 0;i<6;i++){
                message_send.lamps_outside[i] = queue[i][1];
              }
              /*serialization(BUTTON_LAMP, message_send, lamp_message);
              for(int i = 0;i<clients;i++){
                write(client_sockets[i], lamp_message, strlen(lamp_message));
              }
              sleep(1);
              */
              message_send.ID = cost_function(message_receive.button.type, message_receive.button.floor);
              message_send.elevator.new_floor_order=message_receive.button.floor;
              if(message_receive.button.type==BUTTON_CALL_UP){
                queue[message_receive.button.floor][2] = message_send.ID;
                queue[message_receive.button.floor][1] = 1;

              }
              else{
                queue[message_receive.button.floor+2][2] = message_send.ID;
                queue[message_receive.button.floor][1] = 1;
              }

              serialization(ELEV_ORDER, message_send, server_message);
              for(int i = 0;i<clients;i++){
                write(client_sockets[i], server_message, strlen(server_message));  
              }
            } 
          }
              
        break;
      case ELEV_FLOOR_UPDATE: //ok
        puts("ELEV_FLOOR_UPDATE");
        puts("Old queue");

        //Elevator elev_client[3];
        elev_client[message_receive.ID].floor_current=message_receive.elevator.floor_current;
        elev_client[message_receive.ID].direction=message_receive.elevator.direction;
        for(int i=0;i<N_FLOORS;i++){
          elev_client[message_receive.ID].queue[i]=message_receive.elevator.queue[i];
        }

        for(int i = 0;i<6;i++){
          printf("%d",queue[i][1]);
         // elev_client[client_id].elevator.queue[message_receive.elevator.floor_current]=0;
        }
        /*if(message_receive.elevator.direction==DIRN_STOP){ //TURN OFF LAMP IF ELEVATOR IS AVAIABLE, PRIORITY 1.UP 2.DOWN
            if(queue[message_receive.floor_current][1]==1){
              queue[message_receive.elevator.floor_current]=0;
              queue[message_receive.elevator.floor_current][2]=-1;
              puts("DIRN STOP, GOES UP");
            }
            else if(queue[message_receive.floor_current+2][1]){
              queue[message_receive.elevator.floor_current+2]=0;
              queue[message_receive.elevator.floor_current+2][2]=-1;
              puts("DIRN STOP, GOES DOWN");
            }

            broadcast = 1;
        }
        */
        if(message_receive.elevator.direction == DIRN_UP){
              queue[message_receive.elevator.floor_current][1]=0;
              queue[message_receive.elevator.floor_current][2]=-1;
              puts("DIRN UP");
              broadcast = 1;
        }
        else if(message_receive.elevator.direction == DIRN_DOWN){
            queue[message_receive.elevator.floor_current+2][1]=0;
            queue[message_receive.elevator.floor_current+2][2]=-1;
            puts("DIRN DOWN");
            broadcast = 1;
        }

        if(broadcast){
          char lamp_message[BUFSIZE];
          for(int i = 0;i<6;i++){
            message_send.lamps_outside[i] = queue[i][1];
          }
          serialization(BUTTON_LAMP, message_send, lamp_message);
          for(int i = 0;i<clients;i++){
            write(client_sockets[i], lamp_message, strlen(lamp_message));
          }
        }

        puts("\nNew queue");
        for(int i = 0;i<6;i++){
          printf("%d",queue[i][1]);
        }
        puts("");
        break;

      }
      
        /*if(atoi(client_message)==1){
        	for(int i = 0;i<clients;i++){
        		int tempsock = *(int*)sockets[i];
        		write(tempsock, client_message, strlen(client_message));
        	}
        }
        */
        /*
        if(atoi(client_message) >= 0 && atoi(client_message) < 4){
        	for(int i = 0;i<clients;i++){
        		write(client_sockets[i], client_message, strlen(client_message));
       		}
       		elevator.new_floor_order = atoi(client_message);
       	}
       	else 
        	write(sock, client_message, strlen(client_message));
        //write(client2, client_message, strlen(client_message));
        //Send the message back to client
        //write(sock , client_message , strlen(client_message));
        */
        //write(sock , client_message , strlen(client_message));
      	
      	//puts("Client message:");
   
    	//printf("Client message: %d\n", atoi(client_message));   	//puts(client_message);

	     
        //printf("Client message: %d", elevator.new_floor_order);

       /* if(!strcmp(client_message,elev_go));
        {
            //Kjør heis
            flag = 1;
            elevator.new_floor_order=3;
            printf("Halla");
        }
        */
        bzero(client_message,BUFSIZE);
    }
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        clients--;
        fflush(stdout);
        printf("#Clients: %d\n",clients); 
        if(client_id == 0){
          system("gnome-terminal -x ./test 0");
        }
    }
    else if(read_size == -1)
    {
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
    char *message;
     
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
        message = "Hello Client \n";
        write(new_socket , message , strlen(message));
         
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = new_socket;
        client_sockets[clients-1] = new_socket;
        //*sockets[clients-1] = new_socket; 
        if( pthread_create( &sniffer_thread , NULL ,  tcp_connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        puts("Handler assigned");
        //pthread_join( sniffer_thread , NULL);
        
    }
     
    if (new_socket<0)
    {
        perror("accept failed");
        return 1;

    } //Listens on TCP
}

void connection_init(void){
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
                //printf("Error sending packet: %s\n", strerror(errno));
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
    tv.tv_sec = 5;  /* 5 Secs Timeout */
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

       }
       else
       {
          printf("Master not discovered\n");
          printf("Recieved: %s\n", buffer3);
          /*if(connection == 0)
          {
          	boss = 1;
          	printf("Boss: %d",boss);
          	pthread_t tcp_listen_thread,udp_listen_thread,udp_send_thread,communication_thread,elev_thread;
		    pthread_create( &udp_listen_thread, NULL, udp_listen, NULL);
		    pthread_create( &udp_send_thread, NULL, udp_send, NULL);
		    pthread_create( &tcp_listen_thread, NULL, tcp_listen, NULL);
		    pthread_create( &elev_thread, NULL, elevator_thread, NULL);
   	        // pthread_create( &communication_thread, NULL, communication, NULL);
            pthread_join( udp_listen_thread , NULL);
		    pthread_join( udp_send_thread , NULL);
		    pthread_join( tcp_listen_thread , NULL);
	        pthread_join( elev_thread, NULL);
		    //pthread_join( communication_thread, NULL);
          
          }
          */
       }
      //printf("recsize: %zu\n ", recsize);
      //sleep(0.1);
      //printf("datagram: %.*s\n", (int)recsize, buffer);
  
    if(boss == -1){
        boss = 1;
    } 
    close(sock_r);  //Checks if there exists a boss through UDP, if not set self to boss
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
	for(int i = 0;i<13;i++){
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
    	printf("Server message:\n");
    	for(int z = 0;z<19;z++){
      if(buf[z] == '0')
        buf[z] = 0;
			 printf("%d ",buf[z]);
		  }
		printf("\n");
		//char msg[BUFSIZE];
		//strcpy(msg,buf);
		pthread_t message_handler_thread;
	    pthread_create(&message_handler_thread, NULL , message_handler, (void*)msg);	
	    }
    bzero(buf, BUFSIZE);
  }
  free(socket_desc);
     
}

void client_init(){
    int sockfd, portno, n;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname = serv_ip;
    char buf[BUFSIZE];
    char server_reply[6000];

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
    //pthread_join(client_thread2, NULL);
    pthread_join(elev_thread,NULL);
    pthread_join(button_check, NULL);
    close(sockfd); //Makes TCP connection to server,
    connection_init();
}

int main(int argc , char *argv[])
{   
  
    pthread_mutex_init(&mutex,NULL);

    connection_init();
    printf("Boss = %d\n", boss);
    if(boss == 1){

      for(int i=0;i<6;i++){
        queue[i][2] = -1;
      }

      pthread_t tcp_listen_thread,udp_listen_thread,udp_send_thread,communication_thread,elev_thread;
      pthread_create( &udp_listen_thread, NULL, udp_listen, NULL);
      pthread_create( &udp_send_thread, NULL, udp_send, NULL);
      pthread_create( &tcp_listen_thread, NULL, tcp_listen, NULL);
      //pthread_create( &elev_thread, NULL, elevator_thread, NULL);
      //pthread_create( &communication_thread, NULL, communication, NULL);
      //sleep(1);
      system("gnome-terminal -x ./test 0");
      pthread_join( udp_listen_thread , NULL);
      pthread_join( udp_send_thread , NULL);
      pthread_join( tcp_listen_thread , NULL);
      //pthread_join( elev_thread, NULL);
      //pthread_join( communication_thread, NULL);

    }
    else{

      client_init();
    }
    //pthread_t tcp_listen_thread,udp_listen_thread, connection_init_thread;
    //pthread_create( &connection_init_thread, NULL, connection_init, NULL);
    //pthread_create( &udp_listen_thread, NULL, udp_listen, NULL);
    //pthread_create( &tcp_listen_thread, NULL, tcp_listen, NULL);
     
    return 0;
}




