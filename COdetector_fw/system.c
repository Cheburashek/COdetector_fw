/*
 * system.c
 *
 * Created: 2015-10-31 21:02:07
 *  Author: Chebu
 */ 

// Najpierw uruchamiam pomiar (20ms), aby jak najkrócej by³ wzbudzony uk³ad. 

/*****************************************************************************************
   LOCAL INCLUDES
*/

#include "system.h"
#include "timers.h"
#include "ADC.h"
#include "interFace.h"
#include "IO.h"
#include "oneWire.h"

/*****************************************************************************************
   MACROS
*/

/*****************************************************************************************
   LOCAL DEFINITIONS
*/

// Lengths of meaning queues:
#define MEAN_15S_QUEUE_LEN         15/RTC_PER_BATT         // Meaning at every tick (RTC_PERIOD_S) for 15-sec
#define MEAN_1M_QUEUE_LEN          60/RTC_PER_BATT         // Meaning at every tick (RTC_PERIOD_S) for 1-minute 
#define MEAN_15M_QUEUE_LEN         60                      // Meaning at every 15s for 15-min   
#define MEAN_1H_QUEUE_LEN          60                      // Meaning at every min for 1-hour           
#define MEAN_2H_QUEUE_LEN          120                     // Meaning at every min for 2-hour


/*****************************************************************************************
   LOCAL TYPEDEFS
*/

/*****************************************************************************************
   LOCAL CONSTS
*/
// Temperature Compensation Coefficients ( start with -40C):
static const uint16_t compCoefTab[] = {
   453, 463, 473, 483, 493, 503, 523, 513, 534, 544, 554, 564, 574, 584, 594, 605,
   615, 625, 635, 645, 655, 664, 674, 684, 694, 704, 714, 723, 733, 742, 752, 761, 
   771, 780, 789, 799, 808, 817, 826, 835, 
   844, 582, 861, 870, 878, 887, 895, 903, 911, 919, 927, 935, 943, 950, 958, 965, // 0-15
   972, 980, 987, 994, 1000, 1007, 1013, 1020, 1026, 1032, 1038, 1044, 1050, 1055,
   1060, 1066, 1071, 1076, 1080, 1085, 1089, 1094, 1098, 1101,
   1105, 1109, 1112, 1115, 1118, 1121, 1124, 1126, 1128, 1130, 1132, 1134, 1135, 1136, // 40
   1137, 1138, 1139 // end: 56 -> 56-70 is 1139   
};

/*****************************************************************************************
   LOCAL VARIABLES
*/

static uint8_t rtcActPer = RTC_PER_USB;

static volatile uint16_t rawBattVal;
static volatile uint16_t rawSensVal;
static volatile uint16_t DStempVal;

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
static uint8_t systemConvRawBattPercent ( uint16_t raw );
static int8_t systemConvTemp ( uint16_t raw );

static void systemQueueReset (  meanQueue_t* pQueue );


/*****************************************************************************************
   LOCAL FUNCTIONS DEFINITIONS
*/

