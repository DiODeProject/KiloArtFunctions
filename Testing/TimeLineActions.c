#include "kilolib.h"

// Declare constants.
#define ACTION_STOP 0
#define ACTION_LEFT 1
#define ACTION_RIGHT 2
#define ACTION_FORWARD 3

#define COLOUR_NONE 0
#define COLOUR_RED 1
#define COLOUR_GREEN 2
#define COLOUR_BLUE 3

#define PERIOD 32
#define NCYCLE 10

// Declare variables.
int state;
int current_motion = ACTION_STOP;
int both_spined = 0;
int current_cycle = 0;

int actions_line[NCYCLE] = {ACTION_STOP,
	ACTION_STOP,
	ACTION_STOP,
	ACTION_FORWARD,
	ACTION_FORWARD,
	ACTION_LEFT,
	ACTION_LEFT,
	ACTION_FORWARD,
	ACTION_FORWARD};
	
int colours_line[NCYCLE] = actions_line;
// We need an unsigned 32-bit integer to store clock ticks;
// a regular int would overflow.
uint32_t last_state_update;
uint32_t color_update;

void set_colour(int colour){
	if(colour == COLOUR_NONE){
		set_color(RGB(0,0,0));
	}else if (colour == COLOUR_RED){
		set_color(RGB(1,0,0));
	}else if (colour == COLOUR_GREEN){
		set_color(RGB(0,1,0));
	}else if (colour == COLOUR_BLUE){
		set_color(RGB(0,0,1));
	}
}

void set_motion(int new_motion)
{
    // Only take an action if the motion is being changed.
    if (current_motion != new_motion)
    {
        current_motion = new_motion;
        
        if (current_motion == ACTION_STOP)
        {
            set_motors(0, 0);
            both_spined = 0;
        }
        else if (current_motion == ACTION_FORWARD)
        {
        	if(both_spined==0)
        		spinup_motors();
            set_motors(kilo_straight_left, kilo_straight_right);
            both_spined = 1;
        }
        else if (current_motion == ACTION_LEFT)
        {
            if(both_spined == 0)
                spinup_motors();
            set_motors(kilo_turn_left, 0);
            both_spined = 0;
        }
        else if (current_motion == ACTION_RIGHT)
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
  last_state_update = kilo_ticks;
  //color_update = kilo_ticks;
}

void loop()
{

  if(kilo_ticks > last_state_update + PERIOD){
      set_motion(actions_line[current_cycle]);
      set_colour(colours_line[current_cycle]);

      last_state_update = kilo_ticks;
      current_cycle = (current_cycle + 1)%NCYCLE;
  }

}

int main()
{
  kilo_init();
  kilo_start(setup, loop);
    
  return 0;
}
