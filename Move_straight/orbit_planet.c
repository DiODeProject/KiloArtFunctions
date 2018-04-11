#include <kilolib.h>
//#include <stdlib.h>

#define ONEID 0
#define ANOTHERID 1
// Constants for orbit control.
#define TOO_CLOSE_DISTANCE 5
#define DESIRED_DISTANCE 60

// Constants for motion handling function.
#define STOP 0
#define FORWARD 1
#define LEFT 2
#define RIGHT 3

int both_spined = 0;
int current_motion = STOP;
int distance1;
int distance2;
uint32_t last_dist1_update;
uint32_t last_dist2_update;
int diff_dist;
int new_message = 0;
int received_id = 255;

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
            if(both_spined==0)
                spinup_motors();
            set_motors(kilo_turn_left, 0);
            both_spined=0;
        }
        else if (current_motion == RIGHT)
        {
            if(both_spined==0)
                spinup_motors();
            set_motors(0, kilo_turn_right);
            both_spined=0;
        }
    }
}

void straight_adjust(int left, int right){
    if (both_spined==0){
        spinup_motors();
        both_spined = 1;
    }
    set_motors(kilo_straight_left+left,kilo_straight_right+right);
}

void setup()
{
}

void loop()
{
    // Update the motion whenever a message is received.
    if (new_message == 1)
    {
        new_message = 0;

        if(distance1==0 || distance2==0){
                set_color(RGB(0, 0, 0));
                set_motion(STOP);
        }
        else{
            diff_dist = distance1-distance2;
            
            if(diff_dist <= 10){
                if(diff_dist >= 0){
                    set_color(RGB(1, 0, 0));
                    straight_adjust(diff_dist,0);
                }
                else{
                    set_color(RGB(0, 0, 1));
                    straight_adjust(0,-diff_dist);

                }

            }
            else{
                if (distance1 < distance2)
                {
                    set_color(RGB(1, 1, 0));
                    set_motion(RIGHT);
                }
                else
                {
                    set_color(RGB(0, 1, 1));
                    set_motion(LEFT);
                }

            }
        }
        // If too close, move forward to get back into orbit.
        /*if (diff_dist < TOO_CLOSE_DISTANCE)
        {
            set_color(RGB(0, 1, 0));
            set_motion(FORWARD);
        }
        // If not too close, turn left or right depending on distance,
        // to maintain orbit.
        else
        {
            if (distance1 < distance2)
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

    if(kilo_ticks > last_dist1_update + 16){
        distance1 = 0;
    }
    if(kilo_ticks > last_dist2_update + 16){
        distance2 = 0;
    }
}

void message_rx(message_t *m, distance_measurement_t *d)
{
    if (new_message == 0){
        new_message = 1;
        received_id = m->data[0];
        if(received_id==ONEID){
            distance1 = estimate_distance(d);
            last_dist1_update = kilo_ticks;
        }
        else if(received_id==ANOTHERID){
            distance2 = estimate_distance(d);
            last_dist2_update = kilo_ticks;
        }
    }
}

int main()
{
    kilo_init();
    kilo_message_rx = message_rx;
    kilo_start(setup, loop);
    
    return 0;
}
