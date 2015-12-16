/*
 * PDC8544.h
 *
 * Created: 2015-10-09 00:39:20
 *  Author: Chebu
 */ 


#ifndef PDC8544_H_
#define PDC8544_H_

/*****************************************************************************************
   GLOBAL INCLUDES
*/

/*****************************************************************************************
   GLOBAL DEFINITIONS
*/


/*****************************************************************************************
   GLOBAL FUNCTIONS DECLARATIONS
*/

void pdcInit( void );
void pdcInvertMode ( bool stat );
void pdcLine( char ch[14], uint8_t pos_Y );
void pdcChar( char ch, uint8_t pos_Y, uint8_t pos_X );
void pdcUint( uint16_t val, uint8_t pos_Y, uint8_t pos_X, uint8_t length );
void pdcShort( short val, uint8_t pos_Y, uint8_t pos_X, uint8_t length );
void pdcPicture( const uint8_t pic[504] );
void pdcClearRAM( void );
void pdcClearLine( uint8_t pos_Y );

void pdcPowerDown ( void );
void pdcPowerOn ( void );

#endif /* PDC8544 */