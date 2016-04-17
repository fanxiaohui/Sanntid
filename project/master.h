#ifndef MASTER_H
#define MASTER_H

void *master_connection_handler(void *socket_desc);		//Connection handler for TCP connections
void check_clients();									//Sends message every 10 second to clients
int cost_function(elev_button_type_t type, int floor);
int add_to_queue(elev_button_type_t type, int  floor);
void reallocate_orders(int number_of_clients);
void *order_counter(void *x);

#endif
