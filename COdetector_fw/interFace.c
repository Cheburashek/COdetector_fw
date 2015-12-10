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


#define LCD_HEADER_POS_Y             0
#define LCD_ACTMEAS_POS_Y          1
#define LCD_MEAN_1M_POS_Y          2
#define LCD_MEAN_1H_POS_Y          3
#define LCD_MEAN_8H_POS_Y          4
#define LCD_BT_INFO_POS_Y          5

// Positions (X) on LCD:
#define LCD_OPTION_SIGN_POS_X      0

/*****************************************************************************************
   LOCAL VARIABLES
*/

static eMainState_t mainActState = UNKNOWN_M_STATE;
static timeStruct_t sysTime;

/*****************************************************************************************
   LOCAL FUNCTIONS DECLARATIONS
*/


static void interDispValsBackground ( void );
static void interMainStateMachineSet ( eMainState_t state );

static void interDisplayHello ( void );
static void interDisplayConfig ( void );
static void interDisplayTimeSet ( void );

static void interChooseConfig ( eButtons_t bt );
static void interChooseTimeSet ( eButtons_t bt );
static void interDisplayTimeSetUpdate ( void );

/*****************************************************************************************
   LOCAL FUNCTIONS DEFINITIONS
*/


//****************************************************************************************
// State machine for main activities:
static void interMainStateMachineSet ( eMainState_t state )
{
   switch ( state )
   {
      case HELLO_M_STATE:      
         LOG_TXT ( ">>state<<  Hello state\n" );
         mainActState = HELLO_M_STATE;
         interDisplayHello ();    
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
         
      case TIME_SET_M_STATE:
         LOG_TXT ( ">>state<<  Time set state\n" );
         mainActState = TIME_SET_M_STATE;
         interDisplayTimeSet ();
      default:
         break;
            
   }        
}



//****************************************************************************************
// Hello screen:
static void interDisplayHello ( void )
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
   pdcLine ( "              ", 0 );
   pdcLine ( "Act:        mV", LCD_ACTMEAS_POS_Y );
   pdcLine ( "M1m:        mV", LCD_MEAN_1M_POS_Y );
   pdcLine ( "M1h:        mV", LCD_MEAN_1H_POS_Y );
   pdcLine ( "M8h:        mV", LCD_MEAN_8H_POS_Y );
   pdcLine ( "Info      Menu", LCD_BT_INFO_POS_Y );   // TODO: change graphics

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
// Displaying time to be set:
static void interDisplayTimeSet ( void )
{
   if ( TIME_SET_M_STATE == mainActState )
   {
      pdcLine ( " Day:         ", DAY_POS );
      pdcLine ( " Month:       ", MONTH_POS );
      pdcLine ( " Year:        ", YEAR_POS );
      pdcLine ( " Hour:        ", HOUR_POS );
      pdcLine ( " Min:         ", MIN_POS );
      pdcLine ( " EXIT         ", EXIT_T_POS );
      interChooseTimeSet ( BT_NULL );                     // Set first option
   }
}


