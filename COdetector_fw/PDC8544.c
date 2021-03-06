/*
 * PDC8544.c
 *
 * Created: 2015-10-09 00:38:19
 *  Author: Chebu
 */ 

/*****************************************************************************************
   LOCAL INCLUDES
*/
#include "common.h"
#include "PDC8544.h"
#include "SPI.h"

/*****************************************************************************************
   LOCAL MACROS
*/


#define DC_DATA      true
#define DC_CMD       false


/*****************************************************************************************
   LOCAL DEFINITIONS
*/

/*****************************************************************************************
   LOCAL VARIABLES
*/

/*****************************************************************************************
   LOCAL CONSTANTS
*/

static const uint8_t charTab[480] = {
   
   0x00,0x00,0x00,0x00,0x00,// 20 space      
   0x00,0x00,0x5f,0x00,0x00,   // 21 !        
   0x00,0x07,0x00,0x07,0x00,   // 22 "      
   0x14,0x7f,0x14,0x7f,0x14,   // 23 #        
   0x24,0x2a,0x7f,0x2a,0x12,   // 24 $         
   0x23,0x13,0x08,0x64,0x62,   // 25 %
   0x36,0x49,0x55,0x22,0x50,   // 26 &
   0x00,0x05,0x03,0x00,0x00,   // 27 '     
   0x00,0x1c,0x22,0x41,0x00,   // 28 (          
   0x00,0x41,0x22,0x1c,0x00,   // 29 )
   0x14,0x08,0x3e,0x08,0x14,   // 2a *
   0x08,0x08,0x3e,0x08,0x08,   // 2b +
   0x00,0x50,0x30,0x00,0x00,   // 2c ,
   0x08,0x08,0x08,0x08,0x08,   // 2d -
   0x00,0x60,0x60,0x00,0x00,   // 2e .
   0x20,0x10,0x08,0x04,0x02,   // 2f /
   0x3e,0x51,0x49,0x45,0x3e,   // 30 0    //start
   0x00,0x42,0x7f,0x40,0x00,   // 31 1
   0x42,0x61,0x51,0x49,0x46,   // 32 2
   0x21,0x41,0x45,0x4b,0x31,   // 33 3
   0x18,0x14,0x12,0x7f,0x10,   // 34 4
   0x27,0x45,0x45,0x45,0x39,   // 35 5
   0x3c,0x4a,0x49,0x49,0x30,   // 36 6
   0x01,0x71,0x09,0x05,0x03,   // 37 7
   0x36,0x49,0x49,0x49,0x36,   // 38 8
   0x06,0x49,0x49,0x29,0x1e,   // 39 9
   0x00,0x36,0x36,0x00,0x00,   // 3a :
   0x00,0x56,0x36,0x00,0x00,   // 3b ;
   0x08,0x14,0x22,0x41,0x00,   // 3c <
   0x14,0x14,0x14,0x14,0x14,   // 3d =
   0x00,0x41,0x22,0x14,0x08,   // 3e >
   0x02,0x01,0x51,0x09,0x06,   // 3f ?
   0x32,0x49,0x79,0x41,0x3e,   // 40 @
   0x7e,0x11,0x11,0x11,0x7e,   // 41 A
   0x7f,0x49,0x49,0x49,0x36,   // 42 B
   0x3e,0x41,0x41,0x41,0x22,   // 43 C
   0x7f,0x41,0x41,0x22,0x1c,   // 44 D
   0x7f,0x49,0x49,0x49,0x41,   // 45 E
   0x7f,0x09,0x09,0x09,0x01,   // 46 F
   0x3e,0x41,0x49,0x49,0x7a,   // 47 G
   0x7f,0x08,0x08,0x08,0x7f,   // 48 H
   0x00,0x41,0x7f,0x41,0x00,   // 49 I
   0x20,0x40,0x41,0x3f,0x01,   // 4a J
   0x7f,0x08,0x14,0x22,0x41,   // 4b K
   0x7f,0x40,0x40,0x40,0x40,   // 4c L
   0x7f,0x02,0x0c,0x02,0x7f,   // 4d M
   0x7f,0x04,0x08,0x10,0x7f,   // 4e N
   0x3e,0x41,0x41,0x41,0x3e,   // 4f O
   0x7f,0x09,0x09,0x09,0x06,   // 50 P
   0x3e,0x41,0x51,0x21,0x5e,   // 51 Q
   0x7f,0x09,0x19,0x29,0x46,   // 52 R
   0x46,0x49,0x49,0x49,0x31,   // 53 S
   0x01,0x01,0x7f,0x01,0x01,   // 54 T
   0x3f,0x40,0x40,0x40,0x3f,   // 55 U
   0x1f,0x20,0x40,0x20,0x1f,   // 56 V
   0x3f,0x40,0x38,0x40,0x3f,   // 57 W
   0x63,0x14,0x08,0x14,0x63,   // 58 X
   0x07,0x08,0x70,0x08,0x07,   // 59 Y
   0x61,0x51,0x49,0x45,0x43,   // 5a Z
   0x00,0x7f,0x41,0x41,0x00,   // 5b [
   0x02,0x04,0x08,0x10,0x20,   // 5c Yen Currency Sign
   0x00,0x41,0x41,0x7f,0x00,   // 5d ]
   0x04,0x02,0x01,0x02,0x04,   // 5e ^
   0x40,0x40,0x40,0x40,0x40,   // 5f _
   0x00,0x01,0x02,0x04,0x00,   // 60 `
   0x20,0x54,0x54,0x54,0x78,   // 61 a
   0x7f,0x48,0x44,0x44,0x38,   // 62 b
   0x38,0x44,0x44,0x44,0x20,   // 63 c
   0x38,0x44,0x44,0x48,0x7f,   // 64 d
   0x38,0x54,0x54,0x54,0x18,   // 65 e
   0x08,0x7e,0x09,0x01,0x02,   // 66 f
   0x0c,0x52,0x52,0x52,0x3e,   // 67 g
   0x7f,0x08,0x04,0x04,0x78,   // 68 h
   0x00,0x44,0x7d,0x40,0x00,   // 69 i
   0x20,0x40,0x44,0x3d,0x00,   // 6a j
   0x7f,0x10,0x28,0x44,0x00,   // 6b k
   0x00,0x41,0x7f,0x40,0x00,   // 6c l
   0x7c,0x04,0x18,0x04,0x78,   // 6d m
   0x7c,0x08,0x04,0x04,0x78,   // 6e n
   0x38,0x44,0x44,0x44,0x38,   // 6f o
   0x7c,0x14,0x14,0x14,0x08,   // 70 p
   0x08,0x14,0x14,0x18,0x7c,   // 71 q
   0x7c,0x08,0x04,0x04,0x08,   // 72 r
   0x48,0x54,0x54,0x54,0x20,   // 73 s
   0x04,0x3f,0x44,0x40,0x20,   // 74 t
   0x3c,0x40,0x40,0x20,0x7c,   // 75 u
   0x1c,0x20,0x40,0x20,0x1c,   // 76 v
   0x3c,0x40,0x30,0x40,0x3c,   // 77 w
   0x44,0x28,0x10,0x28,0x44,   // 78 x
   0x0c,0x50,0x50,0x50,0x3c,   // 79 y
   0x44,0x64,0x54,0x4c,0x44,   // 7a z
   0x00,0x08,0x36,0x41,0x00,   // 7b <
   0x00,0x00,0x7f,0x00,0x00,   // 7c |
   0x00,0x41,0x36,0x08,0x00,   // 7d >
   0x10,0x08,0x08,0x10,0x08,   // 7e Right Arrow    ->
   0x78,0x46,0x41,0x46,0x78};  // 7f Left Arrow   <-

