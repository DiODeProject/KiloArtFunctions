#include "kilolib.h"

// Constants for motion handling function.
#define STOP 0
#define FORWARD 1
#define LEFT 2
#define RIGHT 3

int current_motion = STOP;
int both_spined = 0;
int current_light = 0;
int previous_light = 0;
uint32_t last_state_update;

//base functions
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
            if(both_spined == 0)
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

void random_move(){

    int random_number = rand_hard();
    int random_direction = (random_number % 2);
    if(random_direction==0){
        set_motion(LEFT);
    }
    else if(random_direction==1){
        set_motion(RIGHT);
    }

}

void setup()
{
  last_state_update = kilo_ticks;
}

void loop()
{
  // Update the state every 64 clock ticks (roughly 2 seconds).
  if (kilo_ticks > (last_state_update + 32))
    {
      last_state_update = kilo_ticks;

        if (current_light > (previous_light))
        {
        	set_motion(FORWARD);
        }
        else
        {
            random_move();
        }
        previous_light = current_light;
    }
}

int main()
{
  kilo_init();
  kilo_start(setup, loop);
    
  return 0;
}