//****************************************************************************************
// Periodic refresh (from RTC):
static void systemPeriodicRefresh ( void )
{   
   static uint16_t ticks = 0;   
   static bool tempMeasStage = FALSE;

   adcStartChannel (SENS);      // Starting sensor voltage measurement

   // Every tick: 
      
   locVals.actSensVal = systemConvRawSens ( rawSensVal );     // Converting from raw (16b) value from ADC to [ppm] or actually [mV]    
   
   systemQueuePush ( &mean15sQ, locVals.actSensVal );  
   systemQueuePush ( &mean1mQ, locVals.actSensVal );  // TODO: 1min from 15s?
   
   systemQueueCalcMean ( &mean15sQ, &locVals.mean15sVal );    // For 15sn meaning
   systemQueueCalcMean ( &mean1mQ, &locVals.mean1mVal );      // For 1min meaning
        
   #ifdef TEMP_MEAS_PERM
   if ( tempMeasStage )
   {
      oneWireConvStart ();
      tempMeasStage = FALSE;
   }
   else
   {
      locVals.tempC = systemConvTemp ( oneWireReadTemp () );
      tempMeasStage = TRUE;
   }
   #endif   
   
   // Every 15s:
   if ( 0 == (ticks % 15) )
   {                  
      systemQueuePush ( &mean15mQ, locVals.actSensVal );
      systemQueueCalcMean ( &mean15mQ, &locVals.mean15mVal );     // For 1min meaning
      
    //  if ( !locVals.usbPlugged ) // If USB is not plugged in
      {
          
         locVals.battPer = systemConvRawBattPercent ( rawBattVal );
         vBattFlag = TRUE; // Setting this flag enables vbatt meas. after sens meas.           
      }            

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
       
   interTimeTickUpdate( rtcActPer );
   ticks += rtcActPer;
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
   uint8_t compIndex = 0;   
   uint32_t temp = (((uint32_t)raw) * ADC_SENS_MULTI_MV) / 65535; // for 1 [mV] resolution @16b 
   
   // Temperature compensation:
   if ( locVals.tempC >= 56 ) compIndex = 56;
   else compIndex = TEMP_COMP_TAB_OFFSET_0C + locVals.tempC;
   
   if ( SENS_OFFSET_MV < temp )
   {
      temp = ((( (temp - SENS_OFFSET_MV) * SENS_NA_MV_MULTI_1k)/1000) * compCoefTab[compIndex-1] ) / sensCodeNaPpm;     // for [ppm]
   }
   else    // In case of glitch
   {
      temp = 0;
   }   
     
   return (uint16_t)temp;
}

//****************************************************************************************
// Converting from raw vBatt value
static uint8_t systemConvRawBattPercent ( uint16_t raw )
{
   uint32_t temp = (((uint32_t)raw) * ADC_VBATT_MULTI_MV) / 65535; // for 1 [mV] resolution @16b
   
   if ( temp >= BATTERY_MIN_VOLTAGE )
   {
      temp -= BATTERY_MIN_VOLTAGE;
      temp = (temp*100) / (BATTERY_MAX_VOLTAGE-BATTERY_MIN_VOLTAGE);
      if ( temp > 100 ) temp = 100;
   }   
   else temp = 0;

   return temp;
}

//****************************************************************************************
// Converting from raw value from DS18B20 to C degrees
static int8_t systemConvTemp ( uint16_t raw )
{
   int8_t temp = 0x00;
   
   temp = raw >> 4;
   if ( raw & 0x1000 ) temp*= -1;   // If 13th bit is set -> negative
   
   return temp;
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
   systemResetMeasRes ();   // Clear all buffers   
  
#ifdef TEMP_MEAS_PERM
   // Initial temperature measurement:
   oneWireConvStart();
   _delay_ms(750);
   locVals.tempC = systemConvTemp ( oneWireReadTemp () );
#else
   locVals.tempC = 25;  // Default temp
#endif
   
   
   systemUSBStateChanged();   // Initial check of USB state
   adcRegisterEndCb( systemMeasEnd );      // Registering CB
   timerRegisterRtcCB ( systemPeriodicRefresh );
   adcStartChannel (VBATT);      // Starting sensor voltage measurement   
   _delay_ms(100);
   locVals.battPer = systemConvRawBattPercent ( rawBattVal );
   systemPeriodicRefresh();
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
   
   if ( SENS == ADC_GET_CH() )
   {      
      rawSensVal = val;
      
      if ( vBattFlag )
      {          
         adcStartChannel ( VBATT ); // Battery voltage measurement start
         vBattFlag = FALSE;               
      }             
      //LOG_UINT ( "raw sens ", val );  
   }
   else if ( VBATT == ADC_GET_CH() )
   {
      rawBattVal = val;
      LOG_UINT ( "raw bat ", val );        
   }   
}  