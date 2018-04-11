#include <kilolib.h>

// Nominal period with which to blink; approximately 2 seconds. This will be
// be the period of every robot once the swarm has synchronized.
#define PERIOD 50
// Affects the size of the reset time adjustment for every discrepancy
// with a neighbor. A larger value means smaller adjustments. As a rule of thumb
// this value should increase with the average number of neighbors each robot has.
#define RESET_TIME_ADJUSTMENT_DIVIDER 120
// A cap on the absolute value of the reset time adjustment.
#define RESET_TIME_ADJUSTMENT_MAX 30

#define STOP 0
#define FORWARD 1
#define LEFT 2
#define RIGHT 3

int current_motion = STOP;
int both_spined = 0;
uint32_t reset_time = 0;
uint32_t last_reset = 0;
int reset_time_adjustment = 0;
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
            both_spined = 0;
        }
        else if (current_motion == FORWARD)
        {
            //if(both_spined == 0)
            spinup_motors();
            //set_motors(kilo_straight_left, kilo_straight_right);
            set_motors(0,0);
            both_spined = 1;
        }
        else if (current_motion == LEFT)
        {
            //if(both_spined == 0)
                spinup_motors();
            set_motors(kilo_turn_left, 0);
            both_spined = 0;
        }
        else if (current_motion == RIGHT)
        {
            //if(both_spined == 0)
                spinup_motors();
            set_motors(0, kilo_turn_right);
            both_spined = 0;
        }
    }
}

void setup()
{
    // Set the message.
    message.type = NORMAL;
    message.data[0] = 0;
    message.crc = message_crc(&message);
    
    // Introduce a random delay so the robots don't become instantly
    // synchronized by the run signal.
    //set_color(RGB(1, 0, 0));
    //delay(10 * rand_hard());
    //set_color(RGB(0, 0, 0));
}

void loop()
{
    if (kilo_ticks >= reset_time)
    {
        reset_time_adjustment = (reset_time_adjustment / RESET_TIME_ADJUSTMENT_DIVIDER);
        
        // Apply a cap to the absolute value of the reset time adjustment.
        if (reset_time_adjustment < - RESET_TIME_ADJUSTMENT_MAX)
        {
            reset_time_adjustment = - RESET_TIME_ADJUSTMENT_MAX;
        }
        else if (reset_time_adjustment > RESET_TIME_ADJUSTMENT_MAX)
        {
            reset_time_adjustment = RESET_TIME_ADJUSTMENT_MAX;
        }
        
        last_reset = kilo_ticks;
        reset_time = kilo_ticks + PERIOD + reset_time_adjustment;
        
        reset_time_adjustment = 0;
        
        // Set the LED white and turn the motors on.

        int random_number = rand_hard();
        int rand_color = random_number%7;
        int rand_dir = random_number%4;
        if (rand_color == 0){
        	set_color(RGB(0, 0, 1));
        }
        else if (rand_color == 1){
        	set_color(RGB(0, 1, 0));
        }
        else if (rand_color == 2){
        	set_color(RGB(0, 1, 1));
        }
        else if (rand_color == 3){
        	set_color(RGB(1, 0, 0));
        }
        else if (rand_color == 4){
        	set_color(RGB(1, 0, 1));
        }
        else if (rand_color == 5){
        	set_color(RGB(1, 1, 0));
        }
        else if (rand_color == 6){
        	set_color(RGB(1, 1, 1));
        }


        if (rand_dir == 0){
        	set_motion(LEFT);
        }
        else if (rand_dir == 1){
        	set_motion(LEFT);
        }
        else if (rand_dir == 2){
        	set_motion(RIGHT);
        }
        else if (rand_dir == 3){
        	set_motion(RIGHT);
        }
    }
    // After 1 clock tick, turn the LED and motors off.
    /*else if (kilo_ticks > (last_reset + 1))
    {
        set_color(RGB(0, 0, 0));
        set_motors(0, 0);
    }*/
    
    // Only send the current time if it can fit in 1 byte (8 bits), which
    // corresponds to a maximum of 2^8 - 1 = 255. Otherwise, set the message
    // CRC to 0 so neighbors ignore the message.
    if ((kilo_ticks - last_reset) < 255)
    {
        message.data[0] = kilo_ticks - last_reset;
        message.crc = message_crc(&message);
    }
    else
    {
        message.crc = 0;
    }
}

message_t *message_tx()
{
    return &message;
}

void message_rx(message_t *m, distance_measurement_t *d)
{
    int my_timer = kilo_ticks - last_reset;
    int rx_timer = m->data[0];
    int timer_discrepancy = my_timer - rx_timer;
    
    // Reset time adjustment due to this message - to be combined with the
    // overall reset time adjustment.
    int rx_reset_time_adjustment = 0;
    
    if (timer_discrepancy > 0)
    {
        // The neighbor is trailing behind: move the reset time forward
        // (reset later).
        if (timer_discrepancy < (PERIOD / 2))
        {
            rx_reset_time_adjustment = timer_discrepancy;
        }
        // The neighbor is running ahead: move the reset time backward
        // (reset sooner).
        else
        {
            rx_reset_time_adjustment = - (PERIOD - timer_discrepancy) % PERIOD;
        }
    }
    else if (timer_discrepancy < 0)
    {
        // The neighbor is running ahead: move the reset time backward
        // (reset sooner).
        if (- timer_discrepancy < (PERIOD / 2))
        {
            rx_reset_time_adjustment = timer_discrepancy;
        }
        // The neighbor is trailing behind: move the reset time forward
        // (reset later).
        else
        {
            rx_reset_time_adjustment = (PERIOD + timer_discrepancy) % PERIOD;
        }
    }

    // Combine the reset time adjustment due to this message with the overall
    // reset time adjustment.
    reset_time_adjustment = reset_time_adjustment + rx_reset_time_adjustment;
}

int main()
{
    kilo_init();
    kilo_message_rx = message_rx;
    kilo_message_tx = message_tx;
    kilo_start(setup, loop);
    
    return 0;
}
