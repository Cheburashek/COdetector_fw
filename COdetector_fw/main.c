/*
 * main.c
 *
 * Created: 2015-10-02 20:50:49
 *  Author: Chebu
 */ 

// Notes for txt:
/*

- 20ms sampling - AC line glitch rejection
- for initialization - fast 2MHz, after - 32kHz ( e.g. fast cleaning LCD RAM )
*/

// Problems to include in txt:
/*

- adc auto offset calibration - temporary manual

*/



// TODO: clk in lopo
// TODO: add watchdog

// TODO: add temperature measurement 

// TODO: disable logs when no usb connected

// TODO: przytrzymanie przycisku

#include "common.h"


//****************************************************************************************
int main(void)
{     
   
   boardInit();                        // Board peripherals initialization       
  

   while(1)
   {
   }

}


   
    

