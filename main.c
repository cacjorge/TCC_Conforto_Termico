#include <16F88.h>
#fuses INTRC_IO,NOWDT,NOPUT,NOMCLR,BROWNOUT,NOLVP,NOCPD,NOWRT,NODEBUG,NOPROTECT,CCPB3
#use delay (clock=8000000)
#byte PORTA = 0x0005
#byte PORTB = 0x0006 
#byte TRISA = 0x0085 
#byte TRISB = 0x0086
#byte ANSEL = 0x009B
#byte ADCON0 = 0x001F
#byte ADCON1 = 0x009F
#byte ADRESH = 0x001E
#byte ADRESL = 0x009E
#bit ADON = ADCON0.0
#bit GO_DONE = ADCON0.2

#define ON  1
#define OFF 0

#include "KS0108.c"
#include "graphics.c"
#include <math.h>

float get_temperature(int1 channel);

void main() 
{
	float temp;
	int8 length, str[5]={};
	
	TRISA=0b00000011;	//set RA0 and RA1 to input
	ANSEL=0b00000011;	//set RA0 and RA1 to analog
	ADCON1=0b11000000;	//setup adc
	
	glcd_init(on);
	glcd_fill(OFF);
	
	draw_background();
	glcd_char(0, 26, 11, 1);	
	glcd_char(0, 32, 12, 1);

	while(true)
	{
		temp=get_temperature(0);					//get adc value from ADC channel 0 and convert it to temperature

		sprintf(str,"%4.1f",temp);					//convert float value to string
		glcd_string(0,4,str,1);						//and draw it on the screen

		length=(int8)((temp*2))+24;					//then draw some graphics
		if(length>124) length=124;
		if(length<24) length=24;
		glcd_rect(24, 6, length, 19, 1, 1);
		glcd_rect(length+1, 6, 125, 19, 1, 0);


		temp=get_temperature(1);					//same things to channel 1

		sprintf(str,"%4.1f",temp);
		glcd_string(0,40,str,1);

		length=(int8)((temp*2))+24;
		if(length>124) length=124;
		if(length<24) length=24;
		glcd_rect(24, 44, length, 57, 1, 1);
		glcd_rect(length+1, 44, 125, 57, 1, 0);
	}
}

float get_temperature(int1 channel)
{
	int16 value, sum1, sum2;
	int8 i, j;

	if(channel==0)
		ADCON0=0b01000000;	//select adc channel 0
	else
		ADCON0=0b01001000;	//select adc channel 1

	ADON=true;
	delay_us(100);

	sum2=0;
	for(j=0; j<8; j++)
	{
		sum1=0;
		for(i=0; i<8; i++)
		{
			GO_DONE=true;
			while(GO_DONE);
		
			value=make16(adresh,adresl);
		
			sum1+=value;
			delay_us(100);
		}
		sum1/=8;

		sum2+=sum1;
	}
	sum2/=8;
		
	ADON=false;

	return (float)sum2/2;
}


