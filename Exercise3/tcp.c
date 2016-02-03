#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 20017
#define SERVER "129.241.187.255"

// gcc -std=gnu99 -Wall -g -o udp udp_send_recv.c -lpthread

// Note the return type: void*
void* tcp_Client(){
    char *message;
    struct sockaddr_in sa;
    int res;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  
    if (-1 == SocketFD) {
      perror("cannot create socket");
      exit(EXIT_FAILURE);
    }
  
    memset(&sa, 0, sizeof sa);
  
    sa.sin_family = AF_INET;
    sa.sin_port = htons(PORT);
    res = inet_pton(AF_INET, "129.241.187.145", &sa.sin_addr);

    if (-1 == connect(SocketFD, (struct sockaddr *)&sa, sizeof sa)) {
      perror("connect failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }

    char server_reply[6000];
    if(recv(SocketFD, server_reply , 6000 , 0) < 0){
        printf("recv failed\n");
    }
    printf("%s\n", server_reply);
  
    /* perform read write operations ... */
    message = "Halla, dette er TCP";
    
      printf("start while(1)\n");
    if(write(SocketFD,message,strlen(message))<0)
    //if(send(SocketFD,message,strlen(message)+1,0)<0)
       {
        printf("Send failed\n");
        exit(1);
       } 
    
    if(recv(SocketFD, server_reply , 6000 , 0) < 0){
        printf("recv failed\n");
    }
    printf("%s\n", server_reply);
  
    (void)shutdown(SocketFD, SHUT_RDWR);
  
    close(SocketFD);
    return EXIT_SUCCESS;
}

void* tcp_Serv(){
    char server_reply[6000];
    struct sockaddr_in sa;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  
    if (-1 == SocketFD) {
      perror("cannot create socket");
      exit(EXIT_FAILURE);
    }
  
    memset(&sa, 0, sizeof sa);
  
    sa.sin_family = AF_INET;
    sa.sin_port = htons(PORT);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
  
    if (-1 == bind(SocketFD,(struct sockaddr *)&sa, sizeof sa)) {
      perror("bind failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
  
    if (-1 == listen(SocketFD, 10)) {
      perror("listen failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
  
    for (;;) {
      int ConnectFD = accept(SocketFD, NULL, NULL);
  
      if (0 > ConnectFD) {
        perror("accept failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
      }
  
      //perform read write operations ... 
      //sock.read(ConnectFD, buff, size)
  
      if(recv(SocketFD, server_reply , 6000 , 0) < 0)
      {
        printf("recv failed\n");
      }
      else printf("Reply received\n");
      printf(server_reply);
	  
      if (-1 == shutdown(ConnectFD, SHUT_RDWR)) {
        perror("shutdown failed");
        close(ConnectFD);
        close(SocketFD);
        exit(EXIT_FAILURE);
      }
      close(ConnectFD);
    }

    close(SocketFD);
    return EXIT_SUCCESS;  
}


int main(){
	
    pthread_t thread1;
    pthread_t thread2;
   // pthread_create(&thread1, NULL, tcp_Serv, NULL);
    pthread_create(&thread2, NULL, tcp_Client, NULL);
    // Arguments to a thread would be passed here ---------^
    
    if(!pthread_join(thread1, NULL))
        printf("Thread1_join sucsess\n");
    if(!pthread_join(thread2, NULL))
        printf("Thread2_join sucsess\n");
    printf("Hello from main!\n");
    
    return 0;
    
}
