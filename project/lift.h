

//elevator module interface:
#ifndef LIFT_H
#define LIFT_H

#include <stdio.h>
#include <string.h> 
#include <stdlib.h> 
#include "elev.h"


int client_ID;


void button_check();
void elevator_thread();

#endif