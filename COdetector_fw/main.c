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

   PORTA_DIRSET = PIN7_bm;
   PORTA_DIRSET = PIN6_bm;


   PORTA_OUTCLR = PIN6_bm; // bcklght

   while(1)
   {
      for ( uint32_t i = 0; i < 160000; i++ ){}
      PORTA_OUTTGL = PIN7_bm;
      
      //LOG_TXT ("Dupa 1000", 9 ); // TODO: StrLen    


   }

}


   
    


