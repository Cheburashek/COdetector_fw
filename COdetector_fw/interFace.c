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
#define LCD_INFO_MAX_POS_Y         2
#define LCD_INFO_SENS_TIME_POS_Y   4

#define LCD_ALARM_POS_Y            0

// Positions (X) on LCD:
#define LCD_OPTION_SIGN_POS_X      0

/*****************************************************************************************
   LOCAL VARIABLES
*/

static eMainState_t mainActState = UNKNOWN_M_STATE;
static timeStruct_t sysTime;
static uint32_t sensTimeH; // Time of sensor usage

static maxVal_t maxVal; // Max 15s meaning val with time
static lastAlarm_t lastAlarm = {{0,0,0}, NO_ALARM_STAGE}; 

static bool loBatSignFlag = FALSE;

static uint16_t codeTemp = SENS_NA_PPM_MULTI_1k;

/*****************************************************************************************
   LOCAL FUNCTIONS DECLARATIONS
*/


static void interDispValsBackground ( void );

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
// Hello screen:
static void interDisplayHello ( void )
{

    pdcLine( " CO detector ", 0, FALSE );
    pdcLine( " by A.Tyran  ", 1, FALSE );
    pdcLine( "    2015     ", 2, FALSE );    
    pdcLine( "   EiT AGH   ", 3, FALSE );
    pdcLine( "   ver 0.1   ", 5, FALSE );
    
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
   pdcLine ( "              ", LCD_BT_INFO_POS_Y-1, FALSE );
   pdcLine ( "Actual:    ppm", LCD_ACTMEAS_POS_Y, FALSE );
   pdcLine ( "For 1m:    ppm", LCD_MEAN_1M_POS_Y, FALSE );
   pdcLine ( "For 1h:    ppm", LCD_MEAN_1H_POS_Y, FALSE );
   pdcLine ( "For 2h:    ppm", LCD_MEAN_2H_POS_Y, FALSE );   
   pdcLine ( "Info      Menu", LCD_BT_INFO_POS_Y, TRUE );
   
}

//****************************************************************************************
// Displaying configuration menu:
void interDisplayConfig ( void )
{
   if ( CONFIG_M_STATE == mainActState )
   {      
      pdcLine ( " CONFIG MENU  ", 0, TRUE );
      pdcLine ( " Time set     ", TIME_O_POS, FALSE );
      pdcLine ( " Sens. code   ", SENS_CODE_O_POS, FALSE );
      pdcLine ( " Alarm test   ", ALARM_O_POS, FALSE );
      pdcLine ( " EXIT         ", EXIT_O_POS, FALSE );
      pdcLine ( " <-   OK   -> ", LCD_BT_INFO_POS_Y, TRUE );   
      interChooseConfig ( BT_NULL );                     // Set first option 
   }
}


//****************************************************************************************
// Displaying time to be set:
static void interDisplayTimeSet ( void )
{
   if ( TIME_SET_M_STATE == mainActState )
   {      
      pdcLine ( " Hour:        ", HOUR_POS, FALSE );
      pdcLine ( " Min:         ", MIN_POS, FALSE );
      pdcClearLine ( 2 );
      pdcClearLine ( 3 );
      pdcLine ( " EXIT         ", EXIT_T_POS, FALSE );
      pdcLine ( " -   <-->   + ", LCD_BT_INFO_POS_Y, TRUE );   
      interChooseTimeSet ( BT_NULL );                     // Set first option
   }
}

static void interDisplaySensCode ( void )
{
   if ( SENS_CODE_STATE == mainActState )
   {      
      pdcLine ( "  Set sensor  ", 0, FALSE );
      pdcLine ( "code from case", 1, FALSE );
      pdcLine ( "   [pA/ppm]   ", 2, FALSE );
      pdcClearLine ( 3 );
      pdcUint( codeTemp, LCD_SENS_SET_POS_Y, 1, 4 );     // Display actual value  
      pdcClearLine ( 4 );
      pdcLine ( " -    OK    + ", 5, TRUE ); 
              
   }      
}

