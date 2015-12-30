/*
 * interFace.c
 *
 * Created: 2015-12-05 23:31:22
 *  Author: Chebu
 */ 


// U¿ytkownik jest informowany kiedy i jaki stage alarmu

/*****************************************************************************************
   LOCAL INCLUDES
*/
#include "interFace.h"
#include "PDC8544.h"
#include "timers.h"
#include "IO.h"
#include "system.h"

/*****************************************************************************************
   MACROS
*/

/*****************************************************************************************
   LOCAL DEFINITIONS
*/


#define LCD_HEADER_POS_Y           0
#define LCD_ACTMEAS_POS_Y          1
#define LCD_MEAN_1M_POS_Y          2
#define LCD_MEAN_1H_POS_Y          3
#define LCD_MEAN_2H_POS_Y          4
#define LCD_BT_INFO_POS_Y          5
#define LCD_SENS_SET_POS_Y         3
#define LCD_INFO_LA_POS_Y          0
#define LCD_INFO_MAX_POS_Y         3

#define LCD_ALARM_POS_Y            0

// Positions (X) on LCD:
#define LCD_OPTION_SIGN_POS_X      0

/*****************************************************************************************
   LOCAL VARIABLES
*/

static eMainState_t mainActState = UNKNOWN_M_STATE;
static timeStruct_t sysTime;
static lastAlarm_t lastAlarm = {{0,0,0}, NO_ALARM_STAGE}; 

static bool loBatSignFlag = FALSE;

static uint16_t max1mVal = 0;

static uint16_t codeTemp = SENS_NA_PPM_MULTI_1k;

/*****************************************************************************************
   LOCAL FUNCTIONS DECLARATIONS
*/


static void interDispValsBackground ( void );
static void interMainStateMachineSet ( eMainState_t state );

static void interDisplayHello ( void );
static void interDisplayConfig ( void );
static void interDisplayTimeSet ( void );
static void interDisplayInfo ( void );

static void interChooseConfig ( eButtons_t bt );
static void interChooseTimeSet ( eButtons_t bt );
static void interDisplayTimeSetUpdate ( void );
static void interDisplaySensCode ( void );

static void interSetSensCode ( eButtons_t bt );

static void interAlarmSirene ( bool stat );

static void interAlarmCBHi ( void );



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
         
      case INFO_M_STATE:
         LOG_TXT ( ">>state<<  Info state\n" );
         mainActState = INFO_M_STATE;
         interDisplayInfo ();     // Main display values background
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
         break;
         
      case ALARM_M_STATE:
         LOG_TXT ( ">>state<<  Alarm state\n" );
         mainActState = ALARM_M_STATE;   
         interAlarmSirene( TRUE );
      break;   
   
      case SENS_CODE_STATE:
         LOG_TXT ( ">>state<<  Sensor code state\n" );
         mainActState = SENS_CODE_STATE;
         interDisplaySensCode();
      break;   
      
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
    _delay_ms ( 500 );
    
    interMainStateMachineSet( DISPVALS_M_STATE );    
} 




//****************************************************************************************
// Displaying values:
static void interDispValsBackground ( void )
{
   pdcInit();
   pdcLine ( "Act:       ppm", LCD_ACTMEAS_POS_Y );
   pdcLine ( "M1m:       ppm", LCD_MEAN_1M_POS_Y );
   pdcLine ( "M1h:       ppm", LCD_MEAN_1H_POS_Y );
   //pdcLine ( "M8h:       ppm", LCD_MEAN_2H_POS_Y );
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
      pdcLine ( " Sens. code   ", SENS_CODE_O_POS );
      pdcLine ( " Alarm test   ", ALARM_O_POS );
      pdcLine ( " EXIT         ", EXIT_O_POS );
      pdcLine ( " <-   OK   -> ", LCD_BT_INFO_POS_Y );   
      interChooseConfig ( BT_NULL );                     // Set first option 
   }
}


