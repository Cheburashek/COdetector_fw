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

/*****************************************************************************************
   MACROS
*/

/*****************************************************************************************
   LOCAL DEFINITIONS
*/

// Positions (Y) on LCD:
#define LCD_ACTMEAS_POS_Y          1
#define LCD_MEAN_1M_POS_Y          2
#define LCD_MEAN_1H_POS_Y          3
#define LCD_MEAN_8H_POS_Y          4

/*****************************************************************************************
   LOCAL VARIABLES
*/

/*****************************************************************************************
   LOCAL FUNCTIONS DECLARATIONS
*/


static void ifDisplayBackground ( void );

/*****************************************************************************************
   LOCAL FUNCTIONS DEFINITIONS
*/

void ifInit ( void )
{
   ifDisplayBackground ();
   
   LOG_TXT ( ">>init<<   Timers initialized\n" );    
}

//****************************************************************************************


//****************************************************************************************
static void ifDisplayBackground ( void )
{
   pdcLine ( "Act:        mV", LCD_ACTMEAS_POS_Y );
   pdcLine ( "M1m:        mV", LCD_MEAN_1M_POS_Y );
   pdcLine ( "M1h:        mV", LCD_MEAN_1H_POS_Y );
   pdcLine ( "M8h:        mV", LCD_MEAN_8H_POS_Y );

}

/*****************************************************************************************
   GLOBAL FUNCTIONS DEFINITIONS
*/

void ifDisplaySystemVals ( valsToDisp_t* pVal )
{
   
   // Time:
   char str[14] = {"              "};
   sprintf ( str, "%.2u:%.2u:%.2u", pVal->sysTime.hour, pVal->sysTime.min, pVal->sysTime.sec );
   pdcLine( str, 0 );
   
   // Measured values:
   pdcUint ( pVal->actVal, LCD_ACTMEAS_POS_Y, 6, 5 );
   pdcUint ( pVal->mean1mVal, LCD_MEAN_1M_POS_Y, 6, 5 );
   pdcUint ( pVal->mean1hVal, LCD_MEAN_1H_POS_Y, 6, 5 );
   pdcUint ( pVal->mean8hVal, LCD_MEAN_8H_POS_Y, 6, 5 );
   
}