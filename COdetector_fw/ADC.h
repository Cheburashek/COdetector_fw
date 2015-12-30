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



#define ADC_EN()             PR.PRPA &= ~PR_ADC_bm;\
                             ADCA.CTRLA |= ADC_ENABLE_bm;
                               
                             
#define ADC_DIS()            ADCA.CTRLA &= ~ADC_ENABLE_bm;\
                             PR.PRPA |= PR_ADC_bm;      

/*****************************************************************************************
   GLOBAL TYPEDEFS
*/

typedef enum
{   
   SENS = CFG_ADC_SENS_MUXPOS,
   VBATT = CFG_ADC_VBATT_MUXPOS,
   TEMP = 0xFF,  
   
} eAdcChan_t;

typedef void (*pfnAdcEnd) (uint16_t); 

/*****************************************************************************************
   GLOBAL FUNCTIONS DECLARATIONS
*/

void adcRegisterEndCb( pfnAdcEnd cb );
void adcStartChannel ( eAdcChan_t ch );
eAdcChan_t adcGetChan ( void );
void adcLowPowerClock ( bool mode );
void adcInit ( void );


#endif /* ADC_H_ */