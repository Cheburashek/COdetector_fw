/*
 * serial.c
 *
 * Created: 2015-10-02 21:22:13
 *  Author: Chebu
 */ 

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
   #define BSEL_BAUD_VAL 9                // 57600bps @ 2MHz & CLK2X enabled
   #define BSCALE_BAUD_VAL (0b11110000)   //  -1

#elif ( F_CPU == F_CPU_8MHZ )
   #define BSEL_BAUD_VAL 17               // 57600bps @ 8MHz & CLK2X enabled
   #define BSCALE_BAUD_VAL (0b11110000)   //  -1

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
   PORTD.OUTCLR=PIN7_bm; 
   // PORTC configuration:
   
   PORTC.REMAP &= ~PORT_USART0_bm;       // Don't remap ports from 0-3 to 4-7   
   PORTC.DIRSET = CFG_TXC0_PIN_MASK;     // Output for Tx (pin must be manually set to output)
   PORTC.DIRCLR = CFG_RXC0_PIN_MASK;     // Input for Rx
   
   // CTRLC:
   USARTC0.CTRLC =  ( USART_CMODE_ASYNCHRONOUS_gc |    // Asynchronous transfer mode
                      USART_PMODE_DISABLED_gc     |    // Parity mode disabled
                      USART_CHSIZE_8BIT_gc        );   // 8b per frame                   
                      
   USARTC0.CTRLC &= ~USART_SBMODE_bm;  // Stop bit disabled
   
   // Baud rate 57600bps for 8MHz clock:
       
   USARTC0.BAUDCTRLA =  BSEL_BAUD_VAL;           // 8 LSB of BSEL
   // TODO : change this U2 -5:
   USARTC0.BAUDCTRLB =  BSCALE_BAUD_VAL | ((BSEL_BAUD_VAL >> 8) & 0x0F) ;           // 4 MSB of BSEL and BSCALE    
       
        
   // Priorities from common.h:
   USARTC0.CTRLA = CFG_PRIO_USARTC0;  
    
   // Clearing interrupt flags:    
   USARTC0.STATUS = 0x00; 
         
   USARTC0.CTRLB |= ( USART_TXEN_bm |    // Transmitter enabled
                      USART_RXEN_bm |    // Receiver enabled
                      USART_CLK2X_bm );  // Enabling 2x clock    
                       
   USARTC0.STATUS &= ~USART_TXCIF_bm;    // Clearing tx interrupt flag
   
   LOG_TXT ( ">>init<<   Serial initialized\n", 31 );
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
      
      if ( txBuff == txTail )       // Initial send
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