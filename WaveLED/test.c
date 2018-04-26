#include <kilolib.h>
#include <stdio.h>

// Since 1 byte (8 bits) of the message is used to communicate gradients, the
// maximum possible gradient is 2^8 - 1 = 255.
#define GRADIENT_MAX 255
// This is an arbitrary number, but it's important that exactly one robot in the
// group is calibrated to have this ID.
#define SEED_ID 42
#define PERIOD 50
#define PHASE 10

int own_gradient = GRADIENT_MAX;
int last_fire = 0;
int received_gradient = 0;
int received_distance = 0;
int upper_last_fire;
int new_message = 0;
int fire_next = 0;
uint32_t last_gradient_anchored;

int send_message = 0;
message_t message;

message_t *message_tx()
{
	if(send_message == 1){
		return &message;
	}
	return 0;
}

void message_tx_success(){
	send_message = 0;
}

void message_rx(message_t *m, distance_measurement_t *d)
{
	// Only process this message if the previous one has been processed.
	if (new_message == 0)
	{
		new_message = 1;
		received_gradient = m->data[0];
		received_distance = estimate_distance(d);
		upper_last_fire = m->data[2];
	}
}

void setup()
{
    //If the robot is the seed, its gradient should be 0: overwrite the 
    // previously set value of GRADIENT_MAX.
    set_color(RGB(0, 0, 0));
    if (kilo_uid == SEED_ID)
    {
        own_gradient = 0;
		last_fire = kilo_ticks;
		message.type = NORMAL;
		message.data[0] = own_gradient;
		message.data[1] = 0;
		message.data[2] = last_fire;
		message.crc = message_crc(&message);
    }
    
    // Set the transmission message.
}

void loop() {
    // Only pay attention to messages if this robot is not the seed.

	if (kilo_uid == SEED_ID) {
		//kilo_message_tx = message_tx;
		if (kilo_ticks > last_fire + PERIOD) {
			set_color(RGB(1, 1, 1));
			last_fire = kilo_ticks;
		} 
		else
			if (kilo_ticks > last_fire + PHASE && kilo_ticks <= last_fire + PHASE + 1) {
			set_color(RGB(0, 0, 0));
			send_message = 1;
			//kilo_message_tx = message_tx;
			//delay(100);
			//kilo_message_tx = NULL;
		}
		/*else
			if (kilo_ticks > last_fire + PHASE + 2 && kilo_ticks < last_fire + PHASE + 4) {
				kilo_message_tx = NULL;
			}*/
	}
	else
    {
		//kilo_message_tx = message_tx;
        if (new_message == 1)
        {
            new_message = 0;
            if (own_gradient >= received_gradient + 1)
            {
            	//last_fire = kilo_ticks;
				last_gradient_anchored = kilo_ticks;
				fire_next = 1;
            	//kilo_message_tx = message_tx;
				//delay(100);
            }           
			if (own_gradient > received_gradient + 1)
			{
				own_gradient = received_gradient + 1;

			}
        }
        
		if (fire_next==1) {
			if (kilo_ticks > last_fire + PERIOD) {
				set_color(RGB(1, 1, 1));
				last_fire = kilo_ticks;

			}
			else if (kilo_ticks > last_fire + PHASE && kilo_ticks <= last_fire + PHASE + 1) {
				set_color(RGB(0, 0, 0));
				message.type = NORMAL;
				message.data[0] = own_gradient;
				message.data[1] = received_distance;
				message.crc = message_crc(&message);
				send_message = 1;
				fire_next = 0;
			}
			
		}
        // If no neighbor with a gradient of 1 or more less than this robot's
        // gradient is detected within 2 seconds, increment the latter by 1.
        if ((kilo_ticks > (last_gradient_anchored + 64)) && (own_gradient < GRADIENT_MAX))
        {
            own_gradient = own_gradient + 1;
        }
    }
    
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
