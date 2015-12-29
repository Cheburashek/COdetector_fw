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

#define IO_GET_USB_CONN()        ( (PORTD.IN & CFG_USB_CON_PIN_MASK) == CFG_USB_CON_PIN_MASK )
#define IO_RISING_EDGE_USB()     (  PORTD.PIN4CTRL |= PORT_ISC_RISING_gc )
#define IO_FALLING_EDGE_USB()    (  PORTD.PIN4CTRL |= PORT_ISC_FALLING_gc )

#define GET_BT_RIGHT_STATE()       BT_RIGHT & ~PORTD.IN
#define GET_BT_LEFT_STATE()        BT_LEFT & ~PORTD.IN
#define GET_BT_OK_STATE()          BT_OK & ~PORTD.IN
   

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

void ioBcklghtOn ( void );
void ioBcklghtOff ( void );

#endif /* IO_H_ */