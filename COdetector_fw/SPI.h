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

#define ENHANCED_SPI

/*****************************************************************************************
   GLOBAL TYPEDEFS
*/

typedef void (*pfnTxEnd) (void);

// Struct with data to send and DC bit:
typedef struct 
{
   uint8_t data;
   bool outDC;
   
} spiEnhStruct_t;


/*****************************************************************************************
   GLOBAL MACROS
*/

#define SPI_EN()      PR.PRPC &= ~PR_SPI_bm;\
                      SPIC.CTRL |= SPI_ENABLE_bm;
                      
                      
#define SPI_DIS()     SPIC.CTRL &= ~SPI_ENABLE_bm;\
                      PR.PRPC |= PR_SPI_bm;

/*****************************************************************************************
   GLOBAL FUNCTIONS DECLARATIONS
*/

void spiInit ( void );

void spiSend ( spiEnhStruct_t* dataStr );

#endif /* SPI_H_ */