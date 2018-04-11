#include <kilolib.h>

#define STOP 0
#define FORWARD 1
#define LEFT 2
#define RIGHT 3
#define LEADER_ID 1

int current_motion = LEFT;
int previous_motion = LEFT;
uint32_t reset_time = 0;
uint32_t last_reset = 0;
int both_spined = 0;
int turned = 0;

int received_id = 0;
int received_ticks = 0;
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

void turn(){
    if (previous_motion == RIGHT)
    {
        set_color(RGB(1, 0, 0));
        set_motion(LEFT);
    }
    else if (previous_motion == LEFT)
    {
        set_color(RGB(0, 0, 1));
        set_motion(RIGHT);
    }
    /*else{

        int random_number = rand_hard();
        int random_direction = (random_number % 2);

        if (random_direction == 0)
        {
            set_color(RGB(1, 0, 0));
            set_motion(LEFT);
        }
        else if (random_direction == 1)
        {
            set_color(RGB(0, 0, 1));
            set_motion(RIGHT);
        }
    }*/
}

void setup()
{
    reset_time = kilo_ticks%64;
    last_reset = kilo_ticks;
    previous_motion = LEFT;
    message.type = NORMAL;
    message.data[0] = kilo_uid;
    message.data[1] = reset_time;
    message.crc = message_crc(&message);
}

void loop()
{
        if (kilo_uid != LEADER_ID){
            if(new_message == 1){
                new_message = 0;
                if (received_id == LEADER_ID){
                    reset_time = received_ticks;
                }
            }

            //message.type = NORMAL;
            //message.data[0] = kilo_uid;
            //message.data[1] = received_ticks;
            //message.crc = message_crc(&message);
            reset_time += (kilo_ticks-last_reset);
            reset_time %= 64;
            //if (received_id == LEADER_ID){

            if (reset_time >= 30 && reset_time < 34){

                turn();
                //turned = 1;
            }
            else{
                //turned = 0;
                previous_motion = current_motion;
            }
            last_reset = kilo_ticks;
            //}
        }
        else{
            reset_time = kilo_ticks%64;

            if(kilo_ticks>last_reset+64){
                last_reset = kilo_ticks;
                message.type = NORMAL;
                message.data[0] = kilo_uid;
                message.data[1] = reset_time;
                message.crc = message_crc(&message);

            }
            if (reset_time >= 30 && reset_time < 34){

                turn();
                //turned = 1;
            }
            else{
                //turned = 0;
                previous_motion = current_motion;
            }
        }
    
    /*else{
        set_color(RGB(0, 0, 0));
        set_motion(STOP);

    }*/
    /*
    // Set the LED green.
    set_color(RGB(0, 1, 0));
    // Spinup the motors to overcome friction.
    spinup_motors();
    // Move straight for 2 seconds (2000 ms).
    set_motors(kilo_straight_left, kilo_straight_right);
    delay(2000);
    
    // Set the LED red.
    set_color(RGB(1, 0, 0));
    // Spinup the motors to overcome friction.
    spinup_motors();
    // Turn left for 2 seconds (2000 ms).
    set_motors(kilo_turn_left, 0);
    delay(2000);
    
    // Set the LED green.
    set_color(RGB(0, 1, 0));
    // Spinup the motors to overcome friction.
    spinup_motors();
    // Move straight for 2 seconds (2000 ms).
    set_motors(kilo_straight_left, kilo_straight_right);
    delay(2000);
    
    // Set the LED blue.
    set_color(RGB(0, 0, 1));
    // Spinup the motors to overcome friction.
    spinup_motors();
    // Turn right for 2 seconds (2000 ms).
    set_motors(0, kilo_turn_right);
    delay(2000);
    
    // Set the LED off.
    set_color(RGB(0, 0, 0));
    // Stop for half a second (500 ms).
    set_motors(0, 0);
    delay(500);*/
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
        received_id = m->data[0];
        received_ticks = m->data[1];
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
