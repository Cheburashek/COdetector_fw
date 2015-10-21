/*
 * timers.c
 *
 * Created: 2015-10-11 02:49:22
 *  Author: Chebu
 */ 


// only for dr

/*****************************************************************************************
   LOCAL INCLUDES
*/
#include "timers.h"

/*****************************************************************************************
   MACROS
*/

/*****************************************************************************************
   LOCAL DEFINITIONS
*/

// Max 15ms !

#define CYCLE_PERIOD_US     10000
#define MEAS_DELAY_US       (CYCLE_PERIOD_US + 280)
#define PULSE_TIME_US       (CYCLE_PERIOD_US + 320)


/*****************************************************************************************
   LOCAL VARIABLES
*/

/*****************************************************************************************
   LOCAL FUNCTIONS DECLARATIONS
*/

/*****************************************************************************************
   LOCAL FUNCTIONS DEFINITIONS
*/


/*****************************************************************************************
   GLOBAL FUNCTIONS DEFINITIONS
*/


void timerInit ( void )
{

   TCC4.CTRLA = 0x04;   // Div clk by 8 for 4MHz @ 32MHz
   
   TCC4.INTCTRLB = CFG_PRIO_TC4_CCALVL |
                   CFG_PRIO_TC4_CCBLVL |
                   CFG_PRIO_TC4_CCCLVL;  
                   
   TCC4.CTRLGSET = TC4_STOP_bm;  // STOP  

   //TCC4.CTRLGSET = TC45_CMD_RESTART_gc;
}

// TODO: channel as enum
void timerSingleUs( uint16_t us )
{
   TCC4.CCABUF = us;   

   TCC4.CTRLGSET = TC45_CMD_RESTART_gc;
   
}



void timerSHARP ( void )
{
   // Measuring init:
      
   TCC4.CCABUF = (uint16_t)CYCLE_PERIOD_US*4;
   TCC4.CCBBUF = (uint16_t)MEAS_DELAY_US*4;
   TCC4.CCCBUF = (uint16_t)PULSE_TIME_US*4;
      
   TCC4.CTRLGCLR = TC4_STOP_bm;     // START
   TCC4.CTRLGSET = TC45_CMD_RESTART_gc;
}


ISR ( TCC4_CCA_vect )
{
    
}


ISR ( TCC4_CCB_vect )
{   
   DEB_3_SET();         // only for test
   DEB_2_SET();         // only for test
   adcStartChToGnd();   // ADC start;   
}



ISR ( TCC4_CCC_vect )
{
   DEB_2_CLR();         // only for test
    
   
   TCC4.CTRLGSET = TC45_CMD_RESTART_gc;   // Restarting timer - continous
}