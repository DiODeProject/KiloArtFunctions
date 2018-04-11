#include <kilolib.h>

#define DEBUG
#include <debug.h>

// Constants for orbit control.
#define TOO_CLOSE_DISTANCE 40
#define DESIRED_DISTANCE 60

// Constants for motion handling function.
#define STOP 0
#define FORWARD 1
#define LEFT 2
#define RIGHT 3

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
            spinup_motors();
            set_motors(kilo_straight_left, kilo_straight_right);
        }
        else if (current_motion == LEFT)
        {
            spinup_motors();
            set_motors(kilo_turn_left, 0);
        }
        else if (current_motion == RIGHT)
        {
            spinup_motors();
            set_motors(0, kilo_turn_right);
        }
    }
}


// Function to sample light.
void sample_light()
{
    // The ambient light sensor gives noisy readings. To mitigate this,
    // we take the average of 300 samples in quick succession.
    
    int number_of_samples = 0;


    printf("begin");
    while (number_of_samples < 10)
    {
        printf("%d\n", distance);
        number_of_samples = number_of_samples + 1;
    }

}

void setup()
{
    last_update = kilo_ticks;
    mini_distance = 500;
}

void loop()
{
    //printf("d1: %d\n", distance);
    //sample_light();
    if (mini_distance > distance){
        mini_distance = distance;
    }
    // Update the motion if a message is received.
    if (kilo_ticks > (last_update+16)){

        last_update = kilo_ticks;
	printf("time to change: %d\n",mini_distance);
	    if (new_message == 1)
	    {
		new_message = 0;
		
		// If too close, move forward to get back into orbit.
		if (distance < TOO_CLOSE_DISTANCE)
		{
		    set_color(RGB(0, 1, 0));
		    set_motion(FORWARD);
		}
		// If not too close, turn left or right depending on distance,
		// to maintain orbit.
		else
		{
		    if (distance < DESIRED_DISTANCE)
		    {
		        set_color(RGB(1, 0, 0));
		        set_motion(LEFT);
		    }
		    else
		    {
		        set_color(RGB(0, 0, 1));
		        set_motion(RIGHT);
		    }
		}
	    }
        mini_distance = 500;
       
    }
}

void message_rx(message_t *m, distance_measurement_t *d)
{
    new_message = 1;
    distance = estimate_distance(d);
}

int main()
{
    kilo_init();
    debug_init();
    kilo_message_rx = message_rx;
    kilo_start(setup, loop);
    
    return 0;
}
