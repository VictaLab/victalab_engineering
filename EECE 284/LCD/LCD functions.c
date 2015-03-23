#include            <pic.h>
static bit LCD_RS @ ((unsigned)&PORTE*8+0);  // Register select 
static bit LCD_EN @ ((unsigned)&PORTE*8+2);  // Enable
static bit LCD_RW @ ((unsigned)&PORTE*8+1);  // R/W
#define EN_TRIG ((LCD_EN = 1),(LCD_EN = 0));

//Functions

void DelayMs(int x);
void DelayUs(int x);
void Init_lcd(void);
void Write_com_lcd(unsigned char c);
void Write_char_lcd(unsigned char c);
void Write_string_lcd(const char * s);
void Clear_lcd(void);
void Goto_lcd(unsigned char pos);
void main()

 {
            ADCON1 = 0x0e;  // Set PORTA as digital port
            TRISA = 0;      // Set PORTA as output
			TRISE = 0;		// Set PORTE as output
            TRISD = 0;      // Set PORTD as output
            Init_lcd();     // LCD Initialization
            Clear_lcd();    // Reset LCD and move to start position
            Write_string_lcd("Hello World!! and welcome to www.MicrocontrollerBoard.com");
            while(1);
 } 

/*---------------------------------------------*/

void Init_lcd(void)

{  
            LCD_RW = 0; // write to LCD
            LCD_RS = 0;       // write control bytes
            DelayMs(15);    // power on delay
            PORTD = 0x3; // attention!
            EN_TRIG;
            DelayMs(5);
            EN_TRIG;
            DelayUs(100);
            EN_TRIG;
            DelayMs(5);
            PORTD = 0x2; // set 4 bit mode
            EN_TRIG;
            DelayUs(40);
            Write_com_lcd(0x28);   // 4 bit mode, 1/16 duty, 5x8 font
            Write_com_lcd(0x08);   // display off
            Write_com_lcd(0x0F);   // display on, blink curson on
      		Write_com_lcd(0x06);   // entry mode 
		
}

void DelayMs(int x)

{
            int y = (x*1000)/15;                                           
                        while(--y != 0) 
              continue;                       
}

void      DelayUs(int x)

{
            int y = x/15;                                           
                         while(--y != 0) 
              continue;

}

void Write_com_lcd(unsigned char c)// send command to lcd

{
            LCD_RW = 0;  
            LCD_RS = 0;                         // write the command 
            PORTD = (PORTD & 0xF0) |  (c >> 4);
            EN_TRIG;
            PORTD = (PORTD & 0xF0) |  (c & 0x0F);
            EN_TRIG;
            DelayUs(40);
}

void Write_char_lcd(unsigned char c)

{
            LCD_RW = 0;
            LCD_RS = 1;   // write characters
            PORTD = (PORTD & 0xF0) |  (c >> 4);
            EN_TRIG;
            PORTD = (PORTD & 0xF0) |  (c & 0x0F);
            EN_TRIG;
            DelayUs(40);
}            

void Write_string_lcd(const char * s)

{
            LCD_RS = 1;   // write characters
            while(*s)
            Write_char_lcd(*s++);
		
}

void Clear_lcd(void)

{
            LCD_RS = 0;
            Write_com_lcd(0x1);
            DelayMs(2);
}

void Goto_lcd(unsigned char pos)
{
            LCD_RS = 0;
            Write_com_lcd(0x80+pos);
}
