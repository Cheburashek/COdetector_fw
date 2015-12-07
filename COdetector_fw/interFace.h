/*
 * interFace.h
 *
 * Created: 2015-12-05 23:31:37
 *  Author: Chebu
 */ 


#ifndef INTERFACE_H_
#define INTERFACE_H_

/*****************************************************************************************
   GLOBAL INCLUDES
*/

#include "common.h"
#include "system.h"


/*****************************************************************************************
   GLOBAL DEFINITIONS
*/

/*****************************************************************************************
   GLOBAL TYPEDEFS
*/

typedef struct
{
   uint16_t year;
   uint8_t  month;
   uint8_t  day;
   uint8_t  hour;
   uint8_t  min;
   uint8_t  sec;
   
} timeStruct_t;



// States:
typedef enum
{
   UNKNOWN_M_STATE,
   HELLO_M_STATE = 0x10,
   DISPVALS_M_STATE,
   CONFIG_M_STATE,
   TIME_SET_M_STATE,
   INFO_M_STATE,
   ALARM_TEST_M_STATE,
   ALARM_M_STATE 
   
} eMainState_t;


typedef enum
{
   TIME_O_POS = 0x01,    // Also position on LCD
   EXIT_O_POS = 0x04
   
} eOptionsState_t;

// Time:
typedef enum
{
   YEAR_POS,
   MONTH_POS,
   DAY_POS,    // Also position on LCD  
   HOUR_POS,
   MIN_POS,
   EXIT_T_POS
   
} eTimeSetState_t;

// Buttons:
typedef enum
{
   BT_NULL,
   BT_LEFT = CFG_BT3_PIN_MASK,
   BT_OK = CFG_BT2_PIN_MASK,   
   BT_RIGHT = CFG_BT1_PIN_MASK
   
} eButtons_t;

/*****************************************************************************************
   GLOBAL FUNCTIONS DECLARATIONS
*/

void interInit ( void );
void interDisplaySystemVals ( valsToDisp_t* pVal );

void interTimeTickUpdate ( void );

void interOnRight ( void );
void interOnOk ( void );
void interOnLeft ( void );

#endif /* INTERFACE_H_ */