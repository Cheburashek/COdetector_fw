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

/*****************************************************************************************
   LOCAL VARIABLES
*/

static volatile uint16_t actMeas;


/*****************************************************************************************
   LOCAL FUNCTIONS DECLARATIONS
*/

static void systemPeriodicRefresh ( void );

static void systemDisplayBackground ( void );


/*****************************************************************************************
   LOCAL FUNCTIONS DEFINITIONS
*/

static void systemPeriodicRefresh ( void )
{   
   static uint16_t actMeasOld;
   
   if ( actMeasOld != actMeas )
   {
      actMeasOld = actMeas;
      pdcUint( actMeas, 1, 5, 7 );

   }
   
   LOG_UINT ( "Result [mV]: ", 13, actMeas );
}


//****************************************************************************************
static void systemDisplayVals ( void )
{

    
}

//****************************************************************************************
static void systemDisplayBackground ( void )
{
   pdcLine ( "Act:        mV", 1 );

}




/*****************************************************************************************
   GLOBAL FUNCTIONS DEFINITIONS
*/


//****************************************************************************************
void systemInit ( void ) 
{
   adcRegisterEndCb( systemMeasEnd );      // Registering CB
   timerRegisterRtcCB ( adcStartChToGnd );
   
   systemDisplayBackground();
   
   LOG_TXT ( ">>init<<   System initialized\n", 31 );
}




//****************************************************************************************
void systemMeasEnd ( uint16_t val )
{
   actMeas = ((((uint32_t)val)*ADC_DIVIDER)/65535);   // For 16b res
   
   systemPeriodicRefresh();
}