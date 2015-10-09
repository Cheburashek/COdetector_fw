/*
 * SPI.h
 *
 * Created: 2015-10-06 21:11:01
 *  Author: Chebu
 */ 


#ifndef SPI_H_
#define SPI_H_

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

typedef void (*pfnTxEnd) (void);

/*****************************************************************************************
   GLOBAL MACROS
*/

#define SPI_EN()    ( SPIC.CTRL |= SPI_ENABLE_bm  )
#define SPI_DIS()   ( SPIC.CTRL &= ~SPI_ENABLE_bm )
#define SPI_TX

/*****************************************************************************************
   GLOBAL FUNCTIONS DECLARATIONS
*/

void spiInit ( void );

void spiSend ( const uint8_t* data, uint16_t len );

void spiTxEndCB ( pfnTxEnd cb);

#endif /* SPI_H_ */