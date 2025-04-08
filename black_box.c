#include "black_box.h"
#include "external_eeprom.h"
#include "matrix_keypad.h"
#include "uart.h"

unsigned int event_flag = 0; 
unsigned int adc_value = 0; //to store the speed of a car
unsigned char clear_screen_once=1;
static unsigned int event_count = 0; //to store the number of events
unsigned int selected_option = 0;
unsigned char read_flag_once = 0;
unsigned char read_events[10][15]; //to store events
extern unsigned char key; 
unsigned char clear_events_flag = 0;
unsigned int store_address = 0; //address to write the events
unsigned int read_address = 0; //address to read the events

unsigned char event_on_save_flag = 1;
void view_dashboard() {
static unsigned char last_event = 0;
    unsigned char a = 0, b = 0; //for speed 
    static unsigned char power_on_flag = 0; 

    adc_value = read_adc(CHANNEL4);
    adc_value = adc_value / 10.23;
    clcd_print("TIME      EV  SP", LINE1(0));
    clcd_print(time, LINE2(0));

    if (power_on_flag == 0 && !(key)) {
        clcd_print(events[event_flag], LINE2(10));  //on power on/reset ON state will display
        a = '0', b = '0';
        event_store();
    } else if (key) { //if any events happened based on key press

        if (adc_value < 99) { //using 10bit ADC  
            a = adc_value / 10 + '0';
            b = (adc_value % 10) + '0';
        }

        if (key == SW1) {
            if (event_flag == 8)  //if collision happens then event start from Gear Neutral
            {
                event_flag = 1;
                 event_store();
            }
            else if (event_flag < MAX_EVENTS - 2) {
                event_flag++; //events increment
                event_count++;
                 event_store();
            }
        } else if (key == SW2) {
            if (event_flag == 8) { //event decrement
                event_flag = 1;
                 event_store();
            } else if (event_flag) {
                if (event_flag != 1) {
                    event_flag--;
                    event_count++;
                     event_store();
                }

            }

        } else if (key == SW3) { //to make collision
            event_flag = 8;
        } else if (key == SW11) { //to enter in to menu's
            state = e_main_menu;
            power_on_flag=1; 
//            CLEAR_DISP_SCREEN;
        }
        clear_events_flag=0;
        read_flag_once = 0;
        clcd_print(events[event_flag], LINE2(10));
    }
    clcd_putch(a, LINE2(14));
    clcd_putch(b, LINE2(15));
   
    
    
//     if(event_on_save_flag == 1)
//    {
//        event_store();
//        last_event = event_flag;
//       //read_flag_once = 1;
//        event_on_save_flag = 0;
//    }
//    if (event_flag!= last_event)
//    {
//        event_store();
//        last_event = event_flag;
//       //read_flag_once= 1;
//    }
}

void event_store() { 
store_address = store_address % 120; 
//using only first 120 bytes of memory out of  512bytes of memory every time re-writing the first byte after 10 events
    for (unsigned char i = 0; i < 8; i++) {
        write_external_eeprom(store_address++, time[i]);
    }
    for (unsigned char i = 0; i < 2; i++) {
        write_external_eeprom(store_address++, events[event_flag][i]);
    }

    write_external_eeprom(store_address++, (adc_value / 10) + '0');
    write_external_eeprom(store_address++, (adc_value % 10) + '0');
}


