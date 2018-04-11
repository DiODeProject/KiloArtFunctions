#include <kilolib.h>

// Constants for orbit control.
#define TOO_CLOSE_DISTANCE 40
#define DESIRED_DISTANCE 100

// Constants for motion handling function.
#define STOP 0
#define FORWARD 1
#define LEFT 2
#define RIGHT 3

int current_motion = STOP;
int distance = 500;
int new_message = 0;
int both_spined = 0;
uint32_t last_motion_update = 0;

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
            spinup_motors();
            set_motors(kilo_straight_left, kilo_straight_right);
            both_spined = 1;
        }
        else if (current_motion == LEFT)
        {
            if(both_spined == 0)
                spinup_motors();
            set_motors(kilo_turn_left, 0);
            both_spined = 0;
        }
        else if (current_motion == RIGHT)
        {
            if(both_spined == 0)
                spinup_motors();
            set_motors(0, kilo_turn_right);
            both_spined = 0;
        }
    }
}

void setup()
{
                set_color(RGB(0, 1, 0));
                set_motion(FORWARD);
}

void loop()
{
    // Update the motion whenever a message is received.
    if (kilo_ticks > last_motion_update + 16){
        last_motion_update = kilo_ticks;

	    if (new_message == 1 || distance < DESIRED_DISTANCE)
	    {
		new_message = 0;
		
		// If too close, move forward to get back into orbit.
		if (distance < DESIRED_DISTANCE)
		{
		        set_color(RGB(1, 0, 1));
		        set_motion(STOP);
		}
		// If not too close, turn left or right depending on distance,
		// to maintain orbit.
		else
		{
		        set_color(RGB(0, 1, 0));
		        set_motion(FORWARD);
		}
		    /*if (distance < DESIRED_DISTANCE)
		    {
		        set_color(RGB(1, 0, 0));
		        set_motion(LEFT);
		    }
		    else
		    {
		        set_color(RGB(0, 0, 1));
		        set_motion(RIGHT);
		    }
		}*/
	    }
           else{
            int random_number = rand_hard();
            int random_direction = (random_number % 4);

            if ((random_direction == 0) || (random_direction == 1))
            {
                set_color(RGB(0, 1, 0));
                set_motion(FORWARD);
            }
            else if (random_direction == 2)
            {
                set_color(RGB(1, 0, 0));
                set_motion(LEFT);
            }
            else if (random_direction == 3)
            {
                set_color(RGB(0, 0, 1));
                set_motion(RIGHT);
            }


           }
    }
    /*else {
                set_color(RGB(0, 1, 0));
                set_motion(FORWARD);
    }*/
}

void message_rx(message_t *m, distance_measurement_t *d)
{
    new_message = 1;
    distance = estimate_distance(d);
}

int main()
{
    kilo_init();
    kilo_message_rx = message_rx;
    kilo_start(setup, loop);
    
    return 0;
}
