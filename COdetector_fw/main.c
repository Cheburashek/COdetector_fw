/*
 * main.c
 *
 * Created: 2015-10-02 20:50:49
 *  Author: Chebu
 */ 


#include <avr/io.h>
#include "serial.h"
#include "boardCfg.h"


int main(void)
{
  // Cfg, to be moved into boardCfg
  CFG_SET_CLOCK_8MHZ ();
  
  
  
  
   
   
    while(1)
    {       
    }
}


// TODO: Add watchdog
// TODO: Add 32kHz low timer when i ULP mode