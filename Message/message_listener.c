#include "kilolib.h"


message_t rcvd_message;
//Flag to keep track of message transmission.
int new_message = 0;

void message_rx(message_t *msg, distance_measurement_t *dist){
  rcvd_message = *msg; //store the incoming message
  new_message = 1;  //set flag to 1 indicating message recieved
}

void setup()
{
}

void loop()
{
  // blink the LED magenta whenever a message is sent.
  if (new_message == 1)
    {
      new_message = 0;
      
      set_color(RGB(1,1,0));
      delay(100);
      set_color(RGB(0,0,0));
    }
}


int main()
{
  kilo_init();
  //register tx
  kilo_message_rx = message_rx;
  kilo_start(setup, loop);
    
  return 0;
}
