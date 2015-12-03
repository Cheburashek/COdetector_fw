/*
 * system.c
 *
 * Created: 2015-10-31 21:02:07
 *  Author: Chebu
 */ 

/*****************************************************************************************
   LOCAL INCLUDES
*/

#include "system.h"


/*****************************************************************************************
   MACROS
*/

/*****************************************************************************************
   LOCAL DEFINITIONS
*/

#define LCD_ACTMEAS_POS_Y          1

#define MEAN_1M_QUEUE_LEN          12       // For 5 seconds * 1m  
#define MEAN_1H_QUEUE_LEN          60       // For 1 minute * 1 hour
#define MEAN_8H_QUEUE_LEN          480      // For 1 minute * 8 hours


/*****************************************************************************************
   LOCAL TYPEDEFS
*/

// Structure describing time:
typedef struct
{
   uint16_t year;
   uint8_t  month;
   uint8_t  day;
   uint8_t  hour;
   uint8_t  min;
   uint8_t  sec;
   
} timeStruct_t;

typedef uint16_t meanQueueType_t;

typedef struct
{
   meanQueueType_t* pStart;
   meanQueueType_t* pHead;
   uint16_t len;
   bool firstOF;   
   
} meanQueue_t; 



/*****************************************************************************************
   LOCAL VARIABLES
*/

static volatile uint16_t rawVal;
static uint16_t actVal = 0;   // in 100[uV]
static timeStruct_t sysTime;

// Meaning queue:
static meanQueue_t mean1mQ;
static meanQueue_t mean1hQ;
static meanQueue_t mean8hQ;

/*****************************************************************************************
   LOCAL FUNCTIONS DECLARATIONS
*/


static void systemPeriodicRefresh ( void );

static void systemQueueInit (  meanQueue_t* pQueue, uint16_t len );
static void systemQueuePush (  meanQueue_t* pQueue, meanQueueType_t val );

static uint16_t systemConvertFromRaw ( uint16_t raw );
static void systemDisplayVals ( void );
static void systemDisplayBackground ( void );
static void systemTimeTickUpdate ( void );

static void systemSerialLog ( void );

/*****************************************************************************************
   LOCAL FUNCTIONS DEFINITIONS
*/

static void systemPeriodicRefresh ( void )
{   

   
   actVal = systemConvertFromRaw ( rawVal );
     
   systemTimeTickUpdate();
   systemSerialLog();  
   systemDisplayVals();
   
   adcStartChToGnd();
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
      LOG_TXT ( "Cannot allocate queue!" );
      while(1){;}      
   }   
}

//****************************************************************************************
// Adding value to queue:
static void systemQueuePush (  meanQueue_t* pQueue, meanQueueType_t val )
{  
   *pQueue->pHead = val;
   
   if ( pQueue->pHead < (pQueue->pStart + (pQueue->len/sizeof(meanQueueType_t))) )
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
static void systemQueueCalcMean ( pQueue )
{
   
}








//****************************************************************************************
static uint16_t systemConvertFromRaw ( uint16_t raw )
{
  uint32_t rawData = (((uint32_t)raw) * ADC_DIVIDER) / 65535; // for 1 [mV] resolution @16b 
   
  return (uint16_t)rawData;
}


//****************************************************************************************
static void systemDisplayVals ( void )
{
   
   // Time:
   char str[14] = {"              "};
   sprintf ( str, "%.2u:%.2u:%.2u", sysTime.hour, sysTime.min, sysTime.sec );
   pdcLine( str, 0 );
   
   // Measured values:
   pdcUint ( actVal, LCD_ACTMEAS_POS_Y, 6, 5 );
}

//****************************************************************************************
static void systemDisplayBackground ( void )
{
   pdcLine ( "Act:        mV", LCD_ACTMEAS_POS_Y );

}

//****************************************************************************************
static void systemTimeTickUpdate ( void )
{
   sysTime.sec ++;
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
      
   // TODO: days, months...
}



//****************************************************************************************
static void systemSerialLog ( void )
{
   char strToLog [64];
   uint8_t len =  sprintf ( strToLog, "[%.2u:%.2u:%.2u] %.4u[mV] \n", sysTime.hour, sysTime.min, sysTime.sec, actVal );
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
   
   adcRegisterEndCb( systemMeasEnd );      // Registering CB
   timerRegisterRtcCB ( systemPeriodicRefresh );
   
   systemDisplayBackground();
   
   LOG_TXT ( ">>init<<   System initialized\n" );
}

//****************************************************************************************


//****************************************************************************************
void systemMeasEnd ( uint16_t val )
{
   rawVal = val;   // For 16b res     
}