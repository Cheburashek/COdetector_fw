/*
 * main.c
 *
 * Created: 2015-10-02 20:50:49
 *  Author: Chebu
 */ 

// Notes for txt:
/*

- 20ms sampling - AC line glitch rejection
- for initialization - fast 2MHz, after - 32kHz ( e.g. fast cleaning LCD RAM )
*/


// TODO: add watchdog

// TODO: add temperature measurement 

// TODO: disable logs when no usb connected

// TODO: przytrzymanie przycisku


// Du¿e skoki sygna³u -> potrzebne uœrednianie np.m za 15s i dzia³a

#include "common.h"


//tests:
#include "ADC.h"

#include "PDC8544.h"
#include "IO.h"
#include "serial.h"


//****************************************************************************************
int main(void)
{     
   //CCP=CCP_SPM_gc; 
   //SLEEP.CTRL |= SLEEP_SMODE_PDOWN_gc | SLEEP_SEN_bm;  // Power saving enabled
   
   boardInit();                        // Board peripherals initialization      


   while(1)
   {
      _delay_ms( 100 );
      adcStartChannel ( TEMP );

   }

}



   
    

