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
   // Initial states:
   ioStatLedOff();
   ioBuzzerOff();
   
   // UNUSED PINS (input and pullup):
      // PORTA:
      PORTA.DIRCLR = PIN2_bm | PIN3_bm | PIN5_bm | PIN6_bm;
      PORTA.PIN2CTRL = PORT_OPC_PULLUP_gc;
      PORTA.PIN3CTRL = PORT_OPC_PULLUP_gc;
      PORTA.PIN5CTRL = PORT_OPC_PULLUP_gc;
      PORTA.PIN6CTRL = PORT_OPC_PULLUP_gc;
   
      // PORTC:
      PORTC.DIRCLR = PIN0_bm | PIN2_bm | PIN4_bm | PIN6_bm;
      PORTC.PIN0CTRL = PORT_OPC_PULLUP_gc;
      PORTC.PIN2CTRL = PORT_OPC_PULLUP_gc;
      PORTC.PIN4CTRL = PORT_OPC_PULLUP_gc;
      PORTC.PIN6CTRL = PORT_OPC_PULLUP_gc;
   
      // PORTD:
      PORTD.DIRCLR = PIN0_bm;
      PORTD.PIN0CTRL = PORT_OPC_PULLUP_gc;
   //
  
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

void ioBuzzShortBeep ( void )
{
   ioBuzzerOn();
   _delay_us ( 700 );
   ioBuzzerOff();
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

void ioStateLedShortTick ( void )
{
   ioStatLedOn();          // Turning on status LED (blink driven by ADC measuring time)
      _delay_us ( 1400 );
   ioStatLedOff();
}

//****************************************************************************************
// ISR:
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