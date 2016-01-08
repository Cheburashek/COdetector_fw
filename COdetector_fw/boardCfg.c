/*
 * boardCfg.c
 *
 * Created: 2015-10-02 21:57:02
 *  Author: Chebu
 */ 

/*****************************************************************************************
   LOCAL INCLUDES
*/

#include "boardCfg.h"
#include "system.h"
#include "serial.h"
#include "SPI.h"
#include "PDC8544.h"
#include "ADC.h"
#include "timers.h"
#include "IO.h"
#include "interFace.h"
/*****************************************************************************************
   MACROS
*/

/*****************************************************************************************
   LOCAL DEFINITIONS}
*/

/*****************************************************************************************
   LOCAL VARIABLES
*/

/*****************************************************************************************
   LOCAL FUNCTIONS DECLARATIONS
*/

/*****************************************************************************************
   LOCAL FUNCTIONS DEFINITIONS
*/

/*****************************************************************************************
   GLOBAL FUNCTIONS DEFINITIONS
*/

void boardInit ( void )
{  
   OSC.CTRL |= (OSC_RC8MEN_bm | OSC_RC8MLPM_bm );                // Enabling 8MHz clock in low power mode
   while (!(OSC.STATUS & OSC_RC8MRDY_bm));    // Waiting for clock
   CCP=CCP_IOREG_gc;                          // Protected register
   CLK.CTRL = CLK_SCLKSEL_RC8M_gc;            // 8Mhz internal
         
   // Initializations:
   #ifdef LOG_USARTD0
      serialInitD();
   #endif 
      
   CFG_GLOBAL_INT_ENABLE();
   PRIO_ALL_LEVELS_ENABLE();
   
   
   // Order is important:  
   adcInit(); 
   timerInit ( RTC_PERIOD );      
   spiInit();
   pdcInit();
   ioInit();
   systemInit();
   interInit();     
   
  
   // Disabling clocks for peripherals ( from ~3.5mA to 3.0mA ):
   PR.PRGEN = PR_EDMA_bm |
              PR_XCL_bm  |
              PR_EVSYS_bm;
              
   PR.PRPA = PR_DAC_bm |
             PR_AC_bm;
             
   PR.PRPC = PR_HIRES_bm |
             PR_TC5_bm   |
             PR_TWI_bm;         
                        
   PR.PRPD = PR.PRPC;           
  
   NVM.CTRLB |= NVM_EPRM_bm;                   
                        
}

