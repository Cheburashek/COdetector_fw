/*
 * main.c
 *
 * Created: 2015-10-02 20:50:49
 *  Author: Chebu
 */ 


#include "common.h"
#include "serial.h"


uint8_t test[] = {"Hello my beautiful world! :3\r"};

int main(void)
{
 
   boardInit();
   
    // TO be changed:
   CFG_GLOBAL_INT_ENABLE();   
   PRIO_ALL_LEVELS_ENABLE();
   
  
   
   
    while(1)
    {        
       for ( uint32_t i = 0; i < 5000; i++ ){;}
       serialSendC ( test, sizeof(test));
       DEB_2_TGL();
    }
}


// TODO: Add watchdog
// TODO: add critical section macro
// TODO: Add 32kHz low timer when i ULP mode
// Clock change -> protected registers