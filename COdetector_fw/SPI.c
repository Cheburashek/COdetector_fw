/*
 * SPI.c
 *
 * Created: 2015-10-06 21:10:38
 *  Author: Chebu
 */ 

// Only for receiving, clock frequency should be <4MHz

/*****************************************************************************************
   LOCAL INCLUDES
*/
#include "SPI.h"

/*****************************************************************************************
   MACROS
*/

/*****************************************************************************************
   LOCAL DEFINITIONS
*/

#define TX_BUFF_LEN 600  // minimum is 6 rows * 84 columns in 3310 LCD 

/*****************************************************************************************
   LOCAL VARIABLES
*/

// It's not a circular, but linear buffer!
static uint8_t  txBuff [ TX_BUFF_LEN ];
static uint8_t* txHead = txBuff;
static uint8_t* txTail = txBuff;


static pfnTxEnd txEndCB = NULL;
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
void spiInit ( void )
{
   
   // Pins as output: 
   PORTC.DIRSET |= ( CFG_RST_PIN_MASK  |
                     CFG_SCE_PIN_MASK  |
                     CFG_DC_PIN_MASK   |
                     CFG_MOSI_PIN_MASK |
                     CFG_SCK_PIN_MASK  ); 
   
   // Unbuffered mode (simple tx use)
   SPIC.CTRLB = SPI_SSD_bm;      // Slave select disable (master mode) 
   
   // TODO: DMA?
   SPIC.CTRL = ( SPI_CLK2X_bm    |     // Clock Double
                 SPI_MASTER_bm   );    // Master mode               
   
  // SPIC.INTCTRL = CFG_PRIO_SPI;        // Interrupt level from boardCfg.h     

                                
   LOG_TXT ( ">>init<<   SPI initialized\n", 28 );
   SPI_EN(); // wywalic
}

//****************************************************************************************
void spiSend ( const uint8_t* data, uint16_t len )
{
   // TODO: Critical section  here
   if ( ((txBuff + TX_BUFF_LEN)-txHead) > len )  // If there's a place to copy data
   {
      for ( uint8_t i = 0; i < len; i++ )
      {
         *txHead = *data;
         txHead++;
         data++;
      }
      
      if ( txBuff == txTail )    // Initial send
      {
         SPI_EN();               // Enabling SPI
         SPIC.DATA = *txTail;    // First character sent starts transmission
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
void spiRegisterTxEndCB ( pfnTxEnd cb)
{
   txEndCB = cb;   
}


//****************************************************************************************
ISR ( SPIC_INT_vect )
{
   // Is there critical section necessary ?
   if ( txTail < txHead )
   {
      SPIC.DATA = *txTail;
      txTail++;
   }
   else // All of data from buffer are send
   {
      txTail = txBuff;
      txHead = txTail;
      
      if ( NULL != txEndCB )
      {
         txEndCB();         
      }
      
      SPI_DIS();     // Disabling SPI (only tx is used)
   }
 
}