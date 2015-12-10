/*
 * ADC.h
 *
 * Created: 2015-10-10 01:15:49
 *  Author: Chebu
 */ 


#ifndef ADC_H_
#define ADC_H_

/*****************************************************************************************
   GLOBAL INCLUDES
*/

#include "common.h"

/*****************************************************************************************
   GLOBAL DEFINITIONS
*/
#define ADC_START()        ( ADCA.CTRLA |= ADC_START_bm   )       // Start single conversion
#define ADC_EN()           ( ADCA.CTRLA |= ADC_ENABLE_bm  )
#define ADC_DIS()          ( ADCA.CTRLA &= ~ADC_ENABLE_bm )

/*****************************************************************************************
   GLOBAL TYPEDEFS
*/

typedef void (*pfnAdcEnd) (uint16_t); 

/*****************************************************************************************
   GLOBAL FUNCTIONS DECLARATIONS
*/

void adcRegisterEndCb( pfnAdcEnd cb );

void adcInit ( void );


#endif /* ADC_H_ */