/*
 * main.c
 *
 * Created: 2015-10-02 20:50:49
 *  Author: Chebu
 */ 


#include "common.h"


#define NUMBER_OF_MEAS  25       // Number of measurements for additional averaging
#define DIVIDER         1000     // Input voltage divider ( 1:1 -> 1000 )

volatile uint16_t detVal = 0;    // Measured voltage on detector output

static void endOfMeas ( uint16_t val );



//****************************************************************************************
int main(void)
{     
   
   PORTA.DIRSET = CFG_PULSE_PIN_MASK;  // Pulse pin as output
   boardInit();                        // Board peripherals initialization 
      
   adcRegisterEndCb( endOfMeas );      // Registering CB
   timerSHARP();                       // Starting measuring loop
   
   while(1){;}

}

//****************************************************************************************
static void endOfMeas ( uint16_t val )
{
   
   
   static uint8_t measNum;     // Number of sample
   static uint32_t measSum;    // For averaging
   
   measSum += val;
   measNum ++;
   
   if ( NUMBER_OF_MEAS == measNum )
   {
      detVal = (((measSum / NUMBER_OF_MEAS)*DIVIDER)/65535);   // For 16b res
      measSum = 0;
      measNum = 0;
      LOG_UINT ( "Result [mV]:  ", 14, (uint16_t) detVal );
   }   
   
    
}

