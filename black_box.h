
#ifndef BLACK_BOX_H
#define BLACK_BOX_H

#include <xc.h>
#include "matrix_keypad.h"
#include "clcd.h"
#include "adc.h"
#include "eeprom.h"
#include "i2c.h"
#include "ds1307.h"
#include "uart.h"
#include "i2c.h"

#define  SW1 1 
#define  SW2 2
#define SW3 3 
#define SW11 11
#define SW12 12 
#define MAX_EVENTS 9





/* Enum for maintaining the app state */
typedef enum {
    e_dashboard, e_main_menu, e_view_log, e_set_time, e_download_log, e_clear_log
} State_t;




extern State_t state; // App state

unsigned char events[9][3]={"ON","GN","G1","G2","G3","G4","G5","GR","C_"};
unsigned char time[9]= "00:00:00";




//Function declarations

//Dashboard function declaration
void view_dashboard();
unsigned char check_matrix_keypad();
//Storing events function declaration
void event_store(void);
void read_event();
//Password function declaration
void password(void);
void select_function();
//main menu function declaration
void display_main_menu() ;

//View log function declaration
void view_log();

//Reading events function declaration
void event_reader(void);

//Change password function declaration
void change_password(void);

//t time function declaration
void set_time(void);
void get_time(void);
//Download log function _declaration
void download_log(void);

//Clear log function declaration
void clear_log(void);
//void set_time(void);
void display_time(void);


#endif 