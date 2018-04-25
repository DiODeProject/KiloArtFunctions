#include "kilolib.h"

int last_update = 0;
//main functions
void setup()
{
	last_update = kilo_ticks;
}

void laughing(){
	set_motors(100,100);
	delay(270);
	set_motors(0,0);
	delay(100);
}
void loop()
{
	laughing();
	delay(370);
	laughing();
	delay(370);
	laughing();
	laughing();
	laughing();
	delay(370);
}

int main()
{
  kilo_init();
  kilo_start(setup, loop);
    
  return 0;
}
