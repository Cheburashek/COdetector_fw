/*
 * IO.h
 *
 * Created: 2015-12-04 01:30:01
 *  Author: Chebu
 */ 


#ifndef IO_H_
#define IO_H_


/*****************************************************************************************
   GLOBAL INCLUDES
*/
#include "common.h"

/*****************************************************************************************
   GLOBAL DEFINITIONS
*/

/*****************************************************************************************
   GLOBAL FUNCTIONS DECLARATIONS
*/

void ioInit ( void );
void ioBuzzer ( bool stat );
void ioStatLED ( bool stat );


#endif /* IO_H_ */