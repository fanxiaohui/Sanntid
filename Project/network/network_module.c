

#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#ifdef __WIN32__
# include <winsock2.h>
#else
# include <sys/socket.h>
#endif
#include<unistd.h>    //write
#include<arpa/inet.h> //inet_addr
#include<pthread.h> //for threading , link with lpthread


#define PORT 20017
int boss = -1;
int ID;
int new_conn = 0;
/*
 * This will handle connection for each client
 * */

void *tcp_connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000];
     
    //Send some messages to the client
    message = "Greetings! I am your connection handler\n";
    write(sock , message , strlen(message));
     
    message = "Now type something and i shall repeat what you type \n";
    write(sock , message , strlen(message));
     
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //Send the message back to client
        write(sock , client_message , strlen(client_message));
    }
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
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
         
        //Reply to the client
        message = "Hello Client , I have received your connection. And now I will assign a handler for you\n";
        write(new_socket , message , strlen(message));
         
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = new_socket;
         
        if( pthread_create( &sniffer_thread , NULL ,  tcp_connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
    }
     
    if (new_socket<0)
    {
        perror("accept failed");
        return 1;
    }


}

//Checks if there exists a boss through UDP, if not set self to boss
void connection_init(void){
    int sock,sock_r;
    struct sockaddr_in sa;
    int bytes_sent;
    int s = sizeof(sa);
    char buffer[200];
    char buffer2[200];
    
    strcpy(buffer, "Noen gangstas her?");
    strcpy(buffer2, "Halla");

    /* create an Internet, datagram, socket using UDP */
    //sock= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

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

    //if(inet_aton(SERVER, &sa.sin_addr) ==0)
    //{
    //exit(1);
    //}
    for(int i = 0;i<10;i++){
        bytes_sent = sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr*)&sa, s);
        if (bytes_sent < 0) {
                //printf("Error sending packet: %s\n", strerror(errno));
                exit(EXIT_FAILURE);

    }
        printf("Packet sent \n");
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


    struct timeval tv;
    tv.tv_sec = 5;  /* 5 Secs Timeout */
    tv.tv_usec = 0;

    if (-1 == bind(sock_r, (struct sockaddr *)&sa, sizeof sa)) {
        perror("error bind failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

     setsockopt(sock_r, SOL_SOCKET, SO_RCVTIMEO,(char *)&tv,sizeof(tv)); 

      recsize = recvfrom(sock_r, (void*)buffer, sizeof buffer, 0, (struct sockaddr*)&sa, &fromlen);
      /*if (recsize < 0) {
        //fprintf(stderr, "%s\n", strerror(errno));
        exit(EXIT_FAILURE);
       }*/
       if(!strcmp(buffer,buffer2))
       {
          boss = 0; 
       }
      printf("recsize: %zu\n ", recsize);
      //sleep(0.1);
      printf("datagram: %.*s\n", (int)recsize, buffer);
  
    if(boss == -1){
        boss = 1;
    } 
    close(sock_r); 
}
 
void* udp_listen(){
  int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  struct sockaddr_in sa; 
  char buffer[1024];
  char buffer2[200];
  strcpy(buffer2, "Noen gangstas her?");
  ssize_t recsize;
  socklen_t fromlen;

  memset(&sa, 0, sizeof sa);
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = htonl(INADDR_ANY);
  sa.sin_port = htons(PORT);
  fromlen = sizeof(sa);

  if (-1 == bind(sock, (struct sockaddr *)&sa, sizeof sa)) {
    perror("error bind failed");
    close(sock);
    exit(EXIT_FAILURE);
  }

  for (;;) {
    recsize = recvfrom(sock, (void*)buffer, sizeof buffer, 0, (struct sockaddr*)&sa, &fromlen);
    if (recsize < 0) {
      fprintf(stderr, "%s\n", strerror(errno));
      exit(EXIT_FAILURE);
     }
    if(strcmp(buffer2,buffer){
        //New connection
        prinft("New connection discovered")
        new_conn = 1;
    }
    printf("recsize: %zu\n ", recsize);
        //sleep(0.1);
    printf("datagram: %.*s\n", (int)recsize, buffer);
    }
    return NULL;
}

void* udp_send(){
  int sock;
  struct sockaddr_in sa;
  int bytes_sent;
  int s = sizeof(sa);
  char buffer[200];
 
  strcpy(buffer, "Halla");
 
  /* create an Internet, datagram, socket using UDP */
  //sock= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  
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

   if(inet_aton(SERVER, &sa.sin_addr) ==0)
   {
    exit(1);
   }
  for(;;){
    if(new_conn == 1){
        for(int i = 0;i<10;i++){
            bytes_sent = sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr*)&sa, s);
            if (bytes_sent < 0) {
                    printf("Error sending packet: %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }
            printf("Packet sent \n");
        }
        new_conn = 0;
    }
  }
  close(sock); /* close the socket */

    return NULL;
}

int main(int argc , char *argv[])
{   
    connection_init();
    printf("Boss = %d\n", boss);
    if(boss == 1){

    }
    else{

    }
    //pthread_t tcp_listen_thread,udp_listen_thread, connection_init_thread;
    //pthread_create( &connection_init_thread, NULL, connection_init, NULL);
    //pthread_create( &udp_listen_thread, NULL, udp_listen, NULL);

    //pthread_create( &tcp_listen_thread, NULL, tcp_listen, NULL);
     
    return 0;
}