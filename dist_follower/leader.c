#include <kilolib.h>

#define STOP 0
#define FORWARD 1
#define LEFT 2
#define RIGHT 3

int message_sent = 0;
message_t message;
uint32_t message_last_changed = 0;
int current_motion = STOP;
int current_light = 0;
int previous_light = 0;
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
    int sum = 0;


    while (number_of_samples < 300)
    {
        int sample = get_ambientlight();
        
        // -1 indicates a failed sample, which should be discarded.
        if (sample != -1)
        {
            sum = sum + sample;
            number_of_samples = number_of_samples + 1;
        }
    }

    // Compute the average.
    current_light = sum / number_of_samples;
}

void setup()
{
    set_motion(LEFT);
    last_update = kilo_ticks;
}

void loop()
{
    // Toggle even/odd message every 64 clock ticks (roughly 2 seconds).
    if (kilo_ticks > last_update + 16)
    {
        last_update = kilo_ticks;
        
        set_color(RGB(0, 1, 0));
        set_motion(FORWARD);
        /*    int random_number = rand_hard();
            int random_direction = (random_number % 4);

            if ((random_direction == 0) || (random_direction == 1))
            {
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
            }*/

/*        if (command == 0)
        {
            message.type = NORMAL;
            message.data[0] = 0;
            message.crc = message_crc(&message);
            
            // Update odd for next time.
            command = 1;
        }
        else if(command == 1)
        {
            message.type = NORMAL;
            message.data[0] = 1;
            message.crc = message_crc(&message);
            
            // Update odd for next time.
            command = 2;
        }
        else
        {
            message.type = NORMAL;
            message.data[0] = 2;
            message.crc = message_crc(&message);
            
            // Update odd for next time.
            command = 0;
        }*/

    }
    
    // Blink the LED magenta whenever a message is sent.
    if (message_sent)
    {
    	sample_light();
        message_sent = 0;
    message.type = NORMAL;
    message.data[0] = (current_light > previous_light);
    message.crc = message_crc(&message);
    previous_light = current_light;

 /*       if(command ==0){
            set_color(RGB(1, 0, 1));
        }
        else if(command ==1){
            set_color(RGB(0, 1, 1));
        }
        if(command ==2){
            set_color(RGB(1, 1, 1));
        }
        delay(100);
        set_color(RGB(0, 0, 0));
    }*/
    }
}

message_t *message_tx()
{
    return &message;
}

void message_tx_succes()
{
    message_sent = 1;
}

int main()
{
    kilo_init();
    kilo_message_tx = message_tx;
    kilo_message_tx_success = message_tx_succes;
    kilo_start(setup, loop);

    return 0;
}
