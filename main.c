/*Title : Car black Box
 * File:   main.c
 * Author: ABHINAY
 
 *Introduction : To implement a simplified car black box system that logs real-time vehicle events like 
  ignition on/off, key inputs, and analog sensor values, and stores them in EEPROM for future
   download or review.
 
 *Key Features:
Logs up to 10 recent vehicle events in a ring buffer.
Time-stamps each event.
Stores logs in external EEPROM.
Displays current status/events on CLCD.
Downloads all logs for review via display.
Allows clearing the log memory.
Persistent storage across power cycles.
 
 Applications of project : 
 * Accident Investigation
 *  Insurance Claims
 * Driver Behavior Monitoring
 * Emergency Response

 * Created on 18 March, 2025, 2:59 PM
 * Submission Date : 08 April ,2025 
 */


#include "black_box.h"
#include "i2c.h"


State_t state;
unsigned int adc_value = 0;
 unsigned char key;
unsigned char check_matrix_keypad(void) {
    return read_switches(STATE_CHANGE);
}
 

void init_config() {
    init_matrix_keypad();
    init_clcd();
    init_config_uart();
    init_adc();
    init_i2c();
    init_ds1307();
    state = e_dashboard;
}

void main(void) {
    init_config();
    

    while (1) {
                key = check_matrix_keypad();
        get_time();
        //clcd_print("hello",LINE1(0));
        
        

        adc_value = read_adc(CHANNEL4);

        switch (state) {

            case e_dashboard:
                view_dashboard();
                
                break;
                
            case e_main_menu:
                display_main_menu();
                break;

            case e_view_log:
                view_log();
                //state=e_main_menu;
                break;

            case e_download_log:
                download_log();
               
                break;

            case e_clear_log:
                clear_log();
               
                break;

            case e_set_time:
                set_time();
              
                break;
        }

    }

}