//****************************************************************************************
// Displaying time to be set:
static void interDisplayTimeSet ( void )
{
   if ( TIME_SET_M_STATE == mainActState )
   {      
      pdcLine ( " Hour:        ", HOUR_POS );
      pdcLine ( " Min:         ", MIN_POS );
      pdcClearLine ( 2 );
      pdcClearLine ( 3 );
      pdcLine ( " EXIT         ", EXIT_T_POS );
      pdcLine ( " -   <-->   + ", LCD_BT_INFO_POS_Y );   
      interChooseTimeSet ( BT_NULL );                     // Set first option
   }
}

static void interDisplaySensCode ( void )
{
   if ( SENS_CODE_STATE == mainActState )
   {      
      pdcLine ( "  Set sensor  ", 0 );
      pdcLine ( "code from case", 1 );
      pdcLine ( "   [pA/ppm]   ", 2 );
      pdcClearLine ( 3 );
      pdcUint( codeTemp, LCD_SENS_SET_POS_Y, 1, 4 );     // Display actual value  
      pdcClearLine ( 4 );
      pdcLine ( " -    OK    + ", 5 ); 
              
   }      
}


//****************************************************************************************
// Get around config menu :
static void interChooseConfig ( eButtons_t bt )
{
   eOptionsState_t stateTab[] = { TIME_O_POS, SENS_CODE_O_POS, ALARM_O_POS, EXIT_O_POS };
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
            
            case SENS_CODE_O_POS:
               interMainStateMachineSet ( SENS_CODE_STATE );
            break;
              
            case ALARM_O_POS:
               interAlarmSirene( TRUE );
               while ( GET_BT_OK_STATE() ) {;}  // Waiting for OK button release
               interAlarmSirene( FALSE );   
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
   eTimeSetState_t stateTab[] = { HOUR_POS, MIN_POS, EXIT_T_POS };
   static uint8_t state = 0x00;
   
   switch ( bt )
   {
      case BT_RIGHT:   // Increase value    
         switch ( stateTab[state] )
         {
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
// Set multiplier [nA/ppm] from code on the sensor case:
static void interSetSensCode ( eButtons_t bt )
{
   _delay_ms(100);
   
   switch (bt)
   {      
      case BT_RIGHT:
         do
         {
            if ( codeTemp++ > 2500 ) codeTemp = 500;
            pdcUint( codeTemp, LCD_SENS_SET_POS_Y, 1, 4 );
            _delay_ms(10);
         } while ( GET_BT_RIGHT_STATE() );      
      break;
      
      case BT_LEFT:
         do 
         {
            if ( codeTemp-- < 500 ) codeTemp = 2500;            
            pdcUint( codeTemp, LCD_SENS_SET_POS_Y, 1, 4 ); 
            _delay_ms(10);
         } while ( GET_BT_LEFT_STATE() );
      break;
      
      case BT_OK:
         systemSensCodeSet( codeTemp );
         interMainStateMachineSet( CONFIG_M_STATE );
      break;
      
      default:
      break;      
   }   
}

//****************************************************************************************
static void interDisplayTimeSetUpdate ( void )
{
      // TODO: clear days of sensor
      pdcUint ( sysTime.hour,  HOUR_POS,  7, 2 );
      pdcUint ( sysTime.min,   MIN_POS,   7, 2 );
}      


//****************************************************************************************
static void interDisplayInfo ( void )
{
   
   // Last alarm:
   pdcInit();
   
   if ( lastAlarm.laStage != NO_ALARM_STAGE )
   {
      pdcLine( "Last alarm:  ", LCD_INFO_LA_POS_Y);
      pdcUint ( lastAlarm.laTime.hour,  LCD_INFO_LA_POS_Y+1,  0, 2 );
      pdcChar( ':', LCD_INFO_LA_POS_Y+1, 3 );
      pdcUint ( lastAlarm.laTime.min,   LCD_INFO_LA_POS_Y+1,  4, 2 );
      pdcLine( ">     ppm    ", LCD_INFO_LA_POS_Y+2 );
      
      switch ( lastAlarm.laStage )
      {
         case ALARM_STAGE_1M:
         pdcUint( TRESH_1M_PPM, LCD_INFO_LA_POS_Y+2, 3, 3 );
         break;
         case ALARM_STAGE_15M:
         pdcUint( TRESH_15M_PPM, LCD_INFO_LA_POS_Y+2, 3, 3 );
         break;
         case ALARM_STAGE_1H:
         pdcUint( TRESH_1H_PPM, LCD_INFO_LA_POS_Y+2, 3, 3 );
         break;
         case ALARM_STAGE_2H:
         pdcUint( TRESH_2H_PPM, LCD_INFO_LA_POS_Y+2, 3, 3 );
         break;
         default:
         break;
      }
   }
   else
   {
       pdcLine( "No alarm yet ", LCD_INFO_LA_POS_Y);
   }
   
   // Max 1 minute value:
   pdcLine( "Max       ppm", LCD_INFO_MAX_POS_Y );
   pdcUint( max1mVal, LCD_INFO_MAX_POS_Y , 5, 3 );
   
   pdcLine ( "         EXIT ", 5 );
   
}


//****************************************************************************************
// Sirene on/off
static void interAlarmSirene ( bool stat )
{
   static eChanNr_t alarmTimIDHi = TIM_ERROR;
      
      
   if ( TRUE == stat ) // Turn on
   {
      alarmTimIDHi = timerRegisterAndStart ( interAlarmCBHi, ALARM_PERIOD_HI, TRUE );
   }
   else // Turn off
   {
      timerDeregister ( alarmTimIDHi );
      ioBuzzerOff();
      ioStatLedOff();
   }
}

static void interAlarmCBHi ( void )
{
   ioBuzzerTgl();
   ioStatLedTgl();   
}


/*****************************************************************************************
   GLOBAL FUNCTIONS DEFINITIONS
*/

void interInit ( void )
{   
#ifdef HELLO_SCREEN_PERM   
   interMainStateMachineSet ( HELLO_M_STATE ); 
#else
   interMainStateMachineSet ( DISPVALS_M_STATE );  
#endif   
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
         if ( pVal->actBattVal > TRESH_LOW_BATT )
         {
            sprintf ( str, "%.2u:%.2u:%.2u  %.4u", sysTime.hour,sysTime.min, sysTime.sec, pVal->actBattVal );
            loBatSignFlag = FALSE;
         }
         else
         {
            sprintf ( str, "%.2u:%.2u:%.2u !LOW!", sysTime.hour,sysTime.min, sysTime.sec );
            loBatSignFlag = TRUE;
         }         
      }
      pdcLine( str, LCD_HEADER_POS_Y );
      
      // Measured values:
      pdcUint ( pVal->mean15sVal, LCD_ACTMEAS_POS_Y, 6, 5 );   // Changed -> 15s as actual (meaning)
      pdcUint ( pVal->mean1mVal, LCD_MEAN_1M_POS_Y, 6, 5 );
      pdcUint ( pVal->mean1hVal, LCD_MEAN_1H_POS_Y, 6, 5 );
      //pdcUint ( pVal->mean2hVal, LCD_MEAN_2H_POS_Y, 6, 5 ); 
      
      // Led blinking:
      #ifdef STAT_LED_ON_TICK_US
         ioStateLedShortTick ();
      #endif   
      

   }    
   else if ( ALARM_M_STATE == mainActState )
   {
      pdcUint ( pVal->mean15sVal, 5, 0, 4 );      
      if ( pVal->mean15sVal < TRESH_ALARM_OFF_PPM )
      {
         interAlarmSirene( FALSE );
         interMainStateMachineSet( DISPVALS_M_STATE );    
         systemResetMeasRes (); 
      }
   }  
         
      if ( max1mVal < pVal->actSensVal )
      {
         max1mVal = pVal->mean15sVal;     // Max 1min value
      }
      
         
   // Serial log:
   char strToLog [64];
   uint8_t len =  sprintf ( strToLog, "[%.2u:%.2u:%.2u] %.4u [ppm] \n", sysTime.hour, sysTime.min, sysTime.sec, pVal->actSensVal );
   LOG_TXT_WL ( strToLog, len ); // Sensor value with timestamp
}



//****************************************************************************************
void interTimeTickUpdate ( void )
{
   sysTime.sec += RTC_PERIOD_S;   // Period could be changed
   
   if ( sysTime.sec >= 60 )  
   { 
      sysTime.sec = 0;
      sysTime.min ++; 
   }
   if ( sysTime.min >= 60 )
   { 
      sysTime.min = 0;
      sysTime.hour ++; 
   } 
   if ( sysTime.hour >= 24 ) 
   { 
      sysTime.hour = 0;
      //sysTime.numOfDays ++;      
   }

   if ( TIME_SET_M_STATE == mainActState )
   {
      interDisplayTimeSetUpdate ();    // Updating time in time set menu
   }      
   
   // Signal of low batt:
   if ( TRUE == loBatSignFlag )
   {
      ioBuzzerOn();
      _delay_ms(10);
      ioBuzzerOff();
   }      
    

}

//****************************************************************************************
// Alarm stage set:
void interAlarmStage ( eAlarmStages_t stage  )
{
   if ( ALARM_M_STATE != mainActState )
   {
      interMainStateMachineSet( ALARM_M_STATE );      
      
      lastAlarm.laStage = stage; // Last alarm info
      lastAlarm.laTime  = sysTime;      
      
      pdcLine ( "  ! ALARM !   ", 0);
      pdcLine ( "Exceeded      ", 2 );
      pdcLine ( "        ppm   ", 3 );
      pdcLine ( "Actual        ", 4 );
      pdcLine ( "        ppm   ", 5 );
      
      switch ( stage )
      {
         case ALARM_STAGE_1M:
         pdcLine ( "1 min meaning", 1 );
         pdcUint ( TRESH_1M_PPM, 3, 0, 4 );
         break;
         
         case ALARM_STAGE_15M:
         pdcLine ( "15 min mean. ", 1 );
         pdcUint ( TRESH_15M_PPM, 3, 0, 4 );
         break;
         
         case ALARM_STAGE_1H:
         pdcLine ( "1 h meaning  ", 1 );
         pdcUint ( TRESH_1H_PPM, 3, 0, 4 );
         break;
         
         case ALARM_STAGE_2H:
         pdcLine ( "2 h meaning  ", 1 );
         pdcUint ( TRESH_2H_PPM, 3, 0, 4 );
         break;
         
         default:
         break;
      }
   }   
}






//****************************************************************************************
// On button pressed:
void interOnRight ( void )
{
   _delay_ms (25);          
   
   if ( GET_BT_RIGHT_STATE() )    // Checking if it isn't a glitch
   {      
      LOG_TXT ( ">>info<< RIGHT bt pressed\n" );
      #ifdef BUZZER_ON_BT_US
         ioBuzzShortBeep ();
      #endif      
      
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
         
         case INFO_M_STATE:
            interMainStateMachineSet( DISPVALS_M_STATE );
         break;     
         
         case SENS_CODE_STATE:
            interSetSensCode(BT_RIGHT);
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
   _delay_ms (25);         
   
   if ( GET_BT_LEFT_STATE() )    // Checking if it isn't a glitch
   {      
      LOG_TXT ( ">>info<< LEFT bt pressed\n" );
      #ifdef BUZZER_ON_BT_US
         ioBuzzShortBeep ();
      #endif            
      
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
        
         case SENS_CODE_STATE:
            interSetSensCode( BT_LEFT );
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
   _delay_ms (25);        
   
   if ( GET_BT_OK_STATE() )    // Checking if it isn't a glitch
   {      
      LOG_TXT ( ">>info<< OK bt pressed\n" );
      #ifdef BUZZER_ON_BT_US
         ioBuzzShortBeep ();
      #endif
      
      switch ( mainActState )
      {         
         case  CONFIG_M_STATE:
            interChooseConfig ( BT_OK );
         break;
         
         case TIME_SET_M_STATE:
            interChooseTimeSet ( BT_OK );
         break;        
         
         case SENS_CODE_STATE:
            interSetSensCode( BT_OK );
         break;
             
         default:
         break;
      }
   }   
}