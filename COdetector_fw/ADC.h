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

/*****************************************************************************************
   GLOBAL TYPEDEFS
*/

typedef void (*pfnAdcEnd) (uint16_t); 

/*****************************************************************************************
   GLOBAL FUNCTIONS DECLARATIONS
*/

void adcRegisterEndCb( pfnAdcEnd cb );
void adcStartSens ( void );
void adcInit ( void );


#endif /* ADC_H_ */