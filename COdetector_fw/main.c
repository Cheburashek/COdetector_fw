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

// Problems to include in txt:
/*

- adc auto offset calibration - temporary manual

*/

// BIG PROBLEM!!!
// UStawienie pind0 na HI (input) robi to samo dziadostwo co ustawienie na hi (output) na pinc0 !! -zmiana Urev dla ADC
//


// TODO: clk in lopo
// TODO: add watchdog

// TODO: add temperature measurement 

// TODO: disable logs when no usb connected

// TODO: przytrzymanie przycisku



// 32k zawsze w³¹czone, 8Mhz tylko do SPI przy wy³¹czonym lopo

// pierwsze chciua³em ca³kiem osobno tworzyc modu³y, czas nie ozwoliu³


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
      //
      //serialTempMeasStart();


   }

}



   
    