//****************************************************************************************
// Get around config menu :
static void interChooseConfig ( eButtons_t bt )
{
   eOptionsState_t stateTab[] = { TIME_O_POS, SENS_CODE_O_POS, ALARM_O_POS, EXIT_O_POS };
   static uint8_t state = 0x00;
   
   if ( bt != BT_OK ) pdcChar( ' ', stateTab[state], LCD_OPTION_SIGN_POS_X, FALSE ); // Clearing old *
   
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
   if ( BT_OK != bt )pdcChar( '*', stateTab[state], LCD_OPTION_SIGN_POS_X, FALSE  ); // Setting new *
   
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
         pdcChar( ' ', stateTab[state], LCD_OPTION_SIGN_POS_X, FALSE ); // Clearing old *         
         if ( state++ == sizeof(stateTab)/sizeof( eOptionsState_t ) - 1 ) { state =  0; }
         pdcChar( '*', stateTab[state], LCD_OPTION_SIGN_POS_X, FALSE  ); // Setting new *         
      break;
      
      case BT_NULL:  // For initial option choose
         pdcChar( '*', 0x00, LCD_OPTION_SIGN_POS_X, FALSE  ); // Setting initial *
      break;   
     
      default:
      break;
         
   }
   
   if ( TIME_SET_M_STATE == mainActState ) interDisplayTimeSetUpdate();  // Update time after setting
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
      pdcUint ( sysTime.hour,  HOUR_POS,  7, 2 );
      pdcUint ( sysTime.min,   MIN_POS,   7, 2 );
}      


//****************************************************************************************
static void interDisplayInfo ( void )
{
   char str[14] = {"              "};
   uint16_t ppmTresh = 0x00;
      
   // Last alarm:
   
   pdcLine( " Last alarm:  ", LCD_INFO_LA_POS_Y, TRUE );
   
   if ( lastAlarm.laStage != NO_ALARM_STAGE )
   {     
      switch ( lastAlarm.laStage )
      {        
         case ALARM_STAGE_1M:
         ppmTresh = TRESH_1M_PPM;
         break;
         case ALARM_STAGE_15M:
         ppmTresh= TRESH_15M_PPM;
         break;
         case ALARM_STAGE_1H:
         ppmTresh = TRESH_1H_PPM;
         break;
         case ALARM_STAGE_2H:
         ppmTresh = TRESH_2H_PPM;
         break;
         default:
         break;
      }
      
      sprintf ( str, "%.3u ppm %.2u:%.2u", ppmTresh ,lastAlarm.laTime.hour, lastAlarm.laTime.min );
      pdcLine( str, LCD_INFO_LA_POS_Y+1, FALSE );
   }
   else
   {
       pdcLine( "No alarm yet ", LCD_INFO_LA_POS_Y+1, FALSE);
   }
   
   // Max 1 minute value:
   pdcLine( "Max 1min val.:", LCD_INFO_MAX_POS_Y, TRUE );
   sprintf ( str, "%.3u ppm %.2u:%.2u", maxVal.mvVal, maxVal.mvTime.hour, maxVal.mvTime.min  );  
   
   pdcLine( str, LCD_INFO_MAX_POS_Y+1, FALSE );
   
   // Time of sensor usage:
   pdcLine ( "S.days:      ", LCD_INFO_SENS_TIME_POS_Y, FALSE);
   pdcUint ( sensTimeH, LCD_INFO_SENS_TIME_POS_Y, 8, 6 );
   
   pdcLine ( "         EXIT", 5, TRUE );
  
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

//****************************************************************************************
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
// State machine for main activities:
void interMainStateMachineSet ( eMainState_t state )
{
   switch ( state )
   {
      case HELLO_M_STATE:               
         mainActState = HELLO_M_STATE;
         interDisplayHello ();    
         break;
            
      case DISPVALS_M_STATE:         
         mainActState = DISPVALS_M_STATE;         
         interDispValsBackground ();               
         break;
         
      case INFO_M_STATE:
         mainActState = INFO_M_STATE;
         interDisplayInfo ();     
      break;
         
      case CONFIG_M_STATE:
         mainActState = CONFIG_M_STATE;
         interDisplayConfig ();         
         break;
         
      case TIME_SET_M_STATE:
         mainActState = TIME_SET_M_STATE;
         interDisplayTimeSet ();
         break;
         
      case ALARM_M_STATE:
         mainActState = ALARM_M_STATE;   
         interAlarmSirene( TRUE );
      break;   
   
      case SENS_CODE_STATE:
         mainActState = SENS_CODE_STATE;
         interDisplaySensCode();
      break;   
      
      default:
         break;
            
   }        
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
         sprintf ( str, "%.2u:%.2u %.2iC  USB", sysTime.hour,sysTime.min, pVal->tempC );
         loBatSignFlag = FALSE;
      }
      else
      {         
         if ( pVal->battPer > TRESH_LOW_BATT_PER )
         {
            sprintf ( str, "%.2u:%.2u %.2iC %.2u%%", sysTime.hour,sysTime.min, pVal->tempC, pVal->battPer );
            loBatSignFlag = FALSE;
         }
         else
         {
            sprintf ( str, "%.2u:%.2u %.2iC LOW!", sysTime.hour,sysTime.min, pVal->tempC );
            loBatSignFlag = TRUE;
         }         
      }
      pdcLine( str, LCD_HEADER_POS_Y, TRUE );
      
      // Measured values:
      pdcUint ( pVal->mean15sVal, LCD_ACTMEAS_POS_Y, 8, 3 ); 
      pdcUint ( pVal->mean1mVal, LCD_MEAN_1M_POS_Y, 8, 3 );
      pdcUint ( pVal->mean1hVal, LCD_MEAN_1H_POS_Y, 8, 3 );  
      pdcUint ( pVal->mean2hVal, LCD_MEAN_2H_POS_Y, 8, 3 );     
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
         
   if ( maxVal.mvVal < pVal->mean15sVal )
   {
      maxVal.mvVal = pVal->mean15sVal;     // Max 15s value
      maxVal.mvTime = sysTime;
   }      
         
   // Serial log if USB plugged in:
   if ( pVal->usbPlugged )
   {
      char strToLog [64];
      uint8_t len =  sprintf ( strToLog, "[%.2u:%.2u:%.2u] %.4u [ppm] @ %.2u C \n", sysTime.hour, sysTime.min, sysTime.sec, pVal->actSensVal, pVal->tempC );
      DATA_TXT_WL ( strToLog, len ); // Sensor value with timestamp
   }   
}



