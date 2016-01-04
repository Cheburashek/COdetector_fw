/*
 * oneWire.c
 *
 * Created: 2016-01-03 22:09:35
 *  Author: Chebu
 */ 



/*****************************************************************************************
   LOCAL INCLUDES
*/

#include "common.h"

/*****************************************************************************************
   MACROS
*/

#define ONE_WIRE_OUT()       PORTC.DIRSET = CFG_1WIRE_PIN_MASK    // Output
#define ONE_WIRE_IN()        PORTC.DIRCLR = CFG_1WIRE_PIN_MASK    // Input

#define ONE_WIRE_SET()       PORTC.OUTSET = CFG_1WIRE_PIN_MASK    // High state
#define ONE_WIRE_CLR()       PORTC.OUTCLR = CFG_1WIRE_PIN_MASK    // Low state

#define ONE_WIRE_STATE()     PORTC.IN & CFG_1WIRE_PIN_MASK        // Read state
/*****************************************************************************************
   LOCAL DEFINITIONS
*/

/*****************************************************************************************
   LOCAL VARIABLES
*/

/*****************************************************************************************
   LOCAL FUNCTIONS DECLARATIONS
*/

static bool oneWireReset ( void );
static void oneWireSendByte ( uint8_t byte );
static uint8_t oneWireReadByte ( void );

/*****************************************************************************************
   LOCAL FUNCTIONS DEFINITIONS
*/

static bool oneWireReset ( void )
{
   bool retVal = FALSE;
   
   ONE_WIRE_CLR();   // Low state
   ONE_WIRE_OUT();   // Pin as an output
   _delay_us(500);
   ONE_WIRE_IN();    // Pin as an input
   
   if ( !ONE_WIRE_STATE() ) retVal = TRUE;
   
   _delay_us(470);
   
   return retVal;
   
}

//****************************************************************************************
static void oneWireSendByte ( uint8_t byte )
{  
   ENTER_CRIT_ALL();
   
   for ( uint8_t i = 0; i < 8; i++ )
   {
      // Sending single bit:
      ONE_WIRE_OUT();
      _delay_us(5);
      
      if ( byte & 0x01 ) ONE_WIRE_IN();
      _delay_us(80);
      ONE_WIRE_IN();    
      byte >>= 0x01;  
   }  
   
   EXIT_CRITICAL();
   _delay_us(100);
}

//****************************************************************************************
static uint8_t oneWireReadByte ( void )
{  
   uint8_t byte = 0x00;
   
   ENTER_CRIT_ALL();
   
   for ( uint8_t i = 0; i < 8; i++ )
   {
      // Reading single bit:   
      ONE_WIRE_OUT();
      _delay_us(2);
      ONE_WIRE_IN();
      _delay_us(15);
      
      if ( ONE_WIRE_STATE() ) byte |= (0x01<<i);
      _delay_us(15);   
   }      
   
   EXIT_CRITICAL();
   return byte;
}


/*****************************************************************************************
   GLOBAL FUNCTIONS DEFINITIONS
*/


void oneWireConvStart ( void )
{    
   oneWireReset();             // Reset  
   oneWireSendByte ( 0xCC );
   oneWireSendByte ( 0x44 );   // Start conversion
}   


//****************************************************************************************
uint16_t oneWireReadTemp( void )
{
   uint16_t temp = 0x00;  // default: temperature in 12bit resolution with signum on 13th bit
   
   oneWireReset(); 
   oneWireSendByte ( 0xCC );
   oneWireSendByte ( 0xBE );   // Read scratchpad
   
   ENTER_CRIT_ALL();
   temp = oneWireReadByte();   // LSB
   temp |= ((uint16_t)oneWireReadByte() ) << 8;  // MSB   
   EXIT_CRITICAL();
   
   return temp;  
}