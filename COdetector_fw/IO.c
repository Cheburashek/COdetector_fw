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
   ioStatLedOff();
   ioBuzzerOff();
   
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
// Driving buzzer ( not inline or macro because of using as a callback ):
void ioBuzzerOn ( void )
{
   PORTD.OUTSET =  CFG_BUZZ_PIN_MASK;
}

void ioBuzzerOff ( void )
{
   PORTD.OUTCLR =  CFG_BUZZ_PIN_MASK; 
}

void ioBuzzerTgl ( void )
{
   PORTD.OUTTGL =  CFG_BUZZ_PIN_MASK;
}

//****************************************************************************************
// Driving status LED ( not inline or macro because of using as a callback ):
void ioStatLedOn ( void )
{
   PORTA.OUTCLR =  CFG_LED_PIN_MASK;  
}

void ioStatLedOff ( void )
{
   PORTA.OUTSET =  CFG_LED_PIN_MASK;  
}

void ioStatLedTgl ( void )
{
   PORTA.OUTTGL =  CFG_LED_PIN_MASK;
}