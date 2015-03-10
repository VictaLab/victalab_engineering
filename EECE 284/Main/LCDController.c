/*
 * This file sets up pin connection with the P89LC Controller with the LCD 
 * Expectation: please adjust the physical connection accordingly wiht the code setup
*/
#include <stdlib.h>
#include <stdio.h>
#include <p89lpc9351.h>
#include <math.h>

#define XTAL 7373000L
#define BAUD 115200L

// Make sure these definitions match your wiring
//===================================================================================
#define LCD_RS P3_0
#define LCD_RW P1_6
#define LCD_E  P1_7
#define LCD_D7 P2_7
#define LCD_D6 P2_6
#define LCD_D5 P2_5
#define LCD_D4 P2_4
#define LCD_D3 P2_3
#define LCD_D2 P2_2
#define LCD_D1 P2_1
#define LCD_D0 P2_0
#define CHARS_PER_LINE 16
//===================================================================================

#define MICRO_SECOND 0.001

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

// Wait for 50 microseconds = 0.00005 second
void Wait50us (void)
{
	_asm
    mov R0, #82
L0: djnz R0, L0 ; 2 machine cycles-> 2*0.27126us*92=50us
    _endasm;
}

// Wait 1 millisecond = 0.001 second
// -- param: ms, integer in milliseconds
void waitms (unsigned int ms)
{
	unsigned int j;
	unsigned char k;
	for(j=0; j<ms; j++)
		for (k=0; k<20; k++) Wait50us();
}

// Here LCD_E is a signal transmission flag 
void LCD_pulse (void)
{
	LCD_E=1;
	Wait50us();
	LCD_E=0;
}

// Register a character message 8 bit long to the LCD 
// -- param: x, an 8 bit character message
void LCD_byte (unsigned char x)
{
	// The accumulator in the C8051Fxxx is bit addressable!
	ACC=x;
	LCD_D7=ACC_7;
	LCD_D6=ACC_6;
	LCD_D5=ACC_5;
	LCD_D4=ACC_4;
	LCD_D3=ACC_3;
	LCD_D2=ACC_2;
	LCD_D1=ACC_1;
	LCD_D0=ACC_0;
	LCD_pulse(); // At the end, pulse the enable flag to register the message
}


// Write an 8-bit message on the LCD 
// -- param: x, 8 bit character message
void WriteData (unsigned char x)
{
	LCD_RS=1;
	LCD_byte(x);
	waitms(2);
}

// Write an 8-bit instruction to the LCD
// -- param: x, 8 bit character message
void WriteCommand (unsigned char x)
{
	LCD_RS=0;
	LCD_byte(x);
	waitms(5);
}

// Set the LCD in 8 bit mode
void LCD_8BIT (void)
{
	LCD_E=0;  // Resting state of LCD's enable is zero
	LCD_RW=0; // We are only writing to the LCD in this program
	waitms(20);

	// First make sure the LCD is in 8-bit mode
	WriteCommand(0x33);
	WriteCommand(0x33);
	WriteCommand(0x33); // Stay in 8-bit mode

	// Configure the LCD
	WriteCommand(0x38);
	WriteCommand(0x0c);
	WriteCommand(0x01); // Clear screen command (takes some time)
	waitms(20); // Wait for clear screen command to finsih.
}

// The final LCD message registration function
// -- param: string, an 8-bit message to write
//			 line, the line number to write on. 1 for Line1, 2 for Line2. (actually it's just checking '2')
//			 clear, a binary number to clear rest of the line, given '1'
void LCDprint(char * string, unsigned char line, bit clear)
{
	unsigned char j;

	WriteCommand(line==2?0xc0:0x80);
	waitms(5);
	for(j=0; string[j]!=0; j++)	WriteData(string[j]);// Write the message
	if(clear) for(; j<CHARS_PER_LINE; j++) WriteData(' '); // Clear the rest of the line
}

// ===========================================================================
// Add-ons:

// Return the current voltage at the pin number 
// -- param: pinNum, int 
float getVoltage(unsigned int pinNum)
{
	return read_pin(pinNum); // how?
}

// write a message describing the current voltage
// -- param: voltage, float
// 			 voltageStr, the resultant string 
void writeVoltage(float voltage, char * voltageStr){
	char temp[5];
	floatToString(voltage, temp, 1);
	timeStr =  "V: " + temp;
}

// write a message describing the Timer
// -- param: currentTime, int
// 		     timeStr, the resultant string
void writeTimer(float currentTime, char * timeStr){
	char temp[5];
	floatToString(currentTime, temp, 1);
	timeStr =  "T: " + temp;
}

// convert a float value to a string
// param: -- x is the target float number
//		  -- str is the converted string 
// 		  -- decimalPlace is the number of decimal places wanted
void floatToString(float x, char * str, int decimalPlace){
	// integer part:
	int intergerPart = (int) x;

	// float part:
	float floatPart = x - intergerPart;

	// Here count has the valur of the last digit of the integer part:
	int count = intergerToString(intergerPart, char * str, 0);

	// check for display option after point
    if (decimalPlace != 0)
    {
        str[count] = '.';  // add dot
 
        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter is needed
        // to handle cases like 233.007
        floatPart = floatPart * pow(10, decimalPlace);
 
        intergerToString((int)floatPart, str + count + 1, decimalPlace);
    }
}

// Converts a given integer x to string str[].  
// param: -- integer is the integer to be converted
// -- str is the resultant string
// -- d is the number
// of digits required in output. If d is more than the number
// of digits in x, then 0s are added at the beginning.
// returns: -- count, the index of the last char of the string
int integerToString(int integer, char * str, int d)
{
    int count = 0;
    while (integer)
    {
        str[count++] = (integer % 10) + '0';
        integer = integer / 10;
    }
 
    // If number of digits required is more, then
    // add 0s at the beginning
    while (count < d)
        str[count++] = '0';
 
    reverse(str, count);
    str[count] = '\0';
    return count;
}

// reverses a string 'string' of length 'length'
void reverse(char * str, int length)
{
    int i=0, j=length-1, temp;
    while (i < j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}

// reset
int resetTimer(float currentTime){

}

// Here is the main LCD program
// It does the following:
//		1. Display the current battery voltage
//		2. Display the time span from the start of the competition. 
void main (void)
{
	// Here is a little demonstration code to test the LCD is working properly.
	/*
	InitPorts();
	LCD_8BIT();
   	// Display something in the LCD
	LCDprint("LCD 8-bit test:", 1, 1);
	LCDprint("Hello, World!", 2, 1);
	*/

	InitPorts();
	LCD_8BIT();

	float currentTime = 0;
	float clockTimeStep = 3 * MICRO_SECOND; // Here we assume the clock time step is 3 micro seconds;
	char voltageStr[8]; // the message to be displayed
	char timeStr[8];


	// ======================================
	// || Timeer & Battery Voltage Display ||
	// ======================================
	// Here we assume the voltage inputs are:
	// Battery voltage input @ P0M1
	// The battery voltage should be 0 to 6 volt

	// We want to display the voltage at the second line (Cuz it's much longer than time!)

	while(true){
		currentTime = currentTime + clockTimeStep;

		currentBatteryVoltage = getVoltage(P0M1); 
		writeTimer(currentTime, timeStr);
		writeVoltage(currentBatteryVoltage, voltageStr);

		LCDprint(timeStr, 1, 1);
		LCDprint(voltageStr, 2, 1);
	}
	

}
