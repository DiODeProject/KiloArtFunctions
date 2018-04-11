#include <kilolib.h>

#define DEBUG
#include <debug.h>

// Constants for orbit control.
#define TOO_CLOSE_DISTANCE 40
#define DESIRED_DISTANCE 50

// Constants for motion handling function.
#define STOP 0
#define FORWARD 1
#define LEFT 2
#define RIGHT 3

//long one_distance = 0;
//int one_updated = 0;
//long another_distance = 0;
//int another_updated = 0;
//int received_id = 0;

//long standard_area = 1171875;
//long current_area = 0;
//long fixed_distance = 50;
int current_motion = STOP;
int distance;
int mini_distance;
int new_message = 0;
uint32_t last_update;

// Function to handle motion.
void set_motion(int new_motion)
{
    // Only take an action if the motion is being changed.
    if (current_motion != new_motion)
    {
        current_motion = new_motion;
        
        if (current_motion == STOP)
        {
            set_motors(0, 0);
        }
        else if (current_motion == FORWARD)
        {
            //spinup_motors();
            set_motors(kilo_straight_left, kilo_straight_right);
        }
        else if (current_motion == LEFT)
        {
            //spinup_motors();
            set_motors(kilo_straight_left, kilo_straight_right);
            delay(500);
            set_motors(kilo_turn_left, 0);
            delay(1000);
            spinup_motors();
            set_motors(kilo_straight_left, kilo_straight_right);
            delay(1000);
            set_motors(0, kilo_turn_right);
            delay(500);
            spinup_motors();
            set_motors(kilo_straight_left, kilo_straight_right);
            delay(500);
        }
        else if (current_motion == RIGHT)
        {
            //spinup_motors();
            set_motors(kilo_straight_left, kilo_straight_right);
            delay(500);
            set_motors(0, kilo_turn_right);
            delay(1000);
            spinup_motors();
            set_motors(kilo_straight_left, kilo_straight_right);
            delay(1000);
            set_motors(kilo_turn_left, 0);
            delay(500);
            spinup_motors();
            set_motors(kilo_straight_left, kilo_straight_right);
            delay(500);
        }
    }
}


void setup()
{
    last_update = kilo_ticks;
    mini_distance = 500;
}

void loop()
{
    
    if (mini_distance > distance){
        mini_distance = distance;
    }
    // Update the motion if a message is received.
    if (kilo_ticks > (last_update+32)){

        last_update = kilo_ticks;
	    if (new_message == 1)
	    {
		new_message = 0;
		
		
		    if (mini_distance > DESIRED_DISTANCE)
		    {
		        set_color(RGB(1, 0, 0));
		        set_motion(LEFT);
		    }
		    /*else if(mini_distance < TOO_CLOSE_DISTANCE){
                        
		        set_color(RGB(0, 0, 1));
		        set_motion(RIGHT);
                    }*/
		    else
		    {
		        set_color(RGB(0, 0, 1));
		        set_motion(RIGHT);
                    }
	    }
       
        mini_distance = 500;
    }
}

void message_rx(message_t *m, distance_measurement_t *d)
{
    new_message = 1;
    distance = estimate_distance(d);
    //received_id = m->data[0];
}

int main()
{
    kilo_init();
    debug_init();
    kilo_message_rx = message_rx;
    kilo_start(setup, loop);
    
    return 0;
}
