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



/*****************************************************************************************
   MACROS
*/

/*****************************************************************************************
   LOCAL DEFINITIONS
*/



// Lengths of meaning queues:
#define MEAN_1M_QUEUE_LEN          60/RTC_PERIOD_S         // Meaning at every tick (RTC_PERIOD_S) for 1-minute 
#define MEAN_1H_QUEUE_LEN          60*MEAN_1M_QUEUE_LEN/4  // Meaning at every 15s for 1-hour         
#define MEAN_8H_QUEUE_LEN          480                     // Meaning at every minute for 8-hour


/*****************************************************************************************
   LOCAL TYPEDEFS
*/

/*****************************************************************************************
   LOCAL VARIABLES
*/

static volatile uint16_t rawVal;


// Meaning queue:
static meanQueue_t mean1mQ;
static meanQueue_t mean1hQ;
static meanQueue_t mean8hQ;

// Structure with values to display on LCD:
static valsToDisp_t locVals;


/*****************************************************************************************
   LOCAL FUNCTIONS DECLARATIONS
*/


static void systemPeriodicRefresh ( void );

static void systemQueueInit (  meanQueue_t* pQueue, uint16_t len );
static void systemQueuePush (  meanQueue_t* pQueue, meanType_t val );
static void systemQueueCalcMean ( meanQueue_t* pQueue, meanType_t* buf );

static uint16_t systemConvertFromRaw ( uint16_t raw );

static void systemTimeTickUpdate ( void );

static void systemSerialLog ( void );




/*****************************************************************************************
   LOCAL FUNCTIONS DEFINITIONS
*/

//****************************************************************************************
// Periodic refresh (from RTC):
static void systemPeriodicRefresh ( void )
{   
   static uint16_t ticks = 0;   
   static bool initFlag = false;
   
   
   
#ifdef STAT_LED_ON_ADC
   ioStatLedOn();          // Turning on status LED (blink driven by ADC measuring time)
#endif
   
   adcStartChToGnd();      // Starting measurement
   
   if ( initFlag )
   {
      locVals.actVal = systemConvertFromRaw ( rawVal );     // Converting from raw (16b) value from ADC to [ppm] or actually [mV]
   
      // Every tick:
      systemQueuePush ( &mean1mQ, locVals.actVal );
      systemQueueCalcMean ( &mean1mQ, &locVals.mean1mVal ); // For 1min meaning
    
      // Every 15 s:   
      if ( 0 == (ticks % 15) )     
      {
         systemQueuePush ( &mean1hQ, locVals.mean1mVal );
         systemQueueCalcMean ( &mean1hQ, &locVals.mean1hVal );  // For 1h meaning
      }
   
      // Every 1 min :
      if ( 0 == (ticks % 60)  )    
      {
         systemQueuePush ( &mean8hQ, locVals.mean1hVal );
         systemQueueCalcMean ( &mean8hQ, &locVals.mean8hVal );  // For 8h meaning 
         ticks = 0;
      }
   }   
 
   systemTimeTickUpdate();
   systemSerialLog();  
   interDisplaySystemVals ( &locVals );
   
   ticks += RTC_PERIOD_S;
   initFlag = true;
}

//****************************************************************************************
// Initializing queue:

static void systemQueueInit (  meanQueue_t* pQueue, uint16_t len )
{
   pQueue->pStart = malloc ( len );
   pQueue->pHead = pQueue->pStart;
   pQueue->len = len;
       
   if ( NULL == pQueue->pHead )
   {
      LOG_TXT ( ">>err<< Cannot allocate queue!" );
      while(1){;}      
   }   
}

//****************************************************************************************
// Adding value to queue:
static void systemQueuePush (  meanQueue_t* pQueue, meanType_t val )
{  
   *pQueue->pHead = val;
   
   if ( pQueue->pHead < (pQueue->pStart + pQueue->len) )
   {
      pQueue->pHead++;
   }
   else
   {
      pQueue->pHead = pQueue->pStart;
      pQueue->firstOF = true;   
   }   
}

//****************************************************************************************
// Calculating mean value from queue:
static void systemQueueCalcMean ( meanQueue_t* pQueue, meanType_t* buf )
{
   //TODO: critical section?
   uint16_t len = pQueue->len;
   meanType_t* pTemp = pQueue->pStart;
   uint32_t tempVal = 0;
   
   if ( false == pQueue->firstOF ) // When before first queue overflow
   {
      len = pQueue->pHead - pQueue->pStart;  // Use only stored data, not random values from memory
   }
   
   for ( uint16_t i = 0; i < len; i++ )
   {
      tempVal += *pTemp;      
      pTemp++;     
   }

   *buf = (meanType_t)(tempVal/len);   // Mean value      
}




//****************************************************************************************
static uint16_t systemConvertFromRaw ( uint16_t raw )
{
  uint32_t rawData = (((uint32_t)raw) * ADC_DIVIDER) / 65535; // for 1 [mV] resolution @16b 
   
  return (uint16_t)rawData;
}




//****************************************************************************************
static void systemTimeTickUpdate ( void )
{
   locVals.sysTime.sec += RTC_PERIOD_S;   // Period could be changed
   
   if ( locVals.sysTime.sec >=60 )  
   { 
      locVals.sysTime.sec = 0;
      locVals.sysTime.min ++; 
   }
   if ( locVals.sysTime.min >=60 )
   { 
      locVals.sysTime.min = 0;
      locVals.sysTime.hour ++; 
   } 
   if ( locVals.sysTime.hour >=24 ) 
   { 
      locVals.sysTime.hour = 0;
      locVals.sysTime.day ++; 
   } 
      
   // TODO: days, months...
}



//****************************************************************************************
static void systemSerialLog ( void )
{
   char strToLog [64];
   uint8_t len =  sprintf ( strToLog, "[%.2u:%.2u:%.2u] %.4u[mV] \n", locVals.sysTime.hour, locVals.sysTime.min, locVals.sysTime.sec, locVals.actVal );
   LOG_TXT_WL ( strToLog, len );
   
} 




/*****************************************************************************************
   GLOBAL FUNCTIONS DEFINITIONS
*/


//****************************************************************************************
void systemInit ( void ) 
{
   
   systemQueueInit ( &mean1mQ, MEAN_1M_QUEUE_LEN );
   systemQueueInit ( &mean1hQ, MEAN_1H_QUEUE_LEN );
   systemQueueInit ( &mean8hQ, MEAN_8H_QUEUE_LEN );
   
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
   
   
   
   adcRegisterEndCb( systemMeasEnd );      // Registering CB
   timerRegisterRtcCB ( systemPeriodicRefresh );
   
   LOG_TXT ( ">>init<<   System initialized\n" );
}

//****************************************************************************************


//****************************************************************************************
void systemMeasEnd ( uint16_t val )
{
   rawVal = val;
   
#ifdef STAT_LED_ON_ADC   
   ioStatLedOff();      // State LED blinking
#endif
}  