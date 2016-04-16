#ifndef MAIN_H
#define MAIN_H
#include "lift.h"








int cost_function(elev_button_type_t type, int floor);
int add_to_queue(elev_button_type_t type, int  floor);
void reallocate_orders(int number_of_clients);
#endif