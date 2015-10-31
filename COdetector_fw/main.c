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


#include "common.h"

#define DIVIDER         1000     // Input voltage divider ( 1:1 -> 1000 )



volatile uint16_t measVal = 0;    // Measured voltage on detector output

static void endOfMeas ( uint16_t val );



//****************************************************************************************
int main(void)
{     
   
   boardInit();                        // Board peripherals initialization       
   adcRegisterEndCb( endOfMeas );      // Registering CB

   while(1)
   {

     for ( uint32_t i = 0; i < 100000; i++ ){}   
              
      adcStartChToGnd();
      
   }

}

//****************************************************************************************
static void endOfMeas ( uint16_t val )
{

  // measVal = ((((uint32_t)val)*DIVIDER)/65535);   // For 16b res


}   
   
    


