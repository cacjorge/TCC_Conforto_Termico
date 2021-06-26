#define GLCD_CS1     PIN_A6   // Chip Selection 1
#define GLCD_CS2     PIN_A7   // Chip Selection 2
#define GLCD_DI      PIN_A4   // Data or Instruction input
#define GLCD_RW      PIN_A3   // Read/Write
#define GLCD_E       PIN_A2   // Enable

// for asm
#define CS1     6   // Chip Selection 1
#define CS2     7   // Chip Selection 2
#define DI      4   // Data or Instruction input
#define RW      3   // Read/Write
#define E       2   // Enable

#define GLCD_LEFT    0
#define GLCD_RIGHT   1

// Purpose:    Write a byte of data to the specified chip
// Inputs:     1) chipSelect - which chip to write the data to
//             2) data - the byte of data to write
void glcd_writeByte(int1 side, BYTE data)
{
#asm
	btfsc side,0
	goto right
	goto left
	
	right:
	bcf PORTA,CS2
	goto end
	
	left:
	bcf PORTA,CS1

	end:

	clrf TRISB
	bcf PORTA,RW
	movf data
	movwf PORTB
	bsf PORTA,E
	bcf PORTA,E
	
	bsf PORTA,CS1
	bsf PORTA,CS2
#endasm
}


// Purpose:    Reads a byte of data from the specified chip
// Ouputs:     A byte of data read from the chip
int8 glcd_readByte(int1 side)
{
   int8 data;                 // Stores the data read from the LCD

#asm
	movlw 0xff
	movwf TRISB
	bsf   PORTA,RW


	btfsc side,0
	goto right
	goto left
	
	right:
	bcf PORTA,CS2
	goto end
	
	left:
	bcf PORTA,CS1

	end:

	bsf	PORTA,E
	movf PORTB,w
	movwf data
	
	bcf PORTA,E
	bsf	PORTA,CS1
	bsf PORTA,CS2
	movf data
#endasm
	return data;    
}

// Purpose:       Initialize the LCD.
//                Call before using any other LCD function.
// Inputs:        OFF - Turns the LCD off
//                ON  - Turns the LCD on
void glcd_init(int1 mode)
{
#asm
	bcf	PORTA, E
	bsf	PORTA, CS1
	bsf	PORTA, CS2
	bcf	PORTA, DI
#endasm

   glcd_writeByte(GLCD_LEFT,  0xC0);    // Specify first RAM line at the top
   glcd_writeByte(GLCD_RIGHT, 0xC0);    //   of the screen
   glcd_writeByte(GLCD_LEFT,  0x40);    // Set the column address to 0
   glcd_writeByte(GLCD_RIGHT, 0x40);
   glcd_writeByte(GLCD_LEFT,  0xB8);    // Set the page address to 0
   glcd_writeByte(GLCD_RIGHT, 0xB8);

   if(mode == ON)
   {
      glcd_writeByte(GLCD_LEFT,  0x3F); // Turn the display on
      glcd_writeByte(GLCD_RIGHT, 0x3F);
   }
   else
   {
      glcd_writeByte(GLCD_LEFT,  0x3E); // Turn the display off
      glcd_writeByte(GLCD_RIGHT, 0x3E);
   }

}


// Purpose:    Turn a pixel on a graphic LCD on or off
// Inputs:     1) x - the x coordinate of the pixel
//             2) y - the y coordinate of the pixel
//             3) color - ON or OFF
void glcd_pixel(int8 x, int8 y, int1 color)
{
   int8 data;
   int1 side = GLCD_LEFT;  // Stores which chip to use on the LCD

   if(x > 63)              // Check for first or second display area
   {
      x -= 64;
      side = GLCD_RIGHT;
   }

   output_low(GLCD_DI);                         // Set for instruction
   bit_clear(x,7);                              // Clear the MSB. Part of an instruction code
   bit_set(x,6);                                // Set bit 6. Also part of an instruction code
   glcd_writeByte(side, x);                     // Set the horizontal address
   glcd_writeByte(side, (y/8 & 0xBF) | 0xB8);   // Set the vertical page address
   output_high(GLCD_DI);                        // Set for data
   glcd_readByte(side);                         // Need two reads to get data
   data = glcd_readByte(side);                  //  at new address

   if(color == ON) bit_set(data, y%8);  // Turn the pixel on     
   else bit_clear(data, y%8);     	   // turn the pixel off

   output_low(GLCD_DI);          // Set for instruction
   glcd_writeByte(side, x);      // Set the horizontal address
   output_high(GLCD_DI);         // Set for data
   glcd_writeByte(side, data);   // Write the pixel data
}

