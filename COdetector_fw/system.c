/*
 * system.c
 *
 * Created: 2015-10-31 21:02:07
 *  Author: Chebu
 */ 


// TODO: w czasie LoPo - co 5s, bez LoPo - 1s period

// Do opisu:
// Najpierw uruchamiam pomiar (20ms), aby jak najkrócej by³ wzbudzony uk³ad. 

/*****************************************************************************************
   LOCAL INCLUDES
*/

#include "system.h"
#include "timers.h"
#include "ADC.h"
#include "interFace.h"
#include "IO.h"



/*****************************************************************************************
   MACROS
*/

/*****************************************************************************************
   LOCAL DEFINITIONS
*/



// Lengths of meaning queues:
#define MEAN_15S_QUEUE_LEN         15/RTC_PERIOD_S         // Meaning at every tick (RTC_PERIOD_S) for 15-sec
#define MEAN_1M_QUEUE_LEN          60/RTC_PERIOD_S         // Meaning at every tick (RTC_PERIOD_S) for 1-minute 
#define MEAN_15M_QUEUE_LEN         60                      // Meaning at every 15s for 15-min   
#define MEAN_1H_QUEUE_LEN          60                      // Meaning at every min for 1-hour           
#define MEAN_2H_QUEUE_LEN          120                     // Meaning at every min for 2-hour


/*****************************************************************************************
   LOCAL TYPEDEFS
*/

/*****************************************************************************************
   LOCAL VARIABLES
*/

static volatile uint16_t rawSensVal;
static volatile uint16_t rawBattVal;
static volatile uint16_t rawTempVal;
static uint16_t sensCodeNaPpm = SENS_NA_PPM_MULTI_1k;

// Meaning queue:
static measType_t mean15sTab[MEAN_15S_QUEUE_LEN];
static measType_t mean1mTab[MEAN_1M_QUEUE_LEN];
static measType_t mean15mTab[MEAN_15M_QUEUE_LEN];
static measType_t mean1hTab[MEAN_1H_QUEUE_LEN];
static measType_t mean2hTab[MEAN_2H_QUEUE_LEN];

// Tables for static queue allocation:
static meanQueue_t mean15sQ = { mean15sTab, mean15sTab, MEAN_15S_QUEUE_LEN };
static meanQueue_t mean1mQ = { mean1mTab, mean1mTab, MEAN_1M_QUEUE_LEN };
static meanQueue_t mean15mQ = { mean15mTab, mean15mTab, MEAN_15M_QUEUE_LEN };
static meanQueue_t mean1hQ = { mean1hTab, mean1hTab, MEAN_1H_QUEUE_LEN };
static meanQueue_t mean2hQ = { mean2hTab, mean2hTab, MEAN_2H_QUEUE_LEN };

// Flags:
static bool vBattFlag = TRUE;    // For adc measurement triggering
static bool vTempFlag = FALSE;

// Structure with values to display on LCD:
static valsToDisp_t locVals;


/*****************************************************************************************
   LOCAL FUNCTIONS DECLARATIONS
*/


static void systemPeriodicRefresh ( void );

static void systemQueuePush (  meanQueue_t* pQueue, measType_t val );
static void systemQueueCalcMean ( meanQueue_t* pQueue, measType_t* buf );

static void systemCheckTresholds ( void );

static uint16_t systemConvRawSens ( uint16_t raw );
static uint16_t systemConvRawBatt ( uint16_t raw );
static void systemQueueReset (  meanQueue_t* pQueue );




/*****************************************************************************************
   LOCAL FUNCTIONS DEFINITIONS
*/

