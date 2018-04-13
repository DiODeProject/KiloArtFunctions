#include "kilolib.h"

// Declare constants.
#define STOP 0
#define FORWARD 1
#define LEFT 2
#define RIGHT 3

// Declare variables.
int current_motion = STOP;
int both_spined = 0;
// We need an unsigned 32-bit integer to store clock ticks;
// a regular int would overflow.
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

void set_speed(float left, float right)
{
    if(both_spined == 0)
        spinup_motors();
    set_motors((uint8_t)(((float)kilo_straight_left)*left), (uint8_t)(((float)kilo_straight_right)*right));
    both_spined = 1;

}

void setup()
{
  last_state_update = kilo_ticks;
}

void loop()
{
  // Update the state every 64 clock ticks (roughly 2 seconds).
  if (kilo_ticks > (last_state_update + 64))
    {
    	set_color(RGB(0, 1, 0));
		set_speed(1.15, 1.15);
    }
}

int main()
{
  kilo_init();
  kilo_start(setup, loop);
    
  return 0;
}