/*****************************************************************************************
   LOCAL FUNCTIONS DECLARATIONS
*/

/*****************************************************************************************
   LOCAL FUNCTIONS DEFINITIONS
*/

static void pdcChipDisable ( void );
static void pdcReset( void );
static void pdcSend( bool DC, uint8_t data );
static void pdcSetRow( uint8_t addr_Y );
static void pdcSetCol( uint8_t addr_X );



/*****************************************************************************************
   GLOBAL FUNCTIONS DEFINITIONS
*/


// *************************************************************************
// Function to reset PDC8544
static void pdcReset( void )
{   
   RST_LO();
   _delay_ms(15);  
   RST_HI();   
   _delay_ms(15);   
}


// *************************************************************************
// Function to set callback in SPI.c (end of transmission)
inline static void pdcChipDisable ( void )
{
   SCE_HI();
}

// *************************************************************************
// Function to send data
static void pdcSend( bool DC, uint8_t data )
{
   spiEnhStruct_t dataStr = {data, DC};
   
   spiSend( &dataStr );   
}

// *************************************************************************
// Function to set Y -> row
static void pdcSetRow( uint8_t addr_Y )
{
   if( addr_Y > 5 ){ /*ERROR!!!!!!*/ }
   
   addr_Y += 0x40;	// Adding Set Y address of RAM command
   pdcSend( DC_CMD, addr_Y );

}


// *************************************************************************
// Function to set X -> Column x5
static void pdcSetCol( uint8_t addr_X )
{
   if( addr_X > 83 ){ /*ERROR!!!!!!*/ }
   
   addr_X += 0x80;	// Adding Set X address of RAM command
   pdcSend( DC_CMD, addr_X );

}


// *************************************************************************
// API:
// *************************************************************************

// *************************************************************************
// Function to configure PDC8544
void pdcInit( void )
{
  
   pdcReset();
   
   SCE_LO();
   
   pdcSend( DC_CMD, 0x21 );	// Extended cmd
   pdcSend( DC_CMD, 0xE0 );	// Bias
   pdcSend( DC_CMD, 0x04 );	// Temp. control
   pdcSend( DC_CMD, 0xCB );	// Set V
   pdcSend( DC_CMD, 0x20 );	// Basic cmd   / horizontal addressing
   pdcSend( DC_CMD, 0x0C );	// Normal mode

   pdcClearRAM();

}

