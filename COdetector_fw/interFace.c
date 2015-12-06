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


#define LCD_OPTION_SIGN_POS_X      0

/*****************************************************************************************
   LOCAL VARIABLES
*/

static eMainState_t mainActState = UNKNOWN_M_STATE;


/*****************************************************************************************
   LOCAL FUNCTIONS DECLARATIONS
*/


static void interDispValsBackground ( void );
static void interMainStateMachineSet ( eMainState_t state );

static void interStateHello ( void );
static void interDisplayConfig ( void );
static void interChooseConfig ( eButtons_t bt );


/*****************************************************************************************
   LOCAL FUNCTIONS DEFINITIONS
*/

void interInit ( void )
{
   interMainStateMachineSet ( HELLO_M_STATE );  // Initial state
   //interDispValsBackground ();
   
   LOG_TXT ( ">>init<<   Interface initialized\n" );    
}

//****************************************************************************************
// State machine for main activities:
static void interMainStateMachineSet ( eMainState_t state )
{
   switch ( state )
   {
      case HELLO_M_STATE:      
         LOG_TXT ( ">>state<<  Hello state\n" );
         mainActState = HELLO_M_STATE;
         interStateHello ();    
         break;
            
      case DISPVALS_M_STATE:
         LOG_TXT ( ">>state<<  Disp vals state\n" );    
         mainActState = DISPVALS_M_STATE;         
         interDispValsBackground ();     // Main display values background                
         break;
         
      case CONFIG_M_STATE:
         LOG_TXT ( ">>state<<  Config state\n" );
         mainActState = CONFIG_M_STATE;
         interDisplayConfig ();         
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
    
    interMainStateMachineSet( DISPVALS_M_STATE );    
} 




//****************************************************************************************
// Displaying values:
static void interDispValsBackground ( void )
{
   pdcLine ( "Act:        mV", LCD_ACTMEAS_POS_Y );
   pdcLine ( "M1m:        mV", LCD_MEAN_1M_POS_Y );
   pdcLine ( "M1h:        mV", LCD_MEAN_1H_POS_Y );
   pdcLine ( "M8h:        mV", LCD_MEAN_8H_POS_Y );
   pdcLine ( "Info      Menu", LCD_BT_INFO_POS_Y );   // TODO: change graphics

}


/*****************************************************************************************
   GLOBAL FUNCTIONS DEFINITIONS
*/

//****************************************************************************************
// Displaying values:
void interDisplaySystemVals ( valsToDisp_t* pVal )
{
   if ( DISPVALS_M_STATE == mainActState ) // Only when device is in appropriate state
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
// Displaying configuration menu:
void interDisplayConfig ( void )
{
   if ( CONFIG_M_STATE == mainActState )
   {
      pdcLine ( " CONFIG MENU  ", 0 );
      pdcLine ( " Time set     ", TIME_O_POS );
      pdcLine ( "              ", 2 );
      pdcLine ( "              ", 3 );
      pdcLine ( " EXIT         ", EXIT_O_POS );
      pdcLine ( " -    OK    + ", LCD_BT_INFO_POS_Y );   // TODO: change graphics
      interChooseConfig ( BT_NULL );                     // Set first option 
   }
}

//****************************************************************************************
// Get around config menu (by mainState):
static void interChooseConfig ( eButtons_t bt )
{
   static eOptionsState_t stateTab[] = { START_CONF_O, TIME_O_POS, EXIT_O_POS, END_CONF_O };
   static uint8_t state = 1;
   
   pdcChar( ' ', stateTab[state], LCD_OPTION_SIGN_POS_X ); // Clearing old *
   
   switch ( bt )
   {
      case BT_LEFT:  // Decrease       
         
         state--;
         if ( START_CONF_O == stateTab[state] ) 
         {
            state = sizeof(stateTab)/sizeof( eOptionsState_t ) - 2;            
         }     
      break;
      
      case BT_RIGHT: // Increase
      
         state++;
         if ( END_CONF_O == stateTab[state] ) 
         {
            state =  1;
         }                     
      break;
      
      case BT_OK:    // Get in
      
         switch ( stateTab[state] )       
         {
            case TIME_O_POS:
               
            break;     
              
            case EXIT_O_POS:
               interMainStateMachineSet ( DISPVALS_M_STATE );
               state = START_CONF_O + 1;
            break;  
            
            default:
            break;
         }  
         
      break;
      
      case BT_NULL:  // For initial option choose
      break;   
     
      default:
      break;
         
   }
   pdcChar( '*', stateTab[state], LCD_OPTION_SIGN_POS_X  ); // Setting new *
   
}


//****************************************************************************************
// On button pressed:
void interOnRight ( void )
{
   _delay_ms (25);         // Debouncing // TODO: another manner    
   
   if ( BT_RIGHT & ~PORTD.IN )    // Checking if it isn't a glitch
   {      
      LOG_TXT ( ">>info<< RIGHT bt pressed\n" );
      
      switch ( mainActState )
      {
         case DISPVALS_M_STATE:
            interMainStateMachineSet ( CONFIG_M_STATE );
         break;
         
         case CONFIG_M_STATE:
            interChooseConfig ( BT_RIGHT );
         break;
         
         default:
         break;
       }  
   }   
}

//****************************************************************************************
// On button pressed:
void interOnLeft ( void )
{
   _delay_ms (25);         // Debouncing // TODO: another manner    
   
   if ( BT_LEFT & ~PORTD.IN )    // Checking if it isn't a glitch
   {      
      LOG_TXT ( ">>info<< LEFT bt pressed\n" );
      
      switch ( mainActState )
      {
         case  DISPVALS_M_STATE:
            interMainStateMachineSet ( INFO_M_STATE );
         break;
         
         case CONFIG_M_STATE:
            interChooseConfig ( BT_LEFT );
         break;
         
         default:
         break;
      }
   }   
}

//****************************************************************************************
// On button pressed:
void interOnOk ( void )
{
   _delay_ms (25);         // Debouncing // TODO: another manner    
   
   if ( BT_OK & ~PORTD.IN )    // Checking if it isn't a glitch
   {      
      LOG_TXT ( ">>info<< OK bt pressed\n" );
      
      switch ( mainActState )
      {
         case  DISPVALS_M_STATE:
         interMainStateMachineSet ( ALARM_M_STATE );
         break;
         
         case  CONFIG_M_STATE:
         interChooseConfig ( BT_OK );
         break;
         
         default:
         break;
      }
   }   
}