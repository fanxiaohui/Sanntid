#ifndef QUEUE_FUNCTIONS_H
#define QUEUE_FUNCTIONS_H

int cost_function(elev_button_type_t type, int floor);
int add_to_queue(elev_button_type_t type, int  floor);
void reallocate_orders(int number_of_clients);
void *order_counter(void *x);

#endif
