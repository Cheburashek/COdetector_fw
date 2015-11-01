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

#define LCD_ACTMEAS_POS_Y 1


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


/*****************************************************************************************
   LOCAL VARIABLES
*/

static volatile uint16_t actMeas;
static timeStruct_t sysTime;




/*****************************************************************************************
   LOCAL FUNCTIONS DECLARATIONS
*/

static void systemPeriodicRefresh ( void );

static void systemDisplayVals ( void );

static void systemDisplayBackground ( void );

static void systemTimeTickUpdate ( void );

static void systemSerialLog ( void );

/*****************************************************************************************
   LOCAL FUNCTIONS DEFINITIONS
*/

static void systemPeriodicRefresh ( void )
{   

   

     
   systemTimeTickUpdate();
   systemSerialLog();  
   systemDisplayVals();
   
   adcStartChToGnd();
}


//****************************************************************************************
static void systemDisplayVals ( void )
{
   
   // Time:
   char str[14] = {"              "};
   sprintf ( str, "%.2u:%.2u:%.2u", sysTime.hour, sysTime.min, sysTime.sec );
   pdcLine( str, 0 );
   
   // Measured values:
   pdcUint ( actMeas, LCD_ACTMEAS_POS_Y, 6, 5 );
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
   uint8_t len =  sprintf ( strToLog, "[%.2u:%.2u:%.2u] %.4u[mV] \n", sysTime.hour, sysTime.min, sysTime.sec, actMeas );
   LOG_TXT ( strToLog, len );
} 




/*****************************************************************************************
   GLOBAL FUNCTIONS DEFINITIONS
*/


//****************************************************************************************
void systemInit ( void ) 
{
   adcRegisterEndCb( systemMeasEnd );      // Registering CB
   timerRegisterRtcCB ( systemPeriodicRefresh );
   
   systemDisplayBackground();
   
   LOG_TXT ( ">>init<<   System initialized\n", 31 );
}




//****************************************************************************************
void systemMeasEnd ( uint16_t val )
{
   actMeas = ((((uint32_t)val)*ADC_DIVIDER)/65535);   // For 16b res  
}