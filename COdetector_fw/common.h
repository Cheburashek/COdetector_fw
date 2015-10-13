/*
 * common.h
 *
 * Created: 2015-10-02 23:23:12
 *  Author: Chebu
 */ 


#ifndef COMMON_H_
#define COMMON_H_


/*****************************************************************************************
   GLOBAL INCLUDES
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include "boardCfg.h"
#include "serial.h"
#include "SPI.h"
#include "PDC8544.h"
#include "ADC.h"

 
/*****************************************************************************************
   GLOBAL DEFINITIONS
*/

#define FALSE  0
#define TRUE   1


/*****************************************************************************************
   GLOBAL MACROS
*/   

 #define DEB_1_SET()     PORTD.OUTSET = PIN5_bm
 #define DEB_2_SET()     PORTD.OUTSET = PIN6_bm
 #define DEB_3_SET()     PORTD.OUTSET = PIN7_bm

 #define DEB_1_CLR()     PORTD.OUTCLR = PIN5_bm
 #define DEB_2_CLR()     PORTD.OUTCLR = PIN6_bm
 #define DEB_3_CLR()     PORTD.OUTCLR = PIN7_bm
 
 #define DEB_1_TGL()     PORTD.OUTTGL = PIN5_bm
 #define DEB_2_TGL()     PORTD.OUTTGL = PIN6_bm
 #define DEB_3_TGL()     PORTD.OUTTGL = PIN7_bm
 
#endif /* COMMON_H_ */