//****************************************************************************************
// Periodic refresh (from RTC):
static void systemPeriodicRefresh ( void )
{   
   static uint16_t ticks = 0;   

   adcStartChannel (SENS);      // Starting sensor voltage measurement


   // Every tick: 
      
   locVals.actSensVal = systemConvRawSens ( rawSensVal );     // Converting from raw (16b) value from ADC to [ppm] or actually [mV]    
   
   systemQueuePush ( &mean15sQ, locVals.actSensVal );  
   systemQueuePush ( &mean1mQ, locVals.actSensVal );  // TODO: 1min from 15s?
   
   systemQueueCalcMean ( &mean15sQ, &locVals.mean15sVal );    // For 1min meaning
   systemQueueCalcMean ( &mean1mQ, &locVals.mean1mVal );      // For 1min meaning
  
   // Every 15s:
   if ( 0 == (ticks % 15) )
   {                  
      systemQueuePush ( &mean15mQ, locVals.actSensVal );
      systemQueueCalcMean ( &mean15mQ, &locVals.mean15mVal );     // For 1min meaning
      
      /*if ( !locVals.usbPlugged )*/ 
      {
         vBattFlag = TRUE; // Setting this flag enables vbatt meas. after sens meas.   
         locVals.actBattVal = systemConvRawBatt ( rawBattVal );
      }
            
      #ifdef TEMP_MEAS_PERM
         vTempFlag = TRUE; // Setting this flag enables temperature meas. after vBatt meas. 
      #endif
   }      
        
   // Every minute:   
   if ( 0 == (ticks % 60) )     
   {

      systemQueuePush ( &mean1hQ, locVals.mean1mVal );
      systemQueueCalcMean ( &mean1hQ, &locVals.mean1hVal );  // For 1h meaning
         
      systemQueuePush ( &mean2hQ, locVals.mean1mVal );
      systemQueueCalcMean ( &mean2hQ, &locVals.mean2hVal );  // For 8h meaning
   
      
      
      ticks = 0;
   }    
   
 
#ifdef ALARM_PERM
   systemCheckTresholds();
#endif
      
   interDisplaySystemVals ( &locVals );
       
   interTimeTickUpdate();
   ticks += RTC_PERIOD_S;
}

//****************************************************************************************
// Adding value to queue:
static void systemQueuePush (  meanQueue_t* pQueue, measType_t val )
{  
   *pQueue->pHead = val;
   
   if ( pQueue->pHead < (pQueue->pStart + pQueue->len) )
   {
      pQueue->pHead++;
   }
   else
   {
      pQueue->pHead = pQueue->pStart; 
   }   
}

//****************************************************************************************
// Reset queue:
static void systemQueueReset ( meanQueue_t* pQueue )
{  
   measType_t* pTemp = pQueue->pStart;
   
   for ( uint8_t i = 0; i < pQueue->len; i++ )
   {
      *pTemp = 0x00;
      pTemp ++;
   }
   
   pQueue->pHead = pQueue->pStart;
}

//****************************************************************************************
// Calculating mean value from queue:
static void systemQueueCalcMean ( meanQueue_t* pQueue, measType_t* buf )
{
   measType_t* pTemp = pQueue->pStart;
   uint32_t tempVal = 0;   
   
   for ( uint16_t i = 0; i < pQueue->len; i++ )
   {
      tempVal += *pTemp;      
      pTemp++;     
   }

   *buf = (measType_t)(tempVal/pQueue->len);   // Mean value      
}


//****************************************************************************************
// Converting from raw sensor value to ppm
static uint16_t systemConvRawSens ( uint16_t raw )
{
  uint32_t temp = (((uint32_t)raw) * ADC_SENS_MULTI_MV) / 65535; // for 1 [mV] resolution @16b 
   
  if ( SENS_OFFSET_MV < temp )
  {
     temp = ((temp - SENS_OFFSET_MV) * SENS_NA_MV_MULTI_1k)/sensCodeNaPpm;     // for [ppm]
  }
  else
  {
     temp = 0;
  }   
   
  return (uint16_t)temp;
}

//****************************************************************************************
// Converting from raw vBatt value
static uint16_t systemConvRawBatt ( uint16_t raw )
{
   uint32_t rawData = (((uint32_t)raw) * ADC_VBATT_MULTI_MV) / 65535; // for 1 [mV] resolution @16b
   
   return (uint16_t)rawData;
}