//****************************************************************************************
// Get around config menu :
static void interChooseConfig ( eButtons_t bt )
{
   eOptionsState_t stateTab[] = { TIME_O_POS, EXIT_O_POS };
   static uint8_t state = 0x00;
   
   pdcChar( ' ', stateTab[state], LCD_OPTION_SIGN_POS_X ); // Clearing old *
   
   switch ( bt )
   {
      case BT_LEFT:  // Decrease              
         if ( state-- == 0x00 ) {  state = sizeof(stateTab)/sizeof( eOptionsState_t ) - 1; }               
      break;
      
      case BT_RIGHT: // Increase         
         if ( state++ == sizeof(stateTab)/sizeof( eOptionsState_t ) - 1 )  { state = 0x00; }                     
      break;
      
      case BT_OK:    // Get in
      
         switch ( stateTab[state] )       
         {
            case TIME_O_POS:
               interMainStateMachineSet ( TIME_SET_M_STATE );
            break;     
              
            case EXIT_O_POS:
               interMainStateMachineSet ( DISPVALS_M_STATE );
               state = 0x00;  // Initial state
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
// Get around time set menu:
static void interChooseTimeSet ( eButtons_t bt )
{
   eTimeSetState_t stateTab[] = {  YEAR_POS, MONTH_POS, DAY_POS, HOUR_POS, MIN_POS, EXIT_T_POS };
   static uint8_t state = 0x00;
   
   switch ( bt )
   {
      case BT_RIGHT:   // Increase value    
         switch ( stateTab[state] )
         {
            case YEAR_POS:               
               if ( ++sysTime.year > 2500 ) { sysTime.year = 0; }          // TODO: calendar with non-linear days
            break;
            case MONTH_POS:
               if ( ++sysTime.month > 12 ) { sysTime.month = 0; }
            break;          
            case DAY_POS:
               if ( ++sysTime.day > 31 ) { sysTime.day = 0; }          
            break;
            case HOUR_POS:
               if ( ++sysTime.hour > 23 ) { sysTime.hour = 0; }          
            break;
            case MIN_POS:
               if ( ++sysTime.min > 59 ) { sysTime.min = 0; }         
            break;            
            case EXIT_T_POS:
               state = 0x00;
               interMainStateMachineSet( CONFIG_M_STATE );     // On exit back to config state
            break;            
            default:
            break;                            
         }
         sysTime.sec = 0;  // Clearing secs
      break;
      
      case BT_LEFT: // Decrease value    
            switch ( stateTab[state] )
            {
               case YEAR_POS:
                  if ( sysTime.year-- == 1993 ) { sysTime.year = 2150; }          // TODO: calendar with non-linear days
               break;
               case MONTH_POS:
                  if ( sysTime.month-- == 0 ) { sysTime.month = 12; }
               break;
               case DAY_POS:
                  if ( sysTime.day-- == 0 ) { sysTime.day = 31; }
               break;
               case HOUR_POS:
                  if ( sysTime.hour-- == 0 ) { sysTime.hour = 0; }
               break;
               case MIN_POS:
                  if ( sysTime.min-- == 0 ) { sysTime.min = 0; }
               break;
               case EXIT_T_POS:
                  state = 0x00;
                  interMainStateMachineSet( CONFIG_M_STATE );     // On exit back to config state
               break;                          
               default:
               break;
            }
            sysTime.sec = 0;  // Clearing secs       
      break;
      
      case BT_OK:    // Next value      
         pdcChar( ' ', stateTab[state], LCD_OPTION_SIGN_POS_X ); // Clearing old *         
         if ( state++ == sizeof(stateTab)/sizeof( eOptionsState_t ) - 1 ) { state =  0; }
         pdcChar( '*', stateTab[state], LCD_OPTION_SIGN_POS_X  ); // Setting new *         
      break;
      
      case BT_NULL:  // For initial option choose
         pdcChar( '*', 0x00, LCD_OPTION_SIGN_POS_X  ); // Setting initial *
      break;   
     
      default:
      break;
         
   }
   
   interDisplayTimeSetUpdate();  // Update time after setting
}


//****************************************************************************************
static void interDisplayTimeSetUpdate ( void )
{
      pdcUint ( sysTime.year,  YEAR_POS,  7, 4 );
      pdcUint ( sysTime.month, MONTH_POS, 7, 2 );
      pdcUint ( sysTime.day,   DAY_POS,   7, 2 );
      pdcUint ( sysTime.hour,  HOUR_POS,  7, 2 );
      pdcUint ( sysTime.min,   MIN_POS,   7, 2 );
}      

/*****************************************************************************************
   GLOBAL FUNCTIONS DEFINITIONS
*/

void interInit ( void )
{
   interMainStateMachineSet ( HELLO_M_STATE );  // Initial state
   //interDispValsBackground ();
   
   LOG_TXT ( ">>init<<   Interface initialized\n" );
}



//****************************************************************************************
// Displaying values:
void interDisplaySystemVals ( valsToDisp_t* pVal )
{
   if ( DISPVALS_M_STATE == mainActState ) // Only when device is in appropriate state
   {
      // Time & Vbatt:
      char str[14] = {"              "};
      
      
      if ( pVal->usbPlugged ) // If USB plugged in
      {
         sprintf ( str, "%.2u:%.2u:%.2u   USB", sysTime.hour,sysTime.min, sysTime.sec );
      }
      else
      {
         sprintf ( str, "%.2u:%.2u:%.2u  %.4u", sysTime.hour,sysTime.min, sysTime.sec, pVal->actBattVal );
      }
      pdcLine( str, LCD_HEADER_POS_Y );

      
      // Measured values:
      pdcUint ( pVal->actSensVal, LCD_ACTMEAS_POS_Y, 6, 5 );
      pdcUint ( pVal->mean1mVal, LCD_MEAN_1M_POS_Y, 6, 5 );
      pdcUint ( pVal->mean1hVal, LCD_MEAN_1H_POS_Y, 6, 5 );
      pdcUint ( pVal->mean8hVal, LCD_MEAN_8H_POS_Y, 6, 5 );
      
   }      
         
   // Serial log:
   char strToLog [64];
   uint8_t len =  sprintf ( strToLog, "[%.2u:%.2u:%.2u] %.4u[mV] \n", sysTime.hour, sysTime.min, sysTime.sec, pVal->actSensVal );
   LOG_TXT_WL ( strToLog, len ); // Sensor value with timestamp
}



//****************************************************************************************
void interTimeTickUpdate ( void )
{
   sysTime.sec += RTC_PERIOD_S;   // Period could be changed
   
   if ( sysTime.sec >=60 )  
   { 
      sysTime.sec = 0;
      sysTime.min ++; 
   }
   if ( sysTime.min >=60 )
   { 
      sysTime.min = 0;
      sysTime.hour ++; 
   } 
   if ( sysTime.hour >=24 ) 
   { 
      sysTime.hour = 0;
      sysTime.day ++; 
   } 
      
   if ( TIME_SET_M_STATE == mainActState )
   {
      interDisplayTimeSetUpdate ();    // Updating time in time set menu
   }      
    
   // TODO: days, months...
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
         
         case TIME_SET_M_STATE:
            interChooseTimeSet ( BT_RIGHT );
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
         
         case TIME_SET_M_STATE:
            interChooseTimeSet ( BT_LEFT );
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
         
         case TIME_SET_M_STATE:
            interChooseTimeSet ( BT_OK );
         break;    
              
         default:
         break;
      }
   }   
}