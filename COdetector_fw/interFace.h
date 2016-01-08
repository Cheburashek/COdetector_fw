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

// Alarm stages:
typedef enum
{
   ALARM_STAGE_1M,
   ALARM_STAGE_15M,
   ALARM_STAGE_1H,
   ALARM_STAGE_2H,
   NO_ALARM_STAGE = 0xFF
   
} eAlarmStages_t;

typedef struct
{
   uint8_t  hour;
   uint8_t  min;
   uint8_t  sec;
     
} timeStruct_t;

typedef struct
{
   timeStruct_t laTime;
   eAlarmStages_t laStage;
      
} lastAlarm_t;

typedef struct
{
   timeStruct_t mvTime;
   measType_t mvVal;
   
} maxVal_t;

// States:
typedef enum
{
   UNKNOWN_M_STATE,
   HELLO_M_STATE = 0x10,
   DISPVALS_M_STATE,
   SLEEP_M_STATE,
   CONFIG_M_STATE,
   TIME_SET_M_STATE,
   INFO_M_STATE,  
   ALARM_M_STATE,
   SENS_CODE_STATE 
   
} eMainState_t;


typedef enum
{
   TIME_O_POS = 0x01,    // Also position on LCD
   SENS_CODE_O_POS = 0x02,
   ALARM_O_POS = 0x03,
   EXIT_O_POS = 0x04
   
} eOptionsState_t;

// Time:
typedef enum
{
   HOUR_POS,
   MIN_POS,
   EXIT_T_POS = 0x04
   
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
void interMainStateMachineSet ( eMainState_t state );
void interDisplaySystemVals ( valsToDisp_t* pVal );
void interTimeTickUpdate ( uint8_t rtcPer );
void interOnRight ( void );
void interOnOk ( void );
void interOnLeft ( void );
void interAlarmStage ( eAlarmStages_t stage );
bool interIsSleepPerm ( void );
void interSetSleepPerm ( void );


#endif /* INTERFACE_H_ */