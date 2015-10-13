/*
 * serial.c
 *
 * Created: 2015-10-02 21:22:13
 *  Author: Chebu
 */ 

// RX is off!


/*****************************************************************************************
   LOCAL INCLUDES
*/
#include "serial.h"

/*****************************************************************************************
   MACROS
*/

/*****************************************************************************************
   LOCAL DEFINITIONS
*/
#if ( F_CPU == F_CPU_32KHZ )
   #define BSEL_BAUD_VAL 22               // 2400bps @ 32768Hz & CLK2X enabled
   #define BSCALE_BAUD_VAL (0b10110000)   // Should be between -7 to 7 in U2 // -5
   
#elif ( F_CPU == F_CPU_2MHZ )
   #define BSEL_BAUD_VAL 54                // 57600bps @ 2MHz & CLK2X enabled
   #define BSCALE_BAUD_VAL (0b11000000)   //  -4

#elif ( F_CPU == F_CPU_8MHZ )
   #define BSEL_BAUD_VAL 110               // 57600bps @ 8MHz & CLK2X enabled
   #define BSCALE_BAUD_VAL (0b10110000)   //  -5

#else 
   #error "F_CPU is not defined correctly!"
#endif


#define TX_BUF_LEN 256
#define RX_BUF_LEN 128


/*****************************************************************************************
   LOCAL VARIABLES
*/

// It's not a circular, but linear buffer!
static uint8_t  txBuff [ TX_BUF_LEN ];
static uint8_t* txHead = txBuff;
static uint8_t* txTail = txBuff;

static bool initFlag = FALSE;

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
// Serial on PORTC initialization:
void serialInitC ( void )
{
   // PORTC configuration:
   
   PORTC.REMAP &= ~PORT_USART0_bm;       // Don't remap ports from 0-3 to 4-7   
   PORTC.DIRSET = CFG_TXC0_PIN_MASK;     // Output for Tx (pin must be manually set to output)
   PORTC.DIRCLR = CFG_RXC0_PIN_MASK;     // Input for Rx
   
   // CTRLC:
   USARTC0.CTRLC =  ( USART_CMODE_ASYNCHRONOUS_gc |    // Asynchronous transfer mode
                      USART_PMODE_DISABLED_gc     |    // Parity mode disabled
                      USART_CHSIZE_8BIT_gc        );   // 8b per frame                   
                      
   USARTC0.CTRLC &= ~USART_SBMODE_bm;  // Stop bit disabled
   
   // Baud rate
       
   USARTC0.BAUDCTRLA =  BSEL_BAUD_VAL;           // 8 LSB of BSEL
   USARTC0.BAUDCTRLB =  BSCALE_BAUD_VAL | ((BSEL_BAUD_VAL >> 8) & 0x0F) ;           // 4 MSB of BSEL and BSCALE    
       
        
   // Priorities from common.h:
   USARTC0.CTRLA = CFG_PRIO_USARTC0;  
         
   USARTC0.CTRLB |= ( USART_TXEN_bm |    // Transmitter enabled
                      //USART_RXEN_bm |    // Receiver enabled
                      USART_CLK2X_bm );  // Enabling 2x clock    
                       
   USARTC0.STATUS &= ~USART_TXCIF_bm;    // Clearing tx interrupt flag
   
   LOG_TXT ( ">>init<<   Serial initialized\n", 31 );
   
   initFlag = TRUE;
}


//****************************************************************************************
// Serial on PORTC send (add to buffer and start triggering tx):

void serialSendC ( const uint8_t* data, uint8_t len )
{
   
   // TODO: Critical section  here
   if ( ((txBuff + TX_BUF_LEN)-txHead) > len )  // If there's a place to copy data
   {
      for ( uint8_t i = 0; i < len; i++ )
      {
         *txHead = *data;
         txHead++;
         data++;                 
      }
      
      if ( (txBuff == txTail) && (TRUE == initFlag) )       // Initial send
      {        
         USARTC0.DATA = *txTail;    // First character sent starts transmission
         txTail++;
      }                
   }
   else
   {
      // Overflow or data too big  
      DEB_3_SET();    
   }
   
}


//****************************************************************************************
// TODO: modify this
void serialLogUintC ( uint8_t* txt, uint8_t len, uint32_t val )
{
   char temp[10];
      
   uint8_t numLen = 0;
   uint32_t tempVal = val;
   
   if ( 0 == val )
   {
      numLen = 1;
   }
   else
   {     
      while ( tempVal ) 
      {
         tempVal /= 10;
         numLen++;      
      }  
   }
   
      
   (void) utoa ( (unsigned int)val, temp, 10 );
   
   serialSendC ( (const uint8_t*) txt, len );
   serialSendC ( (const uint8_t*) temp, numLen );
   serialSendC ( (const uint8_t*) "\n", 2 );
   
}


//****************************************************************************************
// Serial Tx complete ISR ( tx flag is cleared automatically):

ISR ( USARTC0_TXC_vect )
{             
   if ( txTail < txHead )
   {
      USARTC0.DATA = *txTail;
      txTail++;      

   }
   else // All of data from buffer is send
   {
      txTail = txBuff;
      txHead = txTail;    
   } 
}


 
//****************************************************************************************
ISR ( USARTC0_RXC_vect )
{   
}