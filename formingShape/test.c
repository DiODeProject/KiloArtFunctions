#include "kilolib.h"
#include "math.h"

// Declare constants.
#define INIT_X 0
#define INIT_Y 0
#define INIT_ID 40
#define SEC_X 0
#define SEC_Y 4
#define SEC_ID 41
#define PRECISION 8

#define PI 3.14159265
#define COORD_CAL_SIZE 12

struct Coords
{
	int x;
	int y;
	int distance;
	
};

// Declare variables.
int state;
int scale = 1;
int data0;
int data1;
int data2;
int data3;
int self_x = -1;
int self_y = -1;
int sender_id = -1;
Coords* coordinates[5];
int cool_down = 0;
// We need an unsigned 32-bit integer to store clock ticks;
// a regular int would overflow.
uint32_t last_state_update;
message_t message;
// Flag to keep track of message transmission.
int message_sent = 0;
int message_received = 0;


//structure for storing coordinates information

void calculate_coordinates(){
	//other bots receives distance between INIT and SEC bots
	//for each distance-coordinates
		//calculate e.g. 90 possible coordinates
	//compare first and second coordinates, choose the similar as coordinates

	int coords_count[COORD_CAL_SIZE] = {0};
	int coordsx_option[COORD_CAL_SIZE];
	int coordsy_option[COORD_CAL_SIZE];

	double per_size = 2*PI/COORD_CAL_SIZE;
	for(int i=0; i<COORD_CAL_SIZE; i++){
		coordsx_option[i] = coordinates[0].x + (int)(((double)coordinates[0].distance) * cos(i*per_size));
		coordsy_option[i] = coordinates[0].y + (int)(((double)coordinates[0].distance) * sin(i*per_size));

	}

	for(int i=1; i<5; i++){
		if(!coordinates[i]){
			break;
		}
		else {
			for(int j=0; j<COORD_CAL_SIZE; j++){
				int x = coordinates[i].x + (int)(((double)coordinates[0].distance) * cos(j*per_size));
				int y = coordinates[i].y + (int)(((double)coordinates[0].distance) * sin(j*per_size));

				if(x==coordsx_option[j] && y==coordsy_option[j]){
					coords_count[j] += 1
				}

			}
		}
	}

}

void calibrate_scale(Coords c){
	scale = c.distance/self_x;
	//SEC bot receives INI bot, measure the distance
	//set scale as distance over normalised coordination distance
}

void store_coordinates(int x, int y, int distance){
	// store different coords in 5 coords
	for(int i=0; i<5; i++){
		if(!coordinates[i]){
			coordinates[i].x = x;
			coordinates[i].y = y;
			coordinates[i].distance = distance;
			break;
		} else {
			if(coordinates[i].x == x &&	coordinates[i].y = y){
				coordinates[i].distance = distance;
				break;
			}
		}
	}
}
/*
int float2int(float a){
	return int(a*PRECISION)
}

int int2float(int a){
	return float(a)/PRECISION
}*/

void setup()
{
	if(kilo_uid == INIT_ID){
		self_x = 0;
		self_y = 0;
	    // Initialize message:
	    // The type is always NORMAL.
	    message.type = NORMAL;
	    // Some dummy data as an example.
	    message.data[0] = float2int(self_x);
	    message.data[1] = float2int(self_y);
	    message.data[2] = kilo_uid;
	    // It's important that the CRC is computed after the data has been set;
	    // otherwise it would be wrong.
	    message.crc = message_crc(&message);
		//send data0: X, data1: Y, data2: ID
	} else if(kilo_uid == SEC_ID){
		self_x = 1;
		self_y = 0;
	    // Initialize message:
	    // The type is always NORMAL.
	    message.type = NORMAL;
	    // Some dummy data as an example.
	    message.data[0] = float2int(self_x);
	    message.data[1] = float2int(self_y);
	    message.data[2] = kilo_uid;
	    // It's important that the CRC is computed after the data has been set;
	    // otherwise it would be wrong.
	    message.crc = message_crc(&message);
		//send data0: X, data1: Y, data2: ID
		//send data0: X, data1: Y, data2: ID
	}
  last_state_update = kilo_ticks;
}

void loop()
{
  // Update the state every 64 clock ticks (roughly 2 seconds).
  if (kilo_ticks > (last_state_update + 16))
    {
      last_state_update = kilo_ticks;
        
		//SEC bot receives signal from INI bot and calibrate the scale
		if(kilo_uid == SEC_ID){
			if(sender_id==INIT_ID){
				calibrate_scale();
			//after calibration, SEC bot send scale to others. data3: scale
		    	message.data[3] = scale;
		    	message.crc = message_crc(&message);
			}
		}

		//other kilobots try to fix the scale
		if(kilo_uid!=SEC_ID){
			if(data3!=0){
				scale = data3;
		    	message.data[3] = scale;
		    	message.crc = message_crc(&message);			
			}
		}

		//when it stop moving
		if(cool_down==1){
		    message.type = NORMAL;
		    // Some dummy data as an example.
		    message.data[0] = float2int(self_x);
		    message.data[1] = float2int(self_y);
		    message.data[2] = kilo_uid;
			
		}
		//other kilobots move and calculate the coordinates, if destination, stop 
		//static kilobots start to send signals //send data0: X, data1: Y, data2: ID data3: scale
		//movement: keeping fixed distance from nearest neibourgh
		if(kilo_uid!=INIT_ID && kilo_uid!=SEC_ID){
			calculate_coordinates();
		}
    }
}

message_t *message_tx(){
	return &message;
}

void message_tx_success(){
	message_sent = 1;
}

void message_rx(message_t *m, distance_measurement_t *d)
{
	if(message_received == 0){
		message_received = 1;
		data0 = m->data[0];
		data1 = m->data[1];
		data2 = m->data[2];
		data3 = m->data[3];
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
