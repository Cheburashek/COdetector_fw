/*
 * boardCfg.c
 *
 * Created: 2015-10-02 21:57:02
 *  Author: Chebu
 */ 

/*****************************************************************************************
   LOCAL INCLUDES
*/

#include "boardCfg.h"
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

void boardInit ( void )
{
   // Debug LEDs:
   PORTD.DIRSET = PIN5_bm;
   PORTD.DIRSET = PIN6_bm;
   PORTD.DIRSET = PIN7_bm;   

   OSC.CTRL |= OSC_RC32KEN_bm;               // Enabling 32kHz clock
   while (!(OSC.STATUS & OSC_RC32KRDY_bm));  // Waiting for clock ready
   OSC.CTRL = OSC_RC32KEN_bm;                // Disabling other clocks
   CCP=CCP_IOREG_gc;                         // Protected register
   CLK.CTRL = CLK_SCLKSEL_RC32K_gc;          // 32khz internal
   
   //LOGGER:
   #ifdef LOG_USARTC0
      serialInitC();
   #endif
   
}