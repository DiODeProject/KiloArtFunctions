#include "kilolib.h"

// Declare constants.
#define STOP 0
#define LEFT 1
#define RIGHT 2
#define STRAIGHT 3

#define BLACK 0
#define RED 1
#define GREEN 2
#define BLUE 3
#define WHITE 4

int current_motion = STOP;
int current_colour = BLACK;
int both_spined = 0;
int current_action = 0;

// We need an unsigned 32-bit integer to store clock ticks;
// a regular int would overflow.
uint32_t last_state_update;

void set_colour(int new_colour) {

	//only update if colour changes
	if(current_colour != new_colour)
	if (current_colour == BLACK) {
		set_color(RGB(0, 0, 0));
	}
	else if (current_colour == RED) {
		set_color(RGB(1, 0, 0));
	}
	else if (current_colour == GREEN) {
		set_color(RGB(0, 1, 0));
	}
	else if (current_colour == BLUE) {
		set_color(RGB(0, 0, 1));
	}
	else if (current_colour == WHITE) {
		set_color(RGB(1, 1, 1));
	}
}

void set_motion(int new_motion)
{
	// Only take an action if the motion is being changed.
	if (current_motion != new_motion)
	{
		current_motion = new_motion;

		if (current_motion == STOP)
		{
			set_motors(0, 0);
			both_spined = 0;
		}
		else if (current_motion == STRAIGHT)
		{
			if (both_spined == 0)
				spinup_motors();
			set_motors(kilo_straight_left, kilo_straight_right);
			both_spined = 1;
		}
		else if (current_motion == LEFT)
		{
			if (both_spined == 0)
				spinup_motors();
			set_motors(kilo_turn_left, 0);
			both_spined = 0;
		}
		else if (current_motion == RIGHT)
		{
			if (both_spined == 0)
				spinup_motors();
			set_motors(0, kilo_turn_right);
			both_spined = 0;
		}
	}
}

void setup()
{
	last_state_update = kilo_ticks;
}

void loop()
{
	//INSERT CODE HERE
}

int main()
{
	kilo_init();
	kilo_start(setup, loop);
	return 0;
}