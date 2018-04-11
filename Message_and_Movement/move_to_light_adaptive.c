#include <kilolib.h>
#define DEBUG
#include <debug.h>


// Constants for motion handling function.
#define STOP 0
#define FORWARD 1
#define LEFT 2
#define RIGHT 3

#define DIFF_THRE 60

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
    // This ensures that the robot starts moving.
    set_motion(LEFT);
    last_update = kilo_ticks;
}

void loop()
{
    printf("light intensity: %d\n", get_ambientlight());
    sample_light();
    if (kilo_ticks > (last_update + 32))
    {
        last_update = kilo_ticks;
        if (current_light < (previous_light-DIFF_THRE))
        {
            set_motion(RIGHT);
        }
        else if (current_light > (previous_light+DIFF_THRE))
        {
            set_motion(LEFT);
        }
        previous_light = current_light;
    }
}

int main()
{
    kilo_init();
    debug_init();
    kilo_start(setup, loop);

    return 0;
}