//****************************************************************************************
void interTimeTickUpdate ( uint8_t rtcPer )
{
   sysTime.sec += rtcPer;   
   
   if ( sysTime.sec >= 60 )  
   { 
      sysTime.sec = 0;
      sysTime.min ++; 
   }
   if ( sysTime.min >= 60 )
   { 
      sysTime.min = 0;
      sysTime.hour ++; 
      sensTimeH ++;
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
      
   // Led blinking:
   #ifdef STAT_LED_ON_TICK_PERM
   if ( DISPVALS_M_STATE == mainActState )
   ioStateLedShortTick ();
   #endif
         
   // Signal of low battery:
   if ( TRUE == loBatSignFlag )
   {      
      ioBuzzerOn();
      _delay_ms(15);
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
      
      pdcLine ( "  ! ALARM !   ", 0, TRUE);
      pdcLine ( "Exceeded      ", 2, FALSE );
      pdcLine ( "        ppm   ", 3, FALSE );
      pdcLine ( "Actual        ", 4, FALSE );
      pdcLine ( "        ppm   ", 5, FALSE );
      
      switch ( stage )
      {
         case ALARM_STAGE_1M:
         pdcLine ( "1 min meaning", 1, FALSE );
         pdcUint ( TRESH_1M_PPM, 3, 0, 4 );
         break;
         
         case ALARM_STAGE_15M:
         pdcLine ( "15 min mean. ", 1, FALSE );
         pdcUint ( TRESH_15M_PPM, 3, 0, 4 );
         break;
         
         case ALARM_STAGE_1H:
         pdcLine ( "1 h meaning  ", 1, FALSE );
         pdcUint ( TRESH_1H_PPM, 3, 0, 4 );
         break;
         
         case ALARM_STAGE_2H:
         pdcLine ( "2 h meaning  ", 1, FALSE );
         pdcUint ( TRESH_2H_PPM, 3, 0, 4 );
         break;
         
         default:
         break;
      }
   }   
}

//****************************************************************************************
eMainState_t interGetMainState ( void )
{
   return mainActState;
}


//****************************************************************************************
// On button pressed:
void interOnRight ( void )
{
   _delay_ms (25);          
   
   if ( GET_BT_RIGHT_STATE() )    // Checking if it isn't a glitch
   {      
      LOG_TXT ( ">>info<< RIGHT bt pressed\n" );
      #ifdef BUZZER_ON_BT_PERM
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
      #ifdef BUZZER_ON_BT_PERM
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
      #ifdef BUZZER_ON_BT_PERM
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