// *************************************************************************
// Function to set/clear invert video mode
void pdcInvertMode ( bool stat )
{
   if ( !stat )
   {
      pdcSend( DC_CMD, 0x0C );	// Normal mode      
   }
   else
   {
      pdcSend( DC_CMD, 0x0D );	// Inverted mode
   }
}

// *************************************************************************
// Function to clear RAM of LCD
void pdcClearRAM( void )
{
   uint8_t X = 0,
   Y = 0;
   
   pdcSetCol( 0x00 );
   
   for( Y = 0; Y < 6; Y++ )
   {
      pdcSetRow( Y );
      pdcSetCol( 0x00 );
      
      for( X = 0; X < 84; X++)
      {
         pdcSend( DC_DATA, 0x00 );
      }
   }
}

// *************************************************************************
// Function to clear one line
void pdcClearLine( uint8_t pos_Y )
{

   uint8_t X = 0;
   
   pdcSetRow( pos_Y );
   pdcSetCol( 0x00 );
   
   for( X = 0; X < 84; X++)
   {
      pdcSend( DC_DATA, 0x00 );
   }
}

// *************************************************************************
// Function to write a single char from a table
void pdcChar( char ch, uint8_t pos_Y, uint8_t pos_X, bool neg )
{
   uint8_t X = 0;
    
   pdcSetRow( pos_Y   );				// Setting active row
   pdcSetCol( pos_X*6 ); 

   for( X = 0; X < 5; X++ )
   {            
       if ( !neg ) pdcSend( DC_DATA, charTab[ ((ch-0x20)*5) + X ] );	// Finding index of char in table
       else pdcSend( DC_DATA, ~charTab[ ((ch-0x20)*5) + X ] );	
   }
   
   if ( !neg ) pdcSend( DC_DATA, 0x00 );			// One pixel spacing after char   
   else pdcSend( DC_DATA, 0xFF );			// One pixel spacing after char   
}

// *************************************************************************
// Function to display one line char by char
void pdcLine( char ch[14], uint8_t pos_Y, bool neg )
{
   //pdcClearLine( pos_Y );
   
   for( uint8_t i = 0; i < 14; i++ )
   {      
      if( ch[i] < 0x20 || ch[i] > 0x7F)
      {
         pdcChar( 0x20, pos_Y, i, neg );
      }
      else
      {
         pdcChar( ch[i], pos_Y, i, neg );
      }
   }
}


// *************************************************************************
// Function to display uint value in chosen part of LCD (max 2^16)
void pdcUint( uint16_t val, uint8_t pos_Y, uint8_t pos_X, uint8_t length )
{   
   uint32_t temp_val = val;
   uint8_t len = 0,
   k = 0;
   char temp_ch;
   
   if( temp_val > 65535 ){ /*ERROR!!!!*/ } 
   
   for( len = 0; temp_val != 0; len++ )
   {	// Length of input "string"
      temp_val /= 10;
   }
   
   temp_val = val;   

   if ( 0x00 == val )
   {
      pdcChar( 0x30, pos_Y, pos_X, FALSE );
      len = 1;
   }
   else 
   {
      for( k = 1; k <= len; k++ )
      {      
         temp_ch = temp_val % 10;
         temp_ch += 0x30;			// Number -> ASCII
      
         pdcChar( temp_ch, pos_Y, (pos_X+len-k), FALSE );	
         temp_val /= 10;
      }
   }
      
   if( len != length )
   {      
      for( k = 1; k <= (length - len); k++ )
      {
         pdcChar( ' ', pos_Y, (pos_X+length-k), FALSE );         
      }
   }
   else{	/*ERROR!!!!!! */ }
     
}


// *************************************************************************
// Function to display int value in chosen part of LCD (max +-2^15)
void pdcShort( short val, uint8_t pos_Y, uint8_t pos_X, uint8_t length )
{
   // for U2 only!
   
   if( val > 0 )
   {
      pdcChar( ' ', pos_Y, pos_X, FALSE );
      pdcUint( val, pos_Y, pos_X+1, length );
   }
   else if( val < 0 )
   {      
      val = ~val + 1;
      pdcChar( '-', pos_Y, pos_X, FALSE );
      pdcUint( val, pos_Y, pos_X+1, length );      
   }
}


// *************************************************************************
// Function to display a picture
void pdcPicture( const uint8_t pic[504] )
{

   uint8_t X = 0,
   Y = 0;
   
   for( Y = 0; Y < 6; Y++ )
   {      
      pdcSetRow( Y );
      
      for( X = 0; X < 84; X++ )
      {         
         //pdcSetCol( X );
         pdcSend( DC_DATA, pic[ 84*Y + X ] );         
      }
   }
}


// *************************************************************************
// Function to set power down mode
void pdcPowerDown ( void )
{
   pdcSend ( DC_CMD, 0x24 ); // Enter power down mode
}

// *************************************************************************
// Function to wake up
void pdcPowerOn ( void )
{
   pdcSend ( DC_CMD, 0x20 ); // Exit power down mode
}