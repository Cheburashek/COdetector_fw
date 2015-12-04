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


// TODO: timestamps : RTC
// TODO: clk in lopo

// TODO: add temperature measurement 

#include "common.h"





volatile uint16_t measVal = 0;    // Measured voltage on detector output

static void endOfMeas ( uint16_t val );



//****************************************************************************************
int main(void)
{     
   
   boardInit();                        // Board peripherals initialization       

   while(1)
   {
   }

}


   
    


