/*
 * main.c
 *
 * Created: 2015-10-02 20:50:49
 *  Author: Chebu
 */ 


#include "common.h"


#define NUMBER_OF_MEAS  5      // Number of measurements for additional averaging
#define DIVIDER         1000     // Input voltage divider ( 1:1 -> 1000 )

volatile uint16_t detVal = 0;    // Measured voltage on detector output

static void endOfMeas ( void );






//****************************************************************************************
int main(void)
{     
   PORTA.DIRSET = CFG_PULSE_PIN_MASK;  // Pulse pin as output
   PULSE_CLR();                        // Cleared - inverted logic
   boardInit();                        // Board peripherals initialization 
     
   adcRegisterEndCb( endOfMeas );      // Registering CB
   timerSHARP();                       // Starting measuring loop
   
   while(1);
}









//****************************************************************************************
static void endOfMeas ( void )
{
   
   //LOG_UINT ( "", 0, ADCA.CH0RES );
   static uint8_t measNum;     // Number of sample
   static uint32_t measSum;    // For averaging
   
   measSum += ADCA.CH0RES;
   measNum ++;
   
   if ( NUMBER_OF_MEAS == measNum )
   {
      measSum = (((measSum / NUMBER_OF_MEAS)*DIVIDER)/65535);   // For 16b res
     

      LOG_UINT ( "", 0, measSum );      
      LOG_BIN ( "", 0, measSum, 16 ); 
      
      DEB_1_TGL();
      measSum = 0;
      measNum = 0;
      
   }   
   

}

