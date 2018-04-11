#include <kilolib.h>
#define DEBUG
#include <debug.h>


// Constants for motion handling function.
#define STOP 0
#define FORWARD 1
#define LEFT 2
#define RIGHT 3

//#define DIFF_THRE 60

int received_message = 0;
int new_message = 0;
int current_motion = STOP;
int current_distance = 0;
int previous_distance = 0;
int both_spined = 0;
//int current_light = 0;
//int previous_light = 0;
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

// Function to sample light.
/*void sample_light()
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
}*/

void setup()
{
    // This ensures that the robot starts moving.
    set_motion(FORWARD);
    last_update = kilo_ticks;
}

void loop()
{
    //printf("light intensity: %d\n", get_ambientlight());
    //sample_light();
    if (kilo_ticks > (last_update + 16))
    {
       last_update = kilo_ticks;
        if (new_message == 1){
            new_message = 0;
            if (current_distance < previous_distance)
            {
                set_color(RGB(0, 1, 0));
                set_motion(FORWARD);
            }
            
        }
        else{
            int random_number = rand_hard();
            int random_direction = (random_number % 2);

            if (random_direction == 0)
            {
                set_color(RGB(1, 0, 0));
                set_motion(LEFT);
            }
            else if (random_direction == 1)
            {
                set_color(RGB(0, 0, 1));
                set_motion(RIGHT);
            }


    }

    previous_distance = current_distance;

        //previous_light = current_light;
    }
}

void message_rx(message_t *m, distance_measurement_t *d)
{
    // Only process this message if the previous one has been processed.
    if (new_message == 0)
    {
        new_message = 1;
        current_distance = estimate_distance(d);
        received_message = m->data[0];
    }
}

int main()
{
    kilo_init();
    kilo_message_rx = message_rx;
    debug_init();
    kilo_start(setup, loop);

    return 0;
}