void display_main_menu(void) 
{
    unsigned char menu[4][15] = {"view log      ", "clear log     ", "download log  ", "set time       "};
    static unsigned char current_pos = 0;
    static unsigned char star_pos = 0;
    
    if(clear_screen_once == 1)            // clear main menu one time
    {
        clcd_print("                ", LINE1(0));
        clcd_print("                ", LINE2(0));
        clear_screen_once = 0;
    }
    
    //Print menu and star position
    clcd_print(menu[current_pos], LINE1(1));
    clcd_print(menu[(current_pos + 1) % 4], LINE2(1));
    if (star_pos == 1) 
    {
        clcd_putch('*', LINE2(0));
        clcd_putch(' ', LINE1(0));
    } else 
    {
        clcd_putch('*', LINE1(0));
        clcd_putch(' ', LINE2(0));
    }
    
    if (key == SW12) 
    {
        state = e_dashboard;
        clear_screen_once = 1;
        CLEAR_DISP_SCREEN;
    }
    
    if (key == SW2) //scroll up
    {
        if ((star_pos == 0) && (key == SW11))
            view_log();
        if ((current_pos >= 0) && (current_pos < 2) && (star_pos == 1))
            current_pos++;
        star_pos = 1;
       // clear_events_flag=0;
    }
    if (key == SW1) //scroll down
    {
        if ((current_pos < 4) && (current_pos > 0) && (star_pos == 0))
            current_pos--;
        
        star_pos = 0;
        //clear_events_flag=0;
    }
    if (key == SW11) 
    {
        if (star_pos == 0) 
        {
            if (current_pos == 0) 
            {
              clear_screen_once = 1;
              //  event_reader_flag = 1;
                state = e_view_log;
            }
            if (current_pos == 1) 
            {
                clear_screen_once = 1;
                state = e_clear_log;
            }
            if (current_pos == 2)
            {
                clear_screen_once = 1;
                state = e_download_log;
            }
        }
        else if (star_pos == 1) 
        {
            if (current_pos == 0) 
            {
                clear_screen_once = 1;
                state = e_clear_log;
            }
            if (current_pos == 1) 
            {
                clear_screen_once = 1;
                state = e_download_log;
            }
            if (current_pos == 2) 
            {
                clear_screen_once = 1;
                state = e_set_time;
            }
        }
    }
}




void view_log() {
  if(clear_screen_once==1)
  {
    if (clear_events_flag == 0) {//if we have events only it will enter in this
    
        static unsigned int start_index = 0;
        static unsigned int select = 0;
        if (read_flag_once == 0) {
            read_event();
            read_flag_once = 1;
            if (event_count > 10) {
                start_index = event_count - 10;
                select = start_index;
            }
        }
        if (key == SW12) {
            state = e_main_menu;
            CLEAR_DISP_SCREEN;
            return;
        }

        clcd_print("i   TIME   EV SP", LINE1(0));

        if (key == SW2)
        {
            if(select<event_count-1)
                select++;
        }
        else if (key == SW1)
        {
            if(select)
                select--;
        }
        clcd_putch(select + 48, LINE2(0));
        clcd_putch(' ', LINE2(1));
        clcd_print(read_events[select], LINE2(2));
         // clear_screen_once=0;
    }
    else {
        for (unsigned int i = 0; i < 10; i++) {
            clcd_print("    NO EVENTS   ", LINE1(0));
            clcd_print("    TO VIEW     ", LINE2(0));
            __delay_ms(500);
        }
        state=e_main_menu;//after printing no events directly goes to main_menu 
        return ;
    }
  
  }
    if(key==SW12)
    {
        state=e_main_menu;
    }
   // CLEAR_DISP_SCREEN;
}

void read_event() {
    // unsigned int add = 0;
    unsigned int i;
    for (i = 0; i < event_count; i++) {
        for (unsigned int j = 0; j < 15; j++) {
            if (j == 8 || j == 11)
                read_events[i][j] = ' ';
            else if (j == 14)
                read_events[i][j] = '\0';
            else
            {
                read_events[i][j] = read_external_eeprom(read_address++);
                 read_address = read_address % 120; 
            }
        }
       
    }
}

void init_config_uart(void) {
    init_uart();
    /* Enabling peripheral interrupt */
    PEIE = 1;
    /* Enabling global interrupt */
    GIE = 1;
}

void download_log() { 
    //displaying the events in tera term  using UART protocol 
    if(clear_screen_once == 1)
    {
    //CLEAR_DISP_SCREEN;
    if (read_flag_once == 0) {
            read_event();
            read_flag_once = 1;
    }
    if (clear_events_flag == 0) {
       
        puts("   TIME EV SP");
        puts("\n\r");
        for (unsigned int i = 0; i < event_count; i++) {
            puts(read_events[i]);
            puts("\n\r");
        }
        for (unsigned int i = 0; i < 10; i++) {
            clcd_print("  DOWNLOADING   ", LINE1(0));
            clcd_print("   EVENTS ...   ", LINE2(0));
            __delay_ms(500);
        }

    } else { //no events to download when event_count=0
        for (unsigned int i = 0; i < 10; i++) {
            clcd_print("DOWNLOAD FAILED ", LINE1(0));
            clcd_print("NO EVENT PRESENT", LINE2(0));
            __delay_ms(500);
        }
          puts("   No Events to download ");
        puts("\n\r");
    }
    
    clear_screen_once=0;
    }
    if(key==SW12)
    {
        clear_screen_once=1;
         state = e_main_menu;
    }
//    CLEAR_DISP_SCREEN;
//    state=e_main_menu;
//    return;
}

