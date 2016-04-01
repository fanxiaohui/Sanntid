
extern int *new_sock;
#define BUFSIZE 1024

void *tcp_connection_handler(void *socket_desc);
void tcp_listen();
void connection_init(void);
void* udp_listen();
void* udp_send();
void *tcp_send(void *socket_desc);
void tcp_recieve(void *socket_desc);
void client_init();
void* communication();
void* elevator_thread();
