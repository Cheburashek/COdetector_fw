/*
 * boardCfg.h
 *
 * Created: 2015-10-02 21:57:29
 *  Author: Chebu
 */ 


#ifndef BOARDCFG_H_
#define BOARDCFG_H_



/*****************************************************************************************
   GLOBAL INCLUDES
*/

#include "common.h"



/*****************************************************************************************
   GLOBAL CONFIGURATION DEFINES
*/

// UARTC0 pins ( PORTC ):
#define CFG_TXC0_PIN_MASK        PIN3_bm  
#define CFG_RXC0_PIN_MASK        PIN2_bm

// SPI pins (for lcd3310) ( PORTC ):
#define CFG_RST_PIN_MASK         PIN0_bm 
#define CFG_SCE_PIN_MASK         PIN1_bm
#define CFG_DC_PIN_MASK          PIN4_bm
#define CFG_SCK_PIN_MASK         PIN5_bm
#define CFG_MOSI_PIN_MASK        PIN7_bm

// ADC pins (PORTA):
#define CFG_ADC_PIN_MASK         PIN0_bm
#define CFG_ADC_MUXPOS           ADC_CH_MUXPOS_PIN0_gc

// IO:   (PORTA)
#define CFG_PULSE_PIN_MASK       PIN1_bm

// Timer:


/*** Interrupts priority ****/ 
// TODO: there should be all interrupts prioritized:

/* Data Register Empty Interrupt Level
 * Transmit Complete Interrupt level
 * Receive Complete Interrupt level */
#define CFG_PRIO_USARTC0    ( USART_DREINTLVL_OFF_gc | USART_TXCINTLVL_MED_gc | USART_RXCINTLVL_OFF_gc )    

// SPI intlvl:
#define CFG_PRIO_SPI         SPI_INTLVL_MED_gc // not working on HI int - why?

// ADC intlvl:
#define CFG_PRIO_ADC         ADC_CH_INTLVL_MED_gc

// Timer intlvl:
//#define CFG_PRIO_TC4         TC45_OVFINTLVL_HI_gc
#define CFG_PRIO_TC4_CCALVL  TC45_CCAINTLVL_HI_gc
#define CFG_PRIO_TC4_CCBLVL  TC45_CCBINTLVL_HI_gc
#define CFG_PRIO_TC4_CCCLVL  TC45_CCCINTLVL_HI_gc



/*****************************************************************************************
   GLOBAL MACROS AND DEFINITIONS
*/

//****************************************************************************************
#define CFG_GLOBAL_INT_ENABLE()     ( CPU_SREG |= CPU_I_bm ) 
#define CFG_GLOBAL_INT_DISABLE()    ( CPU_SREG &= ~CPU_I_bm ) 


//****************************************************************************************
// PMIC interrupts by levels configuring:

#define PRIO_LOW_LEVEL_ENABLE()      ( PMIC_CTRL |= PMIC_LOLVLEN_bm  )
#define PRIO_MED_LEVEL_ENABLE()      ( PMIC_CTRL |= PMIC_MEDLVLEN_bm )
#define PRIO_HI_LEVEL_ENABLE()       ( PMIC_CTRL |= PMIC_HILVLEN_bm  )
#define PRIO_ALL_LEVELS_ENABLE()     ( PMIC_CTRL |= 0x07             )

#define PRIO_LOW_LEVEL_DISABLE()      ( PMIC_CTRL &= ~PMIC_LOLVLEN_bm  )
#define PRIO_MED_LEVEL_DISABLE()      ( PMIC_CTRL &= ~PMIC_MEDLVLEN_bm )
#define PRIO_HI_LEVEL_DISABLE()       ( PMIC_CTRL &= ~PMIC_HILVLEN_bm  )
#define PRIO_ALL_LEVELS_DISABLE()     ( PMIC_CTRL &= ~0x07             )

#define DEB_1_SET()     PORTD.OUTSET = PIN5_bm
#define DEB_2_SET()     PORTD.OUTSET = PIN6_bm
#define DEB_3_SET()     PORTD.OUTSET = PIN7_bm

#define DEB_1_CLR()     PORTD.OUTCLR = PIN5_bm
#define DEB_2_CLR()     PORTD.OUTCLR = PIN6_bm
#define DEB_3_CLR()     PORTD.OUTCLR = PIN7_bm

#define DEB_1_TGL()     PORTD.OUTTGL = PIN5_bm
#define DEB_2_TGL()     PORTD.OUTTGL = PIN6_bm
#define DEB_3_TGL()     PORTD.OUTTGL = PIN7_bm

// IO pulse output:

#define PULSE_SET()     PORTA.OUTSET = CFG_PULSE_PIN_MASK
#define PULSE_CLR()     PORTA.OUTCLR = CFG_PULSE_PIN_MASK

/*****************************************************************************************
   GLOBAL DEFINES
*/


/*****************************************************************************************
   GLOBAL FUNCTIONS DECLARATIONS
*/
void boardInit ( void );


#endif /* BOARDCFG_H_ */