#include "kilolib.h"

// Declare constants.
#define SEED_ID 42
#define STOP 0
#define FORWARD 1
#define LEFT 2
#define RIGHT 3

// Declare variables.
int received_message = 100;
int state = 0;
int current_motion = STOP;
int motion = STOP;
int both_spined = 0;
//int energy = 1;
// We need an unsigned 32-bit integer to store clock ticks;
// a regular int would overflow.
uint32_t last_state_update;
int new_message = 0;
message_t message;


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
            delay(1000);
                spinup_motors();
            set_motors(kilo_straight_left, kilo_straight_right);
 //           both_spined = 0;
        }
        else if (current_motion == RIGHT)
        {
            if(both_spined == 0)
                spinup_motors();
            set_motors(0, kilo_turn_right);
            delay(1000);
                spinup_motors();
            set_motors(kilo_straight_left, kilo_straight_right);
 //           both_spined = 0;
        }
    }
}


void setup()
{
//    if (kilo_uid == SEED_ID){
//        state = FORWARD;
//    }
//    else{
 //       state = STOP;
 //   }
    // Set the transmission message.
//    current_motion = FORWARD;
    //motion = FORWARD;
    /*message.type = NORMAL;
    message.data[0] = state;
    message.crc = message_crc(&message);*/
  last_state_update = kilo_ticks;
}

void loop()
{
  // Update the state every 64 clock ticks (roughly 2 seconds).
  if (kilo_ticks > (last_state_update + 16))
  {
      last_state_update = kilo_ticks;
      if (kilo_uid==1){
        motion = FORWARD;
            message.type = NORMAL;
            message.data[0] = state;
            message.crc = message_crc(&message);
      }
      else if (new_message == 1){
            //if (state == 0){
            //}
            /*else if(state == 1){
                motion = LEFT;
            }
            else if(state == 2){
                motion = RIGHT;
            }*/

            if(received_message < 40){
                //state = received_message;
            //}
              motion = FORWARD;

              message.type = NORMAL;
              message.data[0] = state;
              message.crc = message_crc(&message);
            }
 /*         if (kilo_uid == SEED_ID){
              if(received_message == 0){
                  state = STOP;
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
         }*/
         new_message = 0;
     }

       if(motion == FORWARD){
	      set_color(RGB(0, 1, 0));
       }
       else if(motion == LEFT){
	      set_color(RGB(1, 0, 0));
       }
       else if(motion == RIGHT){
	      set_color(RGB(0, 0, 1));
       }
       set_motion(motion);


/*     if(current_motion == FORWARD){
//          if(current_motion != state){
//              current_motion = state;
	      set_color(RGB(1, 0, 0));
	      spinup_motors();
	      set_motors(kilo_straight_left, kilo_straight_right);
//          }
            
     }
     else if(current_motion == STOP){

          if(current_motion != state){
              current_motion = state;
	      set_color(RGB(0, 0, 1));
	      set_motors(0, 0);
          }
            
     }*/
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
        received_message = estimate_distance(d);
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
