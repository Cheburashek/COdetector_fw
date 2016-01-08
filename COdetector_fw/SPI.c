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

#define TX_BUFF_LEN 64
#define TX_BUFF_MAX_ADDR()       (txBuff + TX_BUFF_LEN) 

/*****************************************************************************************
   LOCAL VARIABLES
*/
// Circular buffer:
static spiEnhStruct_t  txBuff [ TX_BUFF_LEN ];
static volatile spiEnhStruct_t* txHead = txBuff;
static volatile spiEnhStruct_t* txTail = txBuff;
static bool initFlag = FALSE;

static bool busyFlag = FALSE;

/*****************************************************************************************
   LOCAL FUNCTIONS DECLARATIONS
*/

static bool spiCheckCap ( void );

/*****************************************************************************************
   LOCAL FUNCTIONS DEFINITIONS
*/

//****************************************************************************************
// Check if there is place for new data
static bool spiCheckCap ( void )
{
   if ( (txHead+1) != txTail )
   {
      return true;
   }      
   else if ( (txTail != txBuff) && ((txHead+1) != TX_BUFF_MAX_ADDR()) )   // If not at the end and start of buffer
   {     
      return true;
   }           
     
   return false;
}



/*****************************************************************************************
   GLOBAL FUNCTIONS DEFINITIONS
*/


//****************************************************************************************
void spiInit ( void )
{
   
   // Pins as output: 
   PORTC.DIRSET = (  CFG_SCE_PIN_MASK  |
                     CFG_DC_PIN_MASK   |
                     CFG_MOSI_PIN_MASK |
                     CFG_SCK_PIN_MASK  );
   
   PORTA.DIRSET = CFG_RST_PIN_MASK;
   
   PORTC.DIRCLR= PIN4_bm;             // Important: Slave select is there!
   PORTC.PIN4CTRL = PORT_OPC_PULLUP_gc;
   
   SPIC.CTRLB = SPI_SSD_bm;            // Slave select disable (master mode), unbuffered mode   
   
   SPIC.CTRL = ( SPI_CLK2X_bm    |     // Clock Double
                 SPI_MASTER_bm   );    // Master mode          
   
   SPIC.INTCTRL = CFG_PRIO_SPI;        // Interrupt level from boardCfg.h    
                                                    
   initFlag = true;                                
}

//****************************************************************************************
bool spiIsBusy ( void )
{
   return busyFlag;   
}

//****************************************************************************************
void spiSend ( spiEnhStruct_t* dataStr )
{  
   busyFlag = TRUE;
   
   if ( true == spiCheckCap() )  // If there's a place to copy data
   {          
      *txHead = *dataStr; 

      if ( (txHead == txTail) && (true == initFlag) )    // Initial send
      {      
         SPI_EN();
         
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
         // When Tail is going to exceed buffer size
         if ( ++txTail == TX_BUFF_MAX_ADDR() ) { txTail = txBuff; }
      }      
      if ( ++txHead == TX_BUFF_MAX_ADDR() ) { txHead = txBuff; }    
   }
   else  // Overflow
   {
      LOG_TXT ( ">>warn<< SPI buffer OF\n" );
   }    
}


//****************************************************************************************
ISR ( SPIC_INT_vect )
{
   if ( txTail != txHead )
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
      // When tail is going to exceed buffer size
      if ( ++txTail == TX_BUFF_MAX_ADDR() ) { txTail = txBuff; }
   }
   else // All of data from buffer is send
   {
      SPI_DIS ();
      busyFlag = FALSE;
   }      
 
}