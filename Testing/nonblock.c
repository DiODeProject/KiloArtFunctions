#include "kilolib.h"

// Declare constants.
#define STOP 0
#define FORWARD 1
#define LEFT 2
#define RIGHT 3

#define FLASH_DELAY 100

// Declare variables.
int state;
int current_motion = STOP;
int both_spined = 0;
// We need an unsigned 32-bit integer to store clock ticks;
// a regular int would overflow.
uint32_t last_state_update;
uint32_t color_update;

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

void setup()
{
  last_state_update = kilo_ticks;
  color_update = kilo_ticks;
      int random_number = rand_hard();
      int random_color = (random_number % 7);
      if(random_color == 0){
        set_color(RGB(0, 0, 1));

      } else if(random_color == 1 ){
        set_color(RGB(1, 1, 1));

      }else if(random_color == 2 ){
        set_color(RGB(0, 1, 0));

      }else if(random_color == 3 ){
        set_color(RGB(0, 1, 1));

      }else if(random_color == 4 ){
        set_color(RGB(1, 0, 0));

      }else if(random_color == 5 ){
        set_color(RGB(1, 0, 1));

      }else if(random_color == 6 ){
        set_color(RGB(1, 1, 0));

      }
}

void loop()
{

  if(kilo_ticks > last_state_update + 32){
      last_state_update = kilo_ticks;


    int random_number = rand_hard();
      int random_direction = (random_number % 4);

      if(random_direction==0||random_direction==1){
      set_motion(FORWARD);
      }
      else if(random_direction==2){
      set_motion(LEFT);
      }
      else if(random_direction==3){
      set_motion(RIGHT);
      }
  }

  if(kilo_ticks > color_update + 320*4){
      color_update = kilo_ticks;
      int random_number = rand_hard();
      int random_color = (random_number % 7);
      if(random_color == 0){
        set_color(RGB(0, 0, 1));

      } else if(random_color == 1 ){
        set_color(RGB(1, 1, 1));

      }else if(random_color == 2 ){
        set_color(RGB(0, 1, 0));

      }else if(random_color == 3 ){
        set_color(RGB(0, 1, 1));

      }else if(random_color == 4 ){
        set_color(RGB(1, 0, 0));

      }else if(random_color == 5 ){
        set_color(RGB(1, 0, 1));

      }else if(random_color == 6 ){
        set_color(RGB(1, 1, 0));

      }
  }
  /*
  flash_led();
  flash_led();
  flash_led();
  laughing();
  laughing();
  laughing();
  laughing();
  laughing();
  laughing();
  */
}

int main()
{
  kilo_init();
  kilo_start(setup, loop);
    
  return 0;
}
