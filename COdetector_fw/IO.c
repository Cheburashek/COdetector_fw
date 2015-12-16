/*
 * IO.c
 *
 * Created: 2015-12-04 01:29:09
 *  Author: Chebu
 */ 


//TODO: unused pins
//TODO: PORT as a define


/*****************************************************************************************
   LOCAL INCLUDES
*/

#include "IO.h"
#include "interFace.h"


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
   ioBcklghtOff();
   ioBuzzerOff();
   
   // UNUSED PINS (input and pullup): //TODO: add the others
  
   // Outputs:
   PORTD.DIRSET = CFG_BUZZ_PIN_MASK;
   
   PORTA.DIRSET = CFG_LED_PIN_MASK |
                  CFG_BCKLGHT_PIN_MASK;
   
   // Inputs:
   PORTD.DIRCLR = CFG_BT1_PIN_MASK |
                  CFG_BT2_PIN_MASK |
                  CFG_BT3_PIN_MASK |
                  CFG_USB_CON_PIN_MASK;
                  
   // Interrupts:
   PORTD.PIN5CTRL |= PORT_ISC_FALLING_gc;     // Button 3
   PORTD.PIN6CTRL |= PORT_ISC_FALLING_gc;     // Button 2
   PORTD.PIN7CTRL |= PORT_ISC_FALLING_gc;     // Button 1
   
   if ( IO_GET_USB_CONN() )   // If USB plugged in
   {
      IO_FALLING_EDGE_USB();
   }
   else
   {
      IO_RISING_EDGE_USB();
   }
    
   // Pull's:
   PORTD.PIN5CTRL |= PORT_OPC_PULLUP_gc;      // Button 3
   PORTD.PIN6CTRL |= PORT_OPC_PULLUP_gc;      // Button 2
   PORTD.PIN7CTRL |= PORT_OPC_PULLUP_gc;      // Button 1   
   PORTD.PIN4CTRL |= PORT_OPC_PULLDOWN_gc;    // USB connected
   
   PORTD.INTFLAGS = 0xFF;                     // Clear all pins flags
   
   PORTD.INTCTRL = CFG_PRIO_PORTD;            // Global PORTD int priority
   
   PORTD.INTMASK =  CFG_BT1_PIN_MASK |
                    CFG_BT2_PIN_MASK |
                    CFG_BT3_PIN_MASK |               
                    CFG_USB_CON_PIN_MASK;
        
}


//****************************************************************************************
// Driving buzzer ( not inline or macro because of using as a callback ):
void ioBuzzerOn ( void )
{
   #ifdef BUZZER_PERM
   PORTD.OUTSET =  CFG_BUZZ_PIN_MASK;
   #endif
}

void ioBuzzerOff ( void )
{
   #ifdef BUZZER_PERM   
   PORTD.OUTCLR =  CFG_BUZZ_PIN_MASK; 
   #endif
}

void ioBuzzerTgl ( void )
{
   #ifdef BUZZER_PERM
   PORTD.OUTTGL =  CFG_BUZZ_PIN_MASK;
   #endif
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


//****************************************************************************************
// Driving backlight LED ( not inline or macro because of using as a callback ):
void ioBcklghtOn ( void )
{
   PORTA.OUTCLR = CFG_BCKLGHT_PIN_MASK;
}

void ioBcklghtOff ( void )
{
   PORTA.OUTSET = CFG_BCKLGHT_PIN_MASK;
}


//****************************************************************************************
//  ISRs:

ISR ( PORTD_INT_vect )
{
   // BT 1
   if ( CFG_BT1_PIN_MASK == PORTD.INTFLAGS )   
   {
      interOnRight();
   }
   // BT 2
   if ( CFG_BT2_PIN_MASK == PORTD.INTFLAGS )
   {
      interOnOk();
   }
   // BT 3  
   if ( CFG_BT3_PIN_MASK == PORTD.INTFLAGS )
   {
      interOnLeft();
   }
   
   // USB connected
   if ( CFG_USB_CON_PIN_MASK == PORTD.INTFLAGS )
   {
       systemUSBStateChanged ();
   }
   
   PORTD.INTFLAGS = 0xFF;                  // Clear all pins flags
}