#include <kilolib.h>

int new_message = 0;
int distance = 0;
int order = 0;


// Constants for motion handling function.
#define STOP 0
#define FORWARD 1
#define LEFT 2
#define RIGHT 3

int current_motion = STOP;

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


void setup()
{
}

void loop()
{
    if (new_message == 1)
    {
        new_message = 0;
        

        if (order == 0)
        {
            set_color(RGB(1, 0, 0));
            set_motion(LEFT);

        }
        else if(order == 1)
        {
            set_color(RGB(0, 0, 1));
            set_motion(RIGHT);
        }
        else if(order == 2)
        {
            set_color(RGB(0, 1, 0));
            set_motion(FORWARD);
        }
        // Wherever you are: set LED off after 100 ms to achieve blink effect.
        delay(100);
        set_color(RGB(0, 0, 0));
    }
}

void message_rx(message_t *message, distance_measurement_t *distance_measurement)
{
    // Set flag on message reception.
    new_message = 1;
    
    order = message->data[0];
}

int main()
{
    kilo_init();
    kilo_message_rx = message_rx;
    kilo_start(setup, loop);
    
    return 0;
}
