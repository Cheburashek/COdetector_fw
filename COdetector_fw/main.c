/*
 * main.c
 *
 * Created: 2015-10-02 20:50:49
 *  Author: Chebu
 */ 

#include "common.h"
#include "SPI.h"
#include "IO.h"
#include "interFace.h"

//tests:
#include "ADC.h"

#include "PDC8544.h"

#include "serial.h"
#include "oneWire.h"
#include "SPI.h"
#include "timers.h"

//****************************************************************************************
int main(void)
{     
   boardInit();    // Board peripherals initialization      

   while(1)
   {
#ifdef SLEEP_PERM      
      _delay_ms(10);
      
      if ( !spiIsBusy() && !adcIsBusy() && !IO_GET_USB_CONN() && interIsSleepPerm() )
      {     
         //ioBtIntsLevels();
         //interMainStateMachineSet( SLEEP_M_STATE );
         
         _delay_ms(50);         
         SLEEP_POINT();
         interSetSleepPerm();
         //ioBtIntsFalling();
      }    
#endif   
   }
 
}



   
    

