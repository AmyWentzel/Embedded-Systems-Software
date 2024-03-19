#include "lcd.h"
#include "utility.h"
#include <stdarg.h>

void LCD_GPIO_Init(void)
{
	RCC -> APB1ENR |= RCC_APB1ENR_USART2EN;
	RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;
}


void LCD_Init(void)
{
	LCD_GPIO_Init();
	// Get ready for LCD Comm

	CLEAR_BITS(LCD_PORT, LCD_PORT_BITS);		// Clear bits on LCD port
	
	LCD_E_LO;  	// Set E LO, RS to Instruction
	LCD_RS_IR;
	
	Delay_ms(10);						// Wait for 10ms (arbitrary)
	
	
	//Syncing Sequence 1: Send 0x03 on Data Bus, wait 5ms
	LCD_E_HI;
	LCD_BUS(0x03UL);
	LCD_E_LO;
	Delay_ms(5);	
	
	//Syncing Sequence 2: Send 0x03 on Data Bus, wait 1ms
	LCD_E_HI;
	LCD_BUS(0x03UL);
	LCD_E_LO;
	Delay_ms(1);
	
	//Syncing Sequence 3: Send 0x03 on Data Bus, NO WAIT
	LCD_E_HI;
	LCD_BUS(0x03UL);
	LCD_E_LO;
	
	//Syncing Sequence 4: Send 0x02 on Data Bus, NO WAIT
	LCD_E_HI;
	LCD_BUS(0x02UL);
	LCD_E_LO;
	
	// Set LCD up
	// Set Function Command for 4-bit mode, 2LINE, 5x8 font
	LCDcmd(LCD_CMD_FUNCTION | LCD_FUNCTION_5X8FONT | LCD_FUNCTION_2LINES | LCD_FUNCTION_4BITBUS); //00101000
	
	
	// Set Display Command to OFF
	LCDcmd( LCD_CMD_DISPLAY | LCD_CMD_CLEAR );
	
	// Set Command to CLEAR
	LCD_Clear();
	
	// Set Entry Command to MOVECURSOR and INCREMENT MODE
	LCDcmd( LCD_CMD_ENTRY | LCD_ENTRY_MOVE_CURSOR | LCD_ENTRY_INC );
	
	// Set Display Command to ON, NOCURSOR, NOBLINK
	LCDcmd( LCD_CMD_DISPLAY | LCD_DISPLAY_NOBLINK | LCD_DISPLAY_NOCURSOR );
	
	
	// All done!
	
}

void LCD_Clear(void)
{
	Delay_ms(LCD_STD_CMD_DELAY);
	LCDcmd(LCD_CMD_CLEAR);
	Delay_ms(LCD_LONG_CMD_DELAY);
}
void LCD_HomeCursor(void)
{
	LCDcmd(LCD_CMD_HOME);
	Delay_ms(LCD_LONG_CMD_DELAY);
}

void LCDcmd(uint8_t cmd)
{
	Delay_ms(LCD_STD_CMD_DELAY);
	
	LCD_E_LO;
	LCD_RS_IR;
	
	LCD_E_HI;
	LCD_BUS(HI_NYBBLE(cmd));
	LCD_E_LO;
	
	LCD_E_HI;
	LCD_BUS(LO_NYBBLE(cmd));
	LCD_E_LO;
}


void LCDdata(uint8_t data);

void LCDputc(unsigned char ch);

void LCDputs(char *str);

void LCDprintf(char *str,...);