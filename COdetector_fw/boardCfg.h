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
   GLOBAL CONSTANTS
*/


#define RTC_PERIOD_S           1        // Should be 1,3,5 or 15 [s]
#define ADC_SENS_MULTI_MV      3500     // By input voltage divider ( 1:1 -> 1000 )
#define ADC_VBATT_MULTI_MV     5652

#define SENS_NA_MV_MULTI_1k    934      // nA/mV multiplier x1000  -> circuit   
#define SENS_NA_PPM_MULTI_1k   1594     // nA/ppm multiplier x1000  -> sensor  
#define SENS_OFFSET_MV         15       // offset in mV at ADC input (0ppm)

// Controlling:
#define xHELLO_SCREEN_PERM
#define STAT_LED_ON_TICK_US    1200      // [us] of LED on
#define BUZZER_ON_BT_US        700      // [us] of buzzer on button
#define ALARM_PERM

#define ALARM_PERIOD_HI    700          // ms period of alarm

// Tresholds:
#define TRESH_1M_PPM          300    
#define TRESH_15M_PPM         100
#define TRESH_1H_PPM          50
#define TRESH_2H_PPM          30
#define TRESH_ALARM_OFF_PPM   50

#define TRESH_LOW_BATT   4000



/*****************************************************************************************
   GLOBAL CONFIGURATION DEFINES
*/

// UARTD0 pins ( PORTD ):
#define CFG_TXD0_PIN_MASK        PIN3_bm  
#define CFG_RXD0_PIN_MASK        PIN2_bm

// UARTC0 pins ( PORTC ):
#define CFG_TXC0_PIN_MASK        PIN3_bm
#define CFG_RXC0_PIN_MASK        PIN2_bm

// SPI pins (for lcd3310) ( PORTC ):
#define CFG_SCE_PIN_MASK         PIN1_bm
#define CFG_DC_PIN_MASK          PIN3_bm
#define CFG_SCK_PIN_MASK         PIN5_bm
#define CFG_MOSI_PIN_MASK        PIN7_bm

// SPI pins (for lcd3310) ( PORTA ):
#define CFG_RST_PIN_MASK         PIN1_bm


// ADC pins (PORTA):
#define CFG_ADC_SENS_PIN_MASK    PIN4_bm
#define CFG_ADC_SENS_MUXPOS      ADC_CH_MUXPOS_PIN4_gc // Sensor channel

#define CFG_ADC_VBATT_PIN_MASK   PIN0_bm
#define CFG_ADC_VBATT_MUXPOS     ADC_CH_MUXPOS_PIN0_gc // Sensor channel

// IO pins (PORTD):  
#define CFG_BT1_PIN_MASK         PIN7_bm
#define CFG_BT2_PIN_MASK         PIN6_bm    
#define CFG_BT3_PIN_MASK         PIN5_bm 

#define CFG_USB_CON_PIN_MASK     PIN4_bm 
#define CFG_BUZZ_PIN_MASK        PIN1_bm

// IO pins (PORTA):  
#define CFG_LED_PIN_MASK        PIN7_bm
#define CFG_BCKLGHT_PIN_MASK    PIN6_bm





/*** Interrupts priority ****/ 

// USARTD0 intlvl:
#define CFG_PRIO_USARTD0    ( USART_DREINTLVL_OFF_gc | USART_TXCINTLVL_MED_gc | USART_RXCINTLVL_OFF_gc )    

// USARTC0 intlvl:
#define CFG_PRIO_USARTC0    ( USART_DREINTLVL_OFF_gc | USART_TXCINTLVL_MED_gc | USART_RXCINTLVL_MED_gc )  

// SPI intlvl:
#define CFG_PRIO_SPI         SPI_INTLVL_LO_gc 

// ADC intlvl:
#define CFG_PRIO_ADC         ADC_CH_INTLVL_HI_gc

// Timer intlvl:
#define CFG_PRIO_TC4_CCALVL  TC45_CCAINTLVL_MED_gc
#define CFG_PRIO_TC4_CCBLVL  TC45_CCBINTLVL_MED_gc
#define CFG_PRIO_TC4_CCCLVL  TC45_CCCINTLVL_MED_gc
#define CFG_PRIO_TC4_CCDLVL  TC45_CCDINTLVL_MED_gc

// RTC intlvl
#define CFG_PRIO_RTC_OVFL    RTC_OVFINTLVL_HI_gc

// PORTD intlvl
#define CFG_PRIO_PORTD       PORT_INTLVL_LO_gc

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

// Critical sections:

#define ENTER_CRIT_ALL()      {  uint8_t actInt = PMIC_CTRL;\
                                  PRIO_ALL_LEVELS_DISABLE();

#define ENTER_CRIT_HI()       {  uint8_t actInt = PMIC_CTRL;\
                                 PRIO_HI_LEVEL_DISABLE();
                                     

#define ENTER_CRIT_MED()      {  uint8_t actInt = PMIC_CTRL;\
                                 PRIO_MED_LEVEL_DISABLE();                               
                                                                                        
#define EXIT_CRITICAL()          PMIC_CTRL = actInt;         }                               




// Macros for PDC8544:
#define DC_LO()      ( PORTC.OUTCLR = CFG_DC_PIN_MASK  )
#define DC_HI()      ( PORTC.OUTSET = CFG_DC_PIN_MASK  )
#define SCE_LO()     ( PORTC.OUTCLR = CFG_SCE_PIN_MASK )
#define SCE_HI()     ( PORTC.OUTSET = CFG_SCE_PIN_MASK )
#define RST_LO()     ( PORTA.OUTCLR = CFG_RST_PIN_MASK )
#define RST_HI()     ( PORTA.OUTSET = CFG_RST_PIN_MASK )



/*****************************************************************************************
   GLOBAL DEFINES
*/


/*****************************************************************************************
   GLOBAL FUNCTIONS DECLARATIONS
*/
void boardInit ( void );
void boardPeriEnable ( void );
void boardPeriDisable ( void );

void boardGoSleep ( void );
void boardWakeUp ( void );

#endif /* BOARDCFG_H_ */