/*
 * IO.c
 *
 * Created: 2015-12-04 01:29:09
 *  Author: Chebu
 */ 


//TODO: unused pins


/*****************************************************************************************
   LOCAL INCLUDES
*/

#include "IO.h"
#include "common.h"

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

//****************************************************************************************
// IO initialization:
void ioInit ( void )
{
   // Initial stats:
   ioStatLED(false);
   ioBuzzer(false);
   
   // Outputs:
   PORTD.DIRSET = CFG_BUZZ_PIN_MASK;
   
   PORTA.DIRSET = CFG_LED_PIN_MASK |
                  CFG_BCKLGHT_PIN_MASK;           
                  
                  
   // Inputs:
   PORTD.DIRCLR = CFG_BT1_PIN_MASK |
                  CFG_BT2_PIN_MASK |
                  CFG_BT3_PIN_MASK |
                  CFG_USB_CON_PIN_MASK;
                  
   // Pull's:
   PORTD.PIN5CTRL = PORT_OPC_PULLUP_gc;   // Button 3
   PORTD.PIN6CTRL = PORT_OPC_PULLUP_gc;   // Button 2
   PORTD.PIN7CTRL = PORT_OPC_PULLUP_gc;   // Button 1   
   PORTD.PIN4CTRL = PORT_OPC_PULLDOWN_gc; // USB connected
   
   // Interrupts:
   PORTD.PIN5CTRL = PORT_ISC_FALLING_gc;   // Button 3
   PORTD.PIN6CTRL = PORT_ISC_FALLING_gc;   // Button 2
   PORTD.PIN7CTRL = PORT_ISC_FALLING_gc;   // Button 1
   PORTD.PIN4CTRL = PORT_ISC_LEVEL_gc;     // USB connected -> Low input level for interrupt
   
   LOG_TXT ( ">>init<<   IO initialized\n" );
}


//****************************************************************************************
// Driving buzzer:
void ioBuzzer ( bool stat )
{
   if ( stat )
   {
      PORTD.OUTSET =  CFG_BUZZ_PIN_MASK;  
   }
   else
   {
      PORTD.OUTCLR =  CFG_BUZZ_PIN_MASK; 
   }   
}

//****************************************************************************************
// Driving status LED (Hi-off, Low-on):
void ioStatLED ( bool stat )
{
   if ( stat )
   {
      PORTA.OUTCLR =  CFG_LED_PIN_MASK;  
   }
   else
   {
      PORTA.OUTSET =  CFG_LED_PIN_MASK; 
   }   
}