void clear_log(void) {
    //clearing the all events in the sense making all the count variables to 0. 
    if(clear_screen_once==1)
    {
    if (clear_events_flag == 0) {
        event_count = 0;
        clear_events_flag = 1;
        event_flag = 0;
        for (unsigned int i = 0; i < 10; i++) {
            clcd_print("    CLEARING    ", LINE1(0));
            clcd_print("   EVENTS ...   ", LINE2(0));
            __delay_ms(500);
        }

    } else {
        for (unsigned int i = 0; i < 10; i++) {
            clcd_print("  ALREADY   ALL ", LINE1(0));
            clcd_print(" EVENTS  CLEARED", LINE2(0));
            __delay_ms(500);
        }
    }
    clear_screen_once=0;
    }
//    if(key==SW12)
//    {
        state=e_main_menu;
//    }
    
     //CLEAR_DISP_SCREEN;
}
    

void set_time(void) {
    //to edit the time 
    CLEAR_DISP_SCREEN;
    clcd_print("    HH:MM:SS    ", LINE1(0));

    static int set_field = 1;
    static int hrs, mins, sec;
    static int blink = 0XFF; //1
    static int flag_blink = 0;
    static int delay_blink = 0;
    static int change_field;
    if (set_field == 1) {
        set_field = 0;
        hrs = ((time[0] - 48)*10) + (time[1] - 48); //converting to char to int and storing in each separate field
        mins = ((time[3] - 48)*10) + (time[4] - 48);
        sec = ((time[6] - 48)*10) + (time[7] - 48);
    }
//    key = check_matrix_keypad();
    if (delay_blink++ > 500) { //non blocking delay for blinking the particular field 
        delay_blink = 0;
        flag_blink = !flag_blink;
    }
    //to change the field to hrs,mins and secs
    if (key == SW2) {
        if (change_field >= 0 && change_field < 2) {
            change_field++;
        } else if (change_field >= 2) {
            change_field = 0;
        }
    } else if (key == SW1) { //for incremental purpose 
        if (change_field == 0) {
            if (hrs >= 0 && hrs < 23)
                hrs++;
            else
                hrs = 0;
        } else if (change_field == 1) {
            if (mins >= 0 && mins < 59)
                mins++;
            else
                mins = 0;
        } else if (change_field == 2) {
            if (sec >= 0 && sec < 59)
                sec++;
            else
                sec = 0;
        }
    }
    clcd_print("  ", LINE2(0));
    clcd_putch((hrs / 10) + '0', LINE2(4));
    clcd_putch((hrs % 10) + '0', LINE2(5));
    clcd_putch(':', LINE2(6));

    clcd_putch((mins / 10) + '0', LINE2(7));
    clcd_putch((mins % 10) + '0', LINE2(8));
    clcd_putch(':', LINE2(9));


    clcd_putch((sec / 10) + '0', LINE2(10));
    clcd_putch((sec % 10) + '0', LINE2(11));
    clcd_putch(' ', LINE2(12));


    if (change_field == 0) {
        if (flag_blink) {
            clcd_putch(blink, LINE2(4));
            clcd_putch(blink, LINE2(5));
        }
    } else if (change_field == 1) {
        if (flag_blink) {
            clcd_putch(blink, LINE2(7));
            clcd_putch(blink, LINE2(8));
        }
    } else if (change_field == 2) {
        if (flag_blink) {
            clcd_putch(blink, LINE2(10));
            clcd_putch(blink, LINE2(11));
        }
    }

    if (key == SW11) { //writing to RTC by using i2c protocol
        write_ds1307(HOUR_ADDR, (hrs / 10) << 4 | (hrs % 10));
        write_ds1307(MIN_ADDR, (mins / 10) << 4 | (mins % 10));
        write_ds1307(SEC_ADDR, (sec / 10) << 4 | (sec % 10));
        change_field = 0;
        CLEAR_DISP_SCREEN;
        state = e_dashboard;
    }
    if (key == SW12) { //to come out of set_time to main_menu
        state = e_dashboard;
        change_field = 0;
        CLEAR_DISP_SCREEN;
    }
}