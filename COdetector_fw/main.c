/*
 * main.c
 *
 * Created: 2015-10-02 20:50:49
 *  Author: Chebu
 */ 


#include "common.h"
#include "serial.h"




int main(void)
{
  // Cfg, to be moved into boardCfg
   
   // Debug LEDs:
      PORTD.DIRSET = PIN5_bm;
      PORTD.DIRSET = PIN6_bm;
      PORTD.DIRSET = PIN7_bm;      
   //

  OSC.CTRL |= OSC_RC8MEN_bm;
  while (!(OSC.STATUS & OSC_RC8MRDY_bm));

   CCP=CCP_IOREG_gc;
   CLK.CTRL = CLK_SCLKSEL_RC8M_gc;



  
  
  
   CFG_GLOBAL_INT_ENABLE();
   PRIO_ALL_LEVELS_ENABLE();
   
   serialInit ();
   
    while(1)
    {       
    }
}


// TODO: Add watchdog
// TODO: Add 32kHz low timer when i ULP mode
// Clock change -> protected registers