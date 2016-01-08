/*
 * main.c
 *
 * Created: 2015-10-02 20:50:49
 *  Author: Chebu
 */ 

#include "boardCfg.h"
#include "system.h"

//tests:
#include "ADC.h"

#include "PDC8544.h"

#include "serial.h"
#include "oneWire.h"
#include "SPI.h"
#include "timers.h"
#include "common.h"
#include "SPI.h"
#include "IO.h"
#include "interFace.h"


//****************************************************************************************
int main(void)
{     
   boardInit();    // Board peripherals initialization      

   systemStart();  // Infinite loop
 
}



   
    

