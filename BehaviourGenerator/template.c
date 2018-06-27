#include "kilolib.h"

// Declare constants.

//kilobots' motion state
#define STOP 0
#define LEFT 1
#define RIGHT 2
#define STRAIGHT 3

//kilobots' LED colour state
#define BLACK 0
#define RED 1
#define GREEN 2
#define BLUE 3
#define WHITE 4
#define YELLOW 5
#define MAGENTA 6
#define CYAN 7

int current_motion = STOP;
int current_colour = BLACK;
int both_spined = 0;		//check if the motorss have spined or not
int current_action = 0;		//iterating the action number
int send_message = 0;		//whether it is time to send message
int new_message = 0;		//whether received a new message

// We need an unsigned 32-bit integer to store clock ticks;
// a regular int would overflow.
uint8_t top_secret = 99;	//the random message data to be transmitted
uint32_t last_received;		//the time when last received a message
uint32_t last_state_update;	

//INSERT GLOBAL VARIABLES HERE

message_t message;			//message object

//message transmitting function
message_t *message_tx()
{
	if (send_message == 1) {
		return &message;
	}
	return 0;
}

//actions when message successfully transmitted
void message_tx_success() {
	send_message = 0;
}

//message receiving function
void message_rx(message_t *m, distance_measurement_t *d)
{
	// Only process this message if the previous one has been processed.
	if (new_message == 0)
	{
		new_message = 1;
		last_received = kilo_ticks;
	}
}

void set_colour(int new_colour) {

	//only update if colour changes
	if(current_colour != new_colour){

		current_colour =new_colour;

		//changing the colour by RGB value, 2^3=8 possible states
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
		else if (current_colour == YELLOW) {
			set_color(RGB(1, 1, 0));
		}
		else if (current_colour == MAGENTA) {
			set_color(RGB(1, 0, 1));
		}
		else if (current_colour == CYAN) {
			set_color(RGB(0, 1, 1));
		}
	}
}

void set_motion(int new_motion)
{
	// Only take an action if the motion is being changed.
	if (current_motion != new_motion)
	{
		current_motion = new_motion;

		//can define customer actions here, e.g. vibrating 7 times
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

//set customer speed, takes 2 floats,
//the left-motor and right-motor speed scale from calibrated straight value
void set_speed(float left, float right)
{
    if(both_spined == 0)
        spinup_motors();
    set_motors((uint8_t)(((float)kilo_straight_left)*left), (uint8_t)(((float)kilo_straight_right)*right));
    both_spined = 1;

}

//initialising parameters
void setup()
{
	last_state_update = kilo_ticks;
	message.type = NORMAL;
	message.data[0] = top_secret;
	message.crc = message_crc(&message);
}

//main loop
void loop()
{
	//INSERT CODE HERE

	//reset the receiver after 5 ticks
	if (kilo_ticks > last_received + 5)
		new_message = 0;
}

int main()
{
	kilo_init();
	kilo_message_rx = message_rx;
	kilo_message_tx = message_tx;
	kilo_message_tx_success = message_tx_success;
	kilo_start(setup, loop);
	return 0;
}