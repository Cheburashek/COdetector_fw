/*
 * main.c
 *
 * Created: 2015-10-02 20:50:49
 *  Author: Chebu
 */ 

// Notes for txt:
/*

- 20ms sampling - AC line glitch rejection

*/

// Problems to opowiedziec:
/*

- adc auto offset calibration - temporary manual

*/



#include "common.h"



#define DIVIDER         1000     // Input voltage divider ( 1:1 -> 1000 )



volatile uint16_t measVal = 0;    // Measured voltage on detector output

static void endOfMeas ( uint16_t val );



//****************************************************************************************
int main(void)
{     
   
   boardInit();                        // Board peripherals initialization       
   adcRegisterEndCb( endOfMeas );      // Registering CB

   pdcLine( "Heszek!       ", 2 );

   while(1){
      
      for ( uint32_t i = 0; i < 1000; i++ ){}         
      
      adcStartChToGnd();
      
   }

}

//****************************************************************************************
static void endOfMeas ( uint16_t val )
{

   measVal = ((((uint32_t)val)*DIVIDER)/65535);   // For 16b res

   LOG_UINT ( "Result [mV]:  ", 14, (uint16_t) measVal );
   pdcUint ( measVal, 0, 0, 1 );
}   
   
    


