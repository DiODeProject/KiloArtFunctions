#include "kilolib.h"
#define DEBUG
#include <debug.h>


void setup()
{
}

void loop()
{
printf("volta : %d\n",get_voltage());
}

int main()
{
  kilo_init();
  debug_init();
  kilo_start(setup, loop);
    
  return 0;
}
