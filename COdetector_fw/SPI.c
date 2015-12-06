/*
 * SPI.c
 *
 * Created: 2015-10-06 21:10:38
 *  Author: Chebu
 */ 

// Only for receiving, clock frequency should be <4MHz


//Do opisu: struktura do SPI - na czas wysy³ania bajtu potrzebne jest ustawienie odpowiednio pinu DC
// co normalnie nie by³oby mo¿liwe, dlatego stworzy³em strukturê, która pozwala na wspó³bie¿ne wysy³anie bajtu
// i ustawianie stowarzyszonego z nim stanu pinu Data/Config


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

#define TX_BUFF_LEN 256  

/*****************************************************************************************
   LOCAL VARIABLES
*/

// It's not a circular, but linear buffer!
static spiEnhStruct_t  txBuff [ TX_BUFF_LEN ];
static spiEnhStruct_t* txHead = txBuff;
static spiEnhStruct_t* txTail = txBuff;


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
   PORTC.DIRSET = (  CFG_RST_PIN_MASK  |
                     CFG_SCE_PIN_MASK  |
                     CFG_DC_PIN_MASK   |
                     CFG_MOSI_PIN_MASK |
                     CFG_SCK_PIN_MASK  ); 
   
   PORTC.DIRCLR= PIN4_bm;             // Important: Slave select is there!
   PORTC.PIN4CTRL = PORT_OPC_PULLUP_gc;
   
   SPIC.CTRLB = SPI_SSD_bm;            // Slave select disable (master mode), unbuffered mode   
   
   SPIC.CTRL = ( /*SPI_CLK2X_bm    |*/     // Clock Double
                 SPI_MASTER_bm   );    // Master mode          
   
   SPIC.INTCTRL = CFG_PRIO_SPI;        // Interrupt level from boardCfg.h    
                                
   LOG_TXT ( ">>init<<   SPI initialized\n" );   
}

//****************************************************************************************
void spiSend ( spiEnhStruct_t* dataStr, uint16_t len )
{
   ENTER_CRIT_ALL();
   
   if ( ((txBuff + TX_BUFF_LEN)-txHead) > len )  // If there's a place to copy data
   {
      for ( uint8_t i = 0; i < len; i++ )
      {
         *txHead = *dataStr;
         txHead++;
         dataStr++;  // TODO: check if it could be as a txHead++ = dataStr++;
      }
      
      if ( txBuff == txTail )    // Initial send
      {
         SPI_EN();               // Enabling SPI
         
#ifdef ENHANCED_SPI
         if ( txTail->outDC )
         {
            DC_HI();            
         }
         else
         {
            DC_LO();            
         }
#endif
         SPIC.DATA = txTail->data;    // First character sent starts transmission
         
         txTail++;
      }
   }
   else
   {
      // Overflow or data too big
   }    
   
   EXIT_CRITICAL();
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
               
 #ifdef ENHANCED_SPI
      if ( txTail->outDC )
      {
         DC_HI();
      }
      else
      {
         DC_LO();
      }
 #endif
 
      SPIC.DATA = txTail->data;
      txTail++;
   }
   else // All of data from buffer is send
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