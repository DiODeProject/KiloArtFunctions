#include "kilolib.h"

// Declare constants.
#define SEED_ID 42
#define FORWARD 1
#define STOP 2
#define IDLE 3

// Declare variables.
int received_message = 255;
int state;
int current_motion = IDLE;
int energy = 1;
// We need an unsigned 32-bit integer to store clock ticks;
// a regular int would overflow.
uint32_t last_state_update;
int new_message = 0;
message_t message;

void setup()
{
    if (kilo_uid == SEED_ID){
        state = FORWARD;
    }
    else{
        state = STOP;
    }
    // Set the transmission message.
    message.type = NORMAL;
    message.data[0] = IDLE;
    message.crc = message_crc(&message);
  last_state_update = kilo_ticks;
}

void loop()
{
  // Update the state every 64 clock ticks (roughly 2 seconds).
  if (kilo_ticks > (last_state_update + 32))
  {
      last_state_update = kilo_ticks;
      if (new_message == 1){
          if (kilo_uid == SEED_ID){
              if(received_message == 0){
                  state = STOP;
                    message.type = NORMAL;
                    message.data[0] = STOP;
                    message.crc = message_crc(&message);
              }
              if(state == FORWARD){
                    message.type = NORMAL;
                    message.data[0] = FORWARD;
                    message.crc = message_crc(&message);
              }
          }
          else{
              if(received_message == STOP){
                  state = STOP;
                  message.type = NORMAL;
                  message.data[0] = STOP;
                  message.crc = message_crc(&message);
              }
              else if(received_message == FORWARD && energy == 1){
                  state = FORWARD;
                  energy = 0;
                  message.type = NORMAL;
                  message.data[0] = FORWARD;
                  message.crc = message_crc(&message);
              }
         }
         new_message = 0;
     }

     if(state == FORWARD){
          if(current_motion != state){
              current_motion = state;
	      set_color(RGB(1, 0, 0));
	      spinup_motors();
	      set_motors(kilo_straight_left, kilo_straight_right);
          }
            
     }
     else if(state == STOP){

          if(current_motion != state){
              current_motion = state;
	      set_color(RGB(0, 0, 1));
	      set_motors(0, 0);
          }
            
     }
  }
}

message_t *message_tx()
{
    return &message;
}

void message_rx(message_t *m, distance_measurement_t *d)
{
    // Only process this message if the previous one has been processed.
    if (new_message == 0)
    {
        new_message = 1;
        received_message = m->data[0];
    }
}

int main()
{
  kilo_init();
  kilo_message_rx = message_rx;
  kilo_message_tx = message_tx;
  kilo_start(setup, loop);
    
  return 0;
}
