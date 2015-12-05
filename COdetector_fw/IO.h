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
   GLOBAL DEFINITIONS & MACROS
*/



/*****************************************************************************************
   GLOBAL FUNCTIONS DECLARATIONS
*/

void ioInit ( void );

void ioBuzzerOn ( void );
void ioBuzzerOff ( void );
void ioBuzzerTgl ( void );

void ioStatLedOn ( void );
void ioStatLedOff ( void );
void ioStatLedTgl ( void );

#endif /* IO_H_ */