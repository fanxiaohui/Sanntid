#include "lift.h"
#include "elev.h"


int lift_set_destination(int destination){
	int position_current=elev_get_floor_sensor_signal();
	int direction=elev_get_direction();
	//Allready in motion
	if(position_current==-1){

		//Going upwards
		if(direction == DIRN_UP && destination<destination_current){
			destination_third=destination_next;
			destination_next = destination_current;
			destination_current=destination;
			}
		else if(direction == DIRN_UP && destination < destination_third){

		}
		
	}	
	else
		if(position_current - destination < 0)
			elev_set_motor_direction(DIRN_UP);
		else if(position_current - destination > 0)
			elev_set_motor_direction(DIRN_DOWN);

	while(elev_get_floor_sensor_signal() != destination-1){
		if(elev_get_floor_sensor_signal()){
			elev_set_floor_indicator(elev_get_floor_sensor_signal(),1);
			elev_set_floor_indicator(elev_get_floor_sensor_signal()-1,0);
		}
		if(destination == ne)

//make some kind of pull function that returns the next destination to go to
		//elev_set_floor_indicator(elev_get_floor_sensor_signal());
	}
	elev_set_motor_direction(DIRN_STOP);
	elev_set_door_open_lamp(1);
}

