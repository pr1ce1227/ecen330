#include "gpioTest.h"
#include "leds.h"
#include "switches.h"
#include "buttons.h"
#include "display.h"

void gpioTest_buttons(){
    display_init(); 
    buttons_init(); 
    while(1){
        if(buttons_read() == 0x0F){
            break;
        }
        else {
            switches_read();
        }
    }

}

void gpioTest_switches(){
    leds_init();
    switches_init();
    while(1){
        
        if(leds_read() == 0x0F){
            break; 
        }
        else {
            leds_write(switches_read()); 
        }
    } 

}