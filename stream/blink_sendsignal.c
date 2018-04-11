#include "kilolib.h"

// Declare constants.
#define INHALE 0
#define EXHALE 1

// Declare variables.
int state;
uint32_t last_state_update;
message_t message;
// Flag to keep track of message transmission.
int message_sent = 0;

void setup()
{
  state = INHALE;
  last_state_update = kilo_ticks;
    // The type is always NORMAL.
    message.type = NORMAL;
    // Some dummy data as an example.
    message.data[0] = kilo_uid;
    // It's important that the CRC is computed after the data has been set;
    // otherwise it would be wrong and the message would be dropped by the
    // receiver.
    message.crc = message_crc(&message);
}

void loop()
{
  // Update the state every 64 clock ticks (roughly 2 seconds).

  if (kilo_ticks > (last_state_update + 32))
    {
      last_state_update = kilo_ticks;
       
      if (state == INHALE)
        {
	  set_color(RGB(1, 0, 0));
	  // Set the next state.
	  state = EXHALE;
        }
      else if (state == EXHALE)
        {
	  set_color(RGB(0, 0, 0));
	  // Set the next state.
	  state = INHALE;
        }
      if (message_sent == 1)
        {
          // Reset the flag so the LED is only blinked once per message.
          message_sent = 0;
        }
    }
}


message_t *message_tx()
{
    return &message;
}

void message_tx_success()
{
    // Set the flag on message transmission.
    message_sent = 1;
}


int main()
{
  kilo_init();
    kilo_message_tx = message_tx;
    // Register the message_tx_success callback function.
    kilo_message_tx_success = message_tx_success;
  kilo_start(setup, loop);
    
  return 0;
}
