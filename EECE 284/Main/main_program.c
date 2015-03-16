#include <stdlib.h>
#include <stdio.h>
#include <p89lpc9351.h>

#define XTAL 7373000L
#define BAUD 115200L

//We want timer 0 to interrupt every millisecond ((1/1000Hz)=1 ms)
#define FREQ 1000L
//The reload value formula comes from the datasheet...
#define TIMER0_RELOAD_VALUE (65536L-((XTAL)/(2*FREQ)))

// LCD in 4 bit Mode
#define LCD_RS P3_0
#define LCD_RW P1_6
#define LCD_E  P1_7
#define LCD_D7 P2_7
#define LCD_D6 P2_6
#define LCD_D5 P2_5
#define LCD_D4 P2_4
#define CHARS_PER_LINE 16

// The volatile keyword prevents the compiler from optimizing out these variables
// that are shared between an interrupt service routine and the main code.
volatile int msCount=0;
volatile unsigned char secs=0, mins=0;
volatile bit time_update_flag=0;

void InitPorts(void)
{
	P0M1=0;
	P0M2=0;
	P1M1=0;
	P1M2=0;
	P2M1=0;
	P2M2=0;
	P3M1=0;
	P3M2=0;
}

void Init_Serial (void)
{
	BRGCON=0x00; //Make sure the baud rate generator is off
	BRGR1=((XTAL/BAUD)-16)/0x100;
	BRGR0=((XTAL/BAUD)-16)%0x100;
	BRGCON=0x03; //Turn-on the baud rate generator
	SCON=0x52; //Serial port in mode 1, ren, txrdy, rxempty
}

void Wait50us (void)
{
	_asm
    mov R0, #82
L0: djnz R0, L0 ; 2 machine cycles-> 2*0.27126us*92=50us
    _endasm;
}

void waitms (unsigned int ms)
{
	unsigned int j;
	unsigned char k;
	for(j=0; j<ms; j++)
		for (k=0; k<20; k++) Wait50us();
}

void LCD_pulse (void)
{
	LCD_E=1;
	Wait50us();
	LCD_E=0;
}

void LCD_byte (unsigned char x)
{
	// The accumulator in the C8051Fxxx is bit addressable!
	ACC=x; //Send high nible
	LCD_D7=ACC_7;
	LCD_D6=ACC_6;
	LCD_D5=ACC_5;
	LCD_D4=ACC_4;
	LCD_pulse();
	Wait50us();
	ACC=x; //Send low nible
	LCD_D7=ACC_3;
	LCD_D6=ACC_2;
	LCD_D5=ACC_1;
	LCD_D4=ACC_0;
	LCD_pulse();
}

void WriteData (unsigned char x)
{
	LCD_RS=1;
	LCD_byte(x);
	waitms(2);
}

void WriteCommand (unsigned char x)
{
	LCD_RS=0;
	LCD_byte(x);
	waitms(5);
}

void LCD_4BIT (void)
{
	LCD_E=0; // Resting state of LCD's enable is zero
	LCD_RW=0; // We are only writing to the LCD in this program
	waitms(20);
	// First make sure the LCD is in 8-bit mode and then change to 4-bit mode
	WriteCommand(0x33);
	WriteCommand(0x33);
	WriteCommand(0x32); // Change to 4-bit mode

	// Configure the LCD
	WriteCommand(0x28);
	WriteCommand(0x0c);
	WriteCommand(0x01); // Clear screen command (takes some time)
	waitms(20); // Wait for clear screen command to finsih.
}

void LCDprint(char * string, unsigned char line, bit clear)
{
	unsigned char j;

	WriteCommand(line==2?0xc0:0x80);
	waitms(5);
	for(j=0; string[j]!=0; j++)	WriteData(string[j]);// Write the message
	if(clear) for(; j<CHARS_PER_LINE; j++) WriteData(' '); // Clear the rest of the line
}

void InitADC(void)
{
	// Set adc1 channel pins as input only 
	P0M1 |= (P0M1_4);
	P0M2 &= ~(P0M1_4);

	BURST1=1; //Autoscan continuos conversion mode
	ADMODB = CLK0; //ADC1 clock is 7.3728MHz/2
	ADINS  = (ADI13); // Select the four channels for conversion
	ADCON1 = (ENADC1|ADCS10); //Enable the converter and start immediately
	while((ADCI1&ADCON1)==0); //Wait for first conversion to complete
}

void InitTimer0 (void)
{
	// Initialize timer 0 for ISR 'pwmcounter' below
	TR0=0; // Stop timer 0
	TMOD=(TMOD&0xf0)|0x01; // 16-bit timer
	TH0=TIMER0_RELOAD_VALUE/0x100;
	TL0=TIMER0_RELOAD_VALUE%0x100;
	TR0=1; // Start timer 0 (bit 4 in TCON)
	ET0=1; // Enable timer 0 interrupt
	EA=1;  // Enable global interrupts
}

//Interrupt 1 is for timer 0.  This function is executed every millisecond.
void Timer0ISR (void) interrupt 1
{
	//Reload the timer
	TR0=0; // Stop timer 0
	TH0=TIMER0_RELOAD_VALUE/0x100;
	TL0=TIMER0_RELOAD_VALUE%0x100;
	TR0=1; // Start timer 0
	
	msCount++;
	if(msCount==1000)
	{
		time_update_flag=1;
		msCount=0;
		secs++;
		if(secs==60)
		{
			secs=0;
			mins++;
			if(mins==60)
			{
				mins=0;
			}
		}
	}
	
}

void main (void)
{
	char str[17];
	double threshold = 2;
	
	InitPorts();
	InitADC();
	LCD_4BIT();
	InitTimer0();
		
	while(1)
	{
		if(time_update_flag==1) // If the clock has been updated refresh the display
		{
			time_update_flag=0;
			sprintf(str, "V=%5.2f", (AD1DAT3/255.0)*3.3); // Display the voltage at pin P1.4
			LCDprint(str, 1, 1);
			sprintf(str, "%02d:%02d", mins, secs); // Display the clock
			LCDprint(str, 2, 1);
		}	
	}
	
}