//****************************************************************************************
// Checking tresholds for alarm
static void systemCheckTresholds ( void )
{
   if ( locVals.mean1mVal >= TRESH_1M_PPM )
   {
      interAlarmStage ( ALARM_STAGE_1M );
   }
   if ( locVals.mean15mVal >= TRESH_15M_PPM )
   {
      interAlarmStage ( ALARM_STAGE_15M );
   }
   if ( locVals.mean1hVal >= TRESH_1H_PPM )
   {
      interAlarmStage ( ALARM_STAGE_1H );
   }
   if ( locVals.mean2hVal >= TRESH_2H_PPM )
   {
      interAlarmStage ( ALARM_STAGE_2H );
   }

}


/*****************************************************************************************
   GLOBAL FUNCTIONS DEFINITIONS
*/


//****************************************************************************************
void systemInit ( void ) 
{
   
   /* TEST
   
   for ( uint16_t v = 0; v < MEAN_8H_QUEUE_LEN; v++ )
   {      
      systemQueuePush( &mean8hQ, v );
   }   
   
   uint16_t val;
   
   systemQueueCalcMean( &mean8hQ, &val );

   
   LOG_UINT ( "mean: ", val );
   LOG_UINT ( "len: ", mean8hQ.len );
   LOG_UINT ( "OFf: ", mean8hQ.firstOF );
   LOG_UINT ( "pS: ", (uint16_t)mean8hQ.pStart );
   LOG_UINT ( "pH: ", (uint16_t)mean8hQ.pHead );
   TEST*/
   
   
   systemUSBStateChanged();   // Initial check of USB state
   adcRegisterEndCb( systemMeasEnd );      // Registering CB
   timerRegisterRtcCB ( systemPeriodicRefresh );
   adcStartChannel (VBATT);      // Starting sensor voltage measurement   

}


//****************************************************************************************
// After changing USB connection state:
void systemUSBStateChanged ( void )
{
   _delay_ms(30);  // Glitch
   
   if ( IO_GET_USB_CONN() )   // If USB plugged in
   {
      locVals.lpFlag = FALSE;
      locVals.usbPlugged = TRUE;
      IO_FALLING_EDGE_USB();  // Falling edge sense - now pin state is high
      
      //boardWakeUp ();
   }
   else
   {      
      //boardGoSleep ();
      
      locVals.lpFlag = TRUE;
      locVals.usbPlugged = FALSE;
      IO_RISING_EDGE_USB();  // Rising edge sense - now pin state is low
   }
}

//****************************************************************************************
void systemResetMeasRes ( void )
{
   locVals.actSensVal = 0x00;
   locVals.mean15sVal = 0x00; 
   locVals.mean1mVal = 0x00; 
   locVals.mean15mVal = 0x00;
   locVals.mean1hVal = 0x00;
   locVals.mean2hVal = 0x00;
   
   // Reset Queues:
   systemQueueReset ( &mean15sQ );
   systemQueueReset ( &mean1mQ );
   systemQueueReset ( &mean15mQ );
   systemQueueReset ( &mean1hQ );
   systemQueueReset ( &mean2hQ );
   
}

//****************************************************************************************
void systemSensCodeSet ( uint16_t val )
{
   sensCodeNaPpm = val;
}


//****************************************************************************************
void systemMeasEnd ( uint16_t val )
{
   ADC_DIS();
   
   if ( ADC_CH_INPUTMODE_INTERNAL_gc == ADCA.CH0.CTRL )   // If internal mode (temperature)
   {
      //rawTempVal = val;
      //LOG_UINT ( "raw TEMP ", val );
   }
   else if ( SENS == ADC_GET_CH() )
   {      
      rawSensVal = val;
      
      if ( vBattFlag )
      {          
         adcStartChannel ( VBATT ); // Battery voltage measurement start
         vBattFlag = FALSE;               
      }    
      if ( vTempFlag )
      {
         adcStartChannel ( TEMP );  // Temperature measurement start         
         vTempFlag = FALSE;
      }    
      
      //LOG_UINT ( "raw sens ", val );  
   }
   else if ( VBATT == ADC_GET_CH() )
   {
      rawBattVal = val;
      LOG_UINT ( "raw bat ", val );        
   }   

}  