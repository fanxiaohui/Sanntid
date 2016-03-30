

//elevator module interface:
#include "elev.h"

#define ELEVATOR 1
#define BACKUP 2
#define STATUS 3
#define FLOOR_INDICATOR 4
#define BUTTON_PRESSED 5
#define BUTTON_INDICATOR 6



struct Elevator {
   int floor_current;
   int destination;
   int reached_destination=0;
   int direction=DIRN_STOP:
   int new_floor_order=0;
};
struct Message_received {
	int ID;
	int type;
	int new_floor_order;
	int floor_indicator[4][3];
};
