#include <kilolib.h>

// Since 1 byte (8 bits) of the message is used to communicate gradients, the
// maximum possible gradient is 2^8 - 1 = 255.
#define GRADIENT_MAX 255
// This is an arbitrary number, but it's important that exactly one robot in the
// group is calibrated to have this ID.
#define SEED_ID 42

#define STOP 0
#define FORWARD 1
#define LEFT 2
#define RIGHT 3

int current_motion = STOP;
int both_spined = 0;

int own_gradient = GRADIENT_MAX;
int received_gradient = 0;
int new_message = 0;
uint32_t last_gradient_anchored;
message_t message;

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
    //If the robot is the seed, its gradient should be 0: overwrite the 
    // previously set value of GRADIENT_MAX.
    if (kilo_uid == SEED_ID)
    {
        own_gradient = 0;
    }
    
    // Set the transmission message.
    message.type = NORMAL;
    message.data[0] = own_gradient;
    message.crc = message_crc(&message);
}

void loop() {
    // Only pay attention to messages if this robot is not the seed.
    if (kilo_uid != SEED_ID)
    {
        if (new_message == 1)
        {
            // If a neighbor's gradient is 1 or more less than this robot's
            // gradient, the latter should not increase.
            // Set last_gradient_anchored to kilo_ticks to inhibit activation of
            // the increment statement.
            if (own_gradient >= received_gradient + 1)
            {
            	last_gradient_anchored = kilo_ticks;
            }			
	 
            // If a neighbor's gradient is 2 or more less than this robot's 
            // gradient, reduce the latter to the neighbor's gradient + 1.
            if (own_gradient > received_gradient + 1)
            {
                own_gradient = received_gradient + 1;
                
                // Update the transmission message whenever the gradient changes.
                message.type = NORMAL;
                message.data[0] = own_gradient;
                message.crc = message_crc(&message);
            }
            
            new_message = 0;
        }
		
        // If no neighbor with a gradient of 1 or more less than this robot's
        // gradient is detected within 2 seconds, increment the latter by 1.
        if ((kilo_ticks > (last_gradient_anchored + 64)) && (own_gradient < GRADIENT_MAX))
        {
            own_gradient = own_gradient + 1;
        }
    }
	
    // Set the LED color based on the gradient.
    if (own_gradient == 0)
    {
        set_color(RGB(1, 1, 1)); // White
    }
    else if (own_gradient == 1)
    {
        set_color(RGB(1, 0, 0)); // Red
    }
    else if (own_gradient == 2)
    {
        set_color(RGB(0, 1, 0)); // Green
    }
    else if (own_gradient == 3)
    {
        set_color(RGB(0, 0, 1)); // Blue
    }
    else if (own_gradient == 4)
    {
        set_color(RGB(1, 0, 1)); // Magenta
    }
    else if (own_gradient >= 5)
    {
        set_color(RGB(1, 1, 0)); // Yellow
    }
}

message_t *message_tx()
{
    return &message;
}

void message_rx(message_t *m, distance_measurement_t *d)
{
    // Only process this message if the previous one has been processed.
    if (new_message == 0)
    {
        new_message = 1;
        received_gradient = m->data[0];
    }
}

int main()
{
    kilo_init();
    kilo_message_rx = message_rx;
    kilo_message_tx = message_tx;
    kilo_start(setup, loop);
    
    return 0;
}
