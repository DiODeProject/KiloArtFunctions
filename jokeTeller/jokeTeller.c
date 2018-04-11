#include "kilolib.h"

// Declare constants.
#define STOP 0
#define FORWARD 1
#define LEFT 2
#define RIGHT 3

//messages
#define FINDING_MESSAGE 10
#define IM_HERE 11
#define JOKE_MESSAGE 12

//roles
#define TELLER 20
#define ROADPEOPLE 21

//special_id
#define FIRST_TELLER_ID 42

#define FLASH_DELAY 100

// Declare variables.
int state;
int current_motion = STOP;
int both_spined = 0;
// We need an unsigned 32-bit integer to store clock ticks;
// a regular int would overflow.
uint32_t last_state_update;


message_t message;
// Flag to keep track of message transmission.
int message_sent = 0;
int message_recieved = 0;
int message_content = 0;

int role;

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

void flash_led(){
		
	  		set_color(RGB(0, 0, 1));
	  		delay(FLASH_DELAY);
	  		set_color(RGB(0, 1, 0));
	  		delay(FLASH_DELAY);
	  		set_color(RGB(0, 1, 1));
	  		delay(FLASH_DELAY);
	  		set_color(RGB(1, 0, 0));
	  		delay(FLASH_DELAY);
	  		set_color(RGB(1, 0, 1));
	  		delay(FLASH_DELAY);
	  		set_color(RGB(1, 1, 0));
	  		delay(FLASH_DELAY);
	  		set_color(RGB(1, 1, 1));
	  		delay(FLASH_DELAY);
}

//advance functions
void laughing(){
	spinup_motors();
	delay(30);
	set_motors(0,0);
	delay(150);
}

void find_others(){
	// Initialize message:
	// The type is always NORMAL.
	message.type = NORMAL;
	// Some dummy data as an example.
	message.data[0] = FINDING_MESSAGE;
	// It's important that the CRC is computed after the data has been set;
	// otherwise it would be wrong.
	message.crc = message_crc(&message);

	random_move();

}

void telling_joke(){
	// Initialize message:
	// The type is always NORMAL.
	message.type = NORMAL;
	// Some dummy data as an example.
	message.data[0] = JOKE_MESSAGE;
	// It's important that the CRC is computed after the data has been set;
	// otherwise it would be wrong.
	message.crc = message_crc(&message);

	flash_led();
	flash_led();
	flash_led();
	laughing();
	laughing();
	laughing();
	laughing();
	laughing();
	laughing();
}

void listening_joke(){

	role = TELLER;

	flash_led();
	flash_led();
	flash_led();
	laughing();
	laughing();
	laughing();
	laughing();
	laughing();
	laughing();

}

void find_and_tell(){

    if (message_sent == 1)
    {
        message_sent = 0;
    }

	if (kilo_ticks > (last_state_update + 16)){

		last_state_update = kilo_ticks;

	    if(message_recieved == 1){
	    	message_recieved = 0;
	    	if(message_content==IM_HERE){
	    		set_motion(STOP);
	    		telling_joke();
	    	}
	    	else{
	    		find_others();
	    	}
	    }
	    else{
		    	find_others();

		}
    }
}

void settle_and_listen(){

    if (message_sent == 1)
    {
        message_sent = 0;
    }

	if (kilo_ticks > (last_state_update + 16)){

		last_state_update = kilo_ticks;

	    if(message_recieved == 1){
	    	message_recieved = 0;
	    	if(message_content==FINDING_MESSAGE){
				// Initialize message:
				// The type is always NORMAL.
				message.type = NORMAL;
				// Some dummy data as an example.
				message.data[0] = IM_HERE;
				// It's important that the CRC is computed after the data has been set;
				// otherwise it would be wrong.
				message.crc = message_crc(&message);
	    	}
	    	if(message_content==JOKE_MESSAGE){
	    		set_motion(STOP);
	    		listening_joke();
	    	}
	    }
	    else{
		    	random_move();

		}
	}
}

//main functions
void setup()
{
	//state = FORWARD_GREEN;
	last_state_update = kilo_ticks;
	if(kilo_uid==FIRST_TELLER_ID){
		role = TELLER;
	}else{
		role = ROADPEOPLE;
	}
}

void loop()
{
	if(role == TELLER){
		find_and_tell();
	}else
	if(role == ROADPEOPLE){
		settle_and_listen();
	}
  // Update the state every 64 clock ticks (roughly 2 seconds).f
  /*if (kilo_ticks > (last_state_update + 64))
    {
      last_state_update = kilo_ticks;
        
      if (state == FORWARD_GREEN)
        {
	  set_color(RGB(0, 1, 0));
	  spinup_motors();
	  set_motors(kilo_straight_left, kilo_straight_right);
            
	  // Set the next state.
	  state = LEFT_RED;
        }
      else if (state == LEFT_RED)
        {
	  set_color(RGB(1, 0, 0));
	  spinup_motors();
	  set_motors(kilo_turn_left, 0);
            
	  // Set the next state.
	  state = RIGHT_BLUE;
        }
      else if (state == RIGHT_BLUE)
        {
	  set_color(RGB(0, 0, 1));
	  spinup_motors();
	  set_motors(0, kilo_turn_right);
            
	  // Set the next state.
	  state = FORWARD_GREEN;
        }
    }*/
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
        message_content = m->data[0];
    }
}


int main()
{
  kilo_init();
  kilo_message_tx = message_tx;
  kilo_message_tx_success = message_tx_success;
  kilo_message_rx = message_rx;
  kilo_start(setup, loop);
    
  return 0;
}
