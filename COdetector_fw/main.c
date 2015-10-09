/*
 * main.c
 *
 * Created: 2015-10-02 20:50:49
 *  Author: Chebu
 */ 


#include "common.h"

uint8_t test[] = {"Hello my beautiful world! :3\r"};

int main(void)
{ 
   boardInit();
   

       for ( uint32_t i = 0; i < 5000; i++ ){;}
       pdcLine ( "Heheszek!     ", 0 );
       pdcLine ( "Slodkich snow!", 2 );
       pdcLine ( "<3 <3 <3 <3 <3", 3 );
       pdcLine ( "     (Y)      ", 5 );



/*
    while(1)
    {        
       for ( uint32_t i = 0; i < 5000; i++ ){;}
       serialSendC ( test, sizeof(test));
       DEB_2_TGL();
    }
   */
   /*
    while(1)
    {        
       for ( uint32_t i = 0; i < 5000; i++ ){;}
       spiSend ( test, sizeof(test));
       DEB_2_TGL();
    }*/
   
   while(1){;}
}


// TODO: Add watchdog
// TODO: add critical section macro
// TODO: Add 32kHz low timer when in ULP mode
// Clock change -> protected registers