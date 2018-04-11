#include <kilolib.h>
#define DEBUG
#include <debug.h>

#define LIGHT_THRES 500

int current_light = 0;

void setup()
{
    set_color(RGB(0,0,1));
}

// Function to sample light.
void sample_light()
{
    // The ambient light sensor gives noisy readings. To mitigate this,
    // we take the maximum of 300 samples in quick succession.
    
    int number_of_samples = 0;
    int max_light = 0;


    while (number_of_samples < 300)
    {
        int sample = get_ambientlight();
        
        // -1 indicates a failed sample, which should be discarded.
        if (max_light < sample)
        {
            max_light = sample;
        }
        number_of_samples = number_of_samples + 1;
    }

    // Compute the average.
    current_light = max_light;
    printf("light intensity: %d\n", current_light);
}

void loop()
{
    sample_light();
    if (current_light > LIGHT_THRES){
        set_color(RGB(1,0,0));
    }
    else{
        set_color(RGB(0,0,1));
    }


}

int main()
{
    kilo_init();
    debug_init();
    kilo_start(setup, loop);
    
    return 0;
}
