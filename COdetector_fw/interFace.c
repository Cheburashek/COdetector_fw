/*
 * interFace.c
 *
 * Created: 2015-12-05 23:31:22
 *  Author: Chebu
 */ 


/*****************************************************************************************
   LOCAL INCLUDES
*/
#include "interFace.h"
#include "PDC8544.h"
#include "timers.h"
#include "IO.h"

/*****************************************************************************************
   MACROS
*/

/*****************************************************************************************
   LOCAL DEFINITIONS
*/

// Positions (Y) on LCD:
#define LCD_TIME_POS_Y             0
#define LCD_ACTMEAS_POS_Y          1
#define LCD_MEAN_1M_POS_Y          2
#define LCD_MEAN_1H_POS_Y          3
#define LCD_MEAN_8H_POS_Y          4
#define LCD_BT_INFO_POS_Y          5


/*****************************************************************************************
   LOCAL VARIABLES
*/

eMainState_t actState = UNKNOWN_M_STATE;

/*****************************************************************************************
   LOCAL FUNCTIONS DECLARATIONS
*/


static void interDispValsBackground ( void );
static void interMainStateMachine ( eMainState_t state );

static void interStateHello ( void );
static void interConfigMenu ( void );

/*****************************************************************************************
   LOCAL FUNCTIONS DEFINITIONS
*/

void interInit ( void )
{
   interMainStateMachine ( HELLO_M_STATE );  // Initial state
   //interDispValsBackground ();
   
   LOG_TXT ( ">>init<<   Interface initialized\n" );    
}

//****************************************************************************************
// State machine for main activities:
static void interMainStateMachine ( eMainState_t state )
{
   switch ( state )
   {
      case HELLO_M_STATE:      
         LOG_TXT ( ">>state<<  Hello state\n" );
         actState = HELLO_M_STATE;
         interStateHello ();    
         break;
            
      case DISPVALS_M_STATE:
         LOG_TXT ( ">>state<<  Disp vals state\n" );    
         actState = DISPVALS_M_STATE;         
         interDispValsBackground();     // Main display values background                
         break;
         
      case CONFIG_M_STATE:
         LOG_TXT ( ">>state<<  Config state\n" );
         actState = CONFIG_M_STATE;
         interConfigMenu ();         
         break;
         
         
         
      default:
         break;
            
   }        
}



//****************************************************************************************
// Hello screen:
static void interStateHello ( void )
{

    pdcLine( " CO detector ", 0 );   //TODO: in pdc -> padding string with spaces
    pdcLine( " by A.Tyran  ", 1 );
    pdcLine( "    2015     ", 2 );    
    pdcLine( "   EiT AGH   ", 3 );
    pdcLine( "   ver 0.1   ", 5 );
    
    ioBuzzerOn();
    ioStatLedOn();
    _delay_ms ( 100 );
    ioBuzzerOff();
    ioStatLedOff();
    _delay_ms ( 200 );
    ioBuzzerOn();
    ioStatLedOn();
    _delay_ms ( 300 );   
    ioBuzzerOff();
    ioStatLedOff();
    _delay_ms ( 1600 );
    
    interMainStateMachine( DISPVALS_M_STATE );    
} 




//****************************************************************************************
// Displaying values:
static void interDispValsBackground ( void )
{
   pdcClearRAM();              // Clearing LCD  // TODO: change it to line
   pdcLine ( "Act:        mV", LCD_ACTMEAS_POS_Y );
   pdcLine ( "M1m:        mV", LCD_MEAN_1M_POS_Y );
   pdcLine ( "M1h:        mV", LCD_MEAN_1H_POS_Y );
   pdcLine ( "M8h:        mV", LCD_MEAN_8H_POS_Y );
   pdcLine ( "Info      Menu", LCD_BT_INFO_POS_Y );   // TODO: change graphics

}


//****************************************************************************************
// Main config menu:
static void interConfigMenu ( void )
{
   pdcClearRAM();              // Clearing LCD  // TODO: change it to line
}


/*****************************************************************************************
   GLOBAL FUNCTIONS DEFINITIONS
*/

//****************************************************************************************
// Displaying values:
void interDisplaySystemVals ( valsToDisp_t* pVal )
{
   if ( DISPVALS_M_STATE == actState ) // Only when device is in appropriate state
   {
      // Time:
      char str[14] = {"              "};
      sprintf ( str, "%.2u:%.2u:%.2u", pVal->sysTime.hour, pVal->sysTime.min, pVal->sysTime.sec );
      pdcLine( str, LCD_TIME_POS_Y );
   
      // Measured values:
      pdcUint ( pVal->actVal, LCD_ACTMEAS_POS_Y, 6, 5 );
      pdcUint ( pVal->mean1mVal, LCD_MEAN_1M_POS_Y, 6, 5 );
      pdcUint ( pVal->mean1hVal, LCD_MEAN_1H_POS_Y, 6, 5 );
      pdcUint ( pVal->mean8hVal, LCD_MEAN_8H_POS_Y, 6, 5 );
      
   }        
}


//****************************************************************************************
// On button pressed:
void interOnButtons ( eButtons_t bt )
{
   _delay_ms (25);         // Debouncing // TODO: another manner
   
   if ( bt & ~PORTD.IN )    // Checking if it isn't glitch
   {      
      switch ( bt )
      {
         case BT_LEFT:
         LOG_TXT ( ">>info<< LEFT bt pressed\n" );
         break;
         
         case BT_OK:
         LOG_TXT ( ">>info<< OK bt pressed\n" );
         
         break;
         
         case BT_RIGHT:
         LOG_TXT ( ">>info<< RIGHT bt pressed\n" );
         
         break;
         
         default:
         break;
      }
   }   
}

