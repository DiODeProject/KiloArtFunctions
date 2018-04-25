#include "kilolib.h"

// Declare constants.
#define STOP 0
#define FORWARD 1
#define LEFT 2
#define RIGHT 3

#define TELLER 42
#define LISTENER 0

// Declare variables.
int state;
int current_motion = STOP;
int both_spined = 0;

uint32_t last_state_update;
message_t message;
int message_sent = 0;
int message_recieved = 0;
int pre_distance = 0;
int current_distance = 255;
int current_minidistance = 255;

int role = 0;

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
    int random_direction = (random_number % 4);

    if(random_direction==0||random_direction==1){
    	set_color(RGB(0, 1, 0));
		set_motion(FORWARD);
    }
    else if(random_direction==2){
    	set_color(RGB(1, 0, 0));
		set_motion(LEFT);
    }
    else if(random_direction==3){
    	set_color(RGB(0, 0, 1));
		set_motion(RIGHT);
	}

}

void setup()
{
  last_state_update = kilo_ticks;
  role = kilo_uid;
}

void loop()
{
	if(current_minidistance > current_distance){
		current_minidistance = current_distance;
	}
	if(kilo_ticks > last_state_update + 16){

		  if(role == TELLER){

			set_color(RGB(0,0,0));
			set_motion(STOP);
			// The type is always NORMAL.
			message.type = NORMAL;
			// Some dummy data as an example.
			message.data[0] = TELLER;
			// It's important that the CRC is computed after the data has been set;
			// otherwise it would be wrong.
			message.crc = message_crc(&message);
		  }

		if(role == LISTENER){
			if(message_recieved == 1){
				message_recieved = 0;
				if(current_minidistance > pre_distance){
					set_color(RGB(1,1,1));
					set_motion(FORWARD);
				}else if (current_minidistance < 50){
					set_color(RGB(0,0,0));
					set_motion(STOP);
					role = TELLER;
				}else{
					set_color(RGB(1,1,0));
					set_motion(FORWARD);
				}
				pre_distance = current_distance;
			}else{
					set_color(RGB(1,1,0));
					set_motion(FORWARD);
			}
		}
		last_state_update = kilo_ticks;
		current_minidistance = 255;
	}
}
//internal functions
message_t *message_tx()
{
    return &message;
}

void message_tx_success()
{
    // Set flag on message transmission.
    message_sent = 1;
}

void message_rx(message_t *m, distance_measurement_t *d)
{
    if (message_recieved == 0){
        message_recieved = 1;
        current_distance = estimate_distance(d);

    }
}


int main(){

  kilo_init();
  kilo_message_tx = message_tx;
  kilo_message_tx_success = message_tx_success;
  kilo_message_rx = message_rx;
  kilo_start(setup, loop);
    
  return 0;

}
