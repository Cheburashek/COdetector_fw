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

#include "common.h"

#include "ADC.h"
#include "PDC8544.h"
#include "IO.h"


//****************************************************************************************
int main(void)
{     
   
   boardInit();                        // Board peripherals initialization      
    
    ioBcklghtOff();

register8_t* reg = 0x0200;
   while(1)
   {
      

      //uint8_t* pnt = (uint8_t*)&ADCA;
     //
     //
    //LOG_UINT ("r  ", (uint8_t)*reg );   
    //reg++;  
      //_delay_ms(100);
      //ADC_START();
      //LOG_UINT("raw ", ADCA.CH0RES);
   }

}


   
    

