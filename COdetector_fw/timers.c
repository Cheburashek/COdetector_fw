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

   TCC4.CTRLA = 0x02;   // Div clk by 4 for 1MHz
   TCC4.INTCTRLB = CFG_PRIO_TC4_CCxLVL;   // int prio
   
   
}

// TODO: channel as enum
void timerSingleUs( uint16_t us )
{
   TCC4.CCABUF = us;
   TCC4.CTRLGSET = TC45_CMD_RESTART_gc;

   
}

ISR ( TCC4_CCA_vect )
{

   timerSingleUs (65000);
   DEB_2_TGL();
   
}