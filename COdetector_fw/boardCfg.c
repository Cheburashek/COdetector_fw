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
   // TODO:
      // Low power pre-configuration:      
      // page 117 in manual (power reduction)!!!
  

      // Clocks:
   #if ( F_CPU == F_CPU_32KHZ )
      OSC.CTRL |= OSC_RC32KEN_bm;               // Enabling 32kHz clock
      while (!(OSC.STATUS & OSC_RC32KRDY_bm));  // Waiting for clock
      CCP=CCP_IOREG_gc;                         // Protected register
      CLK.CTRL = CLK_SCLKSEL_RC32K_gc;          // 32khz internal
      OSC.CTRL = OSC_RC32KEN_bm;                // Disabling other clocks
      LOG_TXT ( ">>clock<<   Clock 32kHZ\n" );
      
   #elif ( F_CPU == F_CPU_32MHZ )   
      OSC.CTRL |= OSC_RC32MEN_bm;               // Enabling 32MHz clock
      while (!(OSC.STATUS & OSC_RC32MRDY_bm));  // Waiting for clock 
      CCP=CCP_IOREG_gc;                         // Protected register
      CLK.CTRL = CLK_SCLKSEL_RC32M_gc;          // 32Mhz internal
      OSC.CTRL = OSC_RC32MEN_bm;                // Disabling other clocks
      LOG_TXT ( ">>clock<<   Clock 32MHZ\n" );
      
   #elif ( F_CPU == F_CPU_2MHZ )
      LOG_TXT ( ">>clock<<   Clock 2MHZ\n" );
      // Default after restart
      
   #elif ( F_CPU == F_CPU_8MHZ )   
      OSC.CTRL |= OSC_RC8MEN_bm;               // Enabling 32MHz clock
      while (!(OSC.STATUS & OSC_RC8MRDY_bm));  // Waiting for clock
      CCP=CCP_IOREG_gc;                          // Protected register
      CLK.CTRL = CLK_SCLKSEL_RC8M_gc;            // 8Mhz internal
      LOG_TXT ( ">>clock<<  Clock 8MHZ\n" );
      
   #else
      #error "Clock is not set!"
   #endif


   
   // Initializations:
   #ifdef LOG_USARTD0
      serialInitD();
   #endif 
      
   CFG_GLOBAL_INT_ENABLE();
   PRIO_ALL_LEVELS_ENABLE();
   
   // Order is important:   
   adcInit();
   ioInit();
   spiInit();
   pdcInit();
   interInit();  
   timerInit();
   systemInit();
     
   
   LOG_TXT ( ">>init<<   Board initialized\n" );
   
}