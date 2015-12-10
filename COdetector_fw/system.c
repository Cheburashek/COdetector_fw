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
#define MEAN_1M_QUEUE_LEN          60/RTC_PERIOD_S         // Meaning at every tick (RTC_PERIOD_S) for 1-minute 
#define MEAN_1H_QUEUE_LEN          60                      // Meaning at every 15s for 1-hour         
#define MEAN_8H_QUEUE_LEN          96                      // Meaning at every 5 minutes for 8-hour


/*****************************************************************************************
   LOCAL TYPEDEFS
*/

/*****************************************************************************************
   LOCAL VARIABLES
*/

static volatile uint16_t rawSensVal;
static volatile uint16_t rawBattVal;

// Meaning queue:
static meanType_t mean1mTab[MEAN_1M_QUEUE_LEN];
static meanType_t mean1hTab[MEAN_1H_QUEUE_LEN];
static meanType_t mean8hTab[MEAN_8H_QUEUE_LEN];

// Tables for static queue allocation:
static meanQueue_t mean1mQ = { mean1mTab, mean1mTab, MEAN_1M_QUEUE_LEN, false };
static meanQueue_t mean1hQ = { mean1hTab, mean1hTab, MEAN_1H_QUEUE_LEN, false };
static meanQueue_t mean8hQ = { mean8hTab, mean8hTab, MEAN_8H_QUEUE_LEN, false };

// Flags:
static bool vBattFlag = TRUE;    // For adc measurement triggering

// Structure with values to display on LCD:
static valsToDisp_t locVals;


/*****************************************************************************************
   LOCAL FUNCTIONS DECLARATIONS
*/


static void systemPeriodicRefresh ( void );

static void systemQueuePush (  meanQueue_t* pQueue, meanType_t val );
static void systemQueueCalcMean ( meanQueue_t* pQueue, meanType_t* buf );

static uint16_t systemConvRawSens ( uint16_t raw );
static uint16_t systemConvRawBatt ( uint16_t raw );





/*****************************************************************************************
   LOCAL FUNCTIONS DEFINITIONS
*/

//****************************************************************************************
// Periodic refresh (from RTC):
static void systemPeriodicRefresh ( void )
{   
   static uint16_t ticks = 0;   
   
#ifdef STAT_LED_ON_ADC
   ioStatLedOn();          // Turning on status LED (blink driven by ADC measuring time)
#endif
   
   adcStartChannel (SENS);      // Starting sensor voltage measurement


   {
      // Every tick: // TODO: maybe another manner?

      
      locVals.actSensVal = systemConvRawSens ( rawSensVal );     // Converting from raw (16b) value from ADC to [ppm] or actually [mV]    
      systemQueuePush ( &mean1mQ, locVals.actSensVal );
      systemQueueCalcMean ( &mean1mQ, &locVals.mean1mVal );     // For 1min meaning
    
      // Every 15 s:   
      if ( 0 == (ticks % 15) )     
      {
         systemQueuePush ( &mean1hQ, locVals.mean1mVal );
         systemQueueCalcMean ( &mean1hQ, &locVals.mean1hVal );  // For 1h meaning
         locVals.actBattVal = systemConvRawBatt ( rawBattVal );
         vBattFlag = TRUE; // Setting this flag enables vbatt meas. after sens meas.   
      }
   
      // Every 5 min :
      if ( 0 == (ticks % 300)  )    
      {
         systemQueuePush ( &mean8hQ, locVals.mean1hVal );
         systemQueueCalcMean ( &mean8hQ, &locVals.mean8hVal );  // For 8h meaning 
         ticks = 0;
      }
   }   
 
   interTimeTickUpdate();
   interDisplaySystemVals ( &locVals );
   
   ticks += RTC_PERIOD_S;
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
// Converting from raw sensor value
static uint16_t systemConvRawSens ( uint16_t raw )
{
  uint32_t rawData = (((uint32_t)raw) * ADC_SENS_MULTI_MV) / 65535; // for 1 [mV] resolution @16b 
   
  return (uint16_t)rawData;
}

//****************************************************************************************
// Converting from raw vBatt value
static uint16_t systemConvRawBatt ( uint16_t raw )
{
   uint32_t rawData = (((uint32_t)raw) * ADC_VBATT_MULTI_MV) / 65535; // for 1 [mV] resolution @16b
   
   return (uint16_t)rawData;
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
   
   adcStartChannel (SENS);   // Initial measurements
   
   LOG_TXT ( ">>init<<   System initialized\n" );
}

//****************************************************************************************
// After changing USB connection state:

void systemWakeUp ( void )
{
      
}


void systemUSBStateChanged ( void )
{
   _delay_ms(30);  // Glitch
   
   if ( IO_GET_USB_CONN() )   // If USB plugged in
   {
      locVals.lpFlag = FALSE;
      locVals.usbPlugged = TRUE;
      IO_FALLING_EDGE_USB();  // Falling edge sense - now pin state is high
   }
   else
   {
      locVals.lpFlag = TRUE;
      locVals.usbPlugged = FALSE;
      IO_RISING_EDGE_USB();  // Rising edge sense - now pin state is low
   }
}


//****************************************************************************************
void systemMeasEnd ( uint16_t val )
{
   
   if ( SENS == adcGetChan() )
   {
      rawSensVal = val;
      if ( vBattFlag )
      {          
         adcStartChannel ( VBATT ); // Battery voltage measurement start
         vBattFlag = FALSE;
      }         
   }
   else if ( VBATT == adcGetChan() )
   {
      rawBattVal = val;      
   }
   

#ifdef STAT_LED_ON_ADC   
   ioStatLedOff();      // State LED blinking
#endif
}  