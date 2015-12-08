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

#include "ADC.h"
//****************************************************************************************
int main(void)
{     
   
   boardInit();                        // Board peripherals initialization      
    
ADCA.CH0.MUXCTRL = CFG_ADC_MUXPOS; 
   while(1)
   {
      ADCA.CTRLA |= ADC_START_bm;
      _delay_ms(1500);
      
   }

}


   
    

