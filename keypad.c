/*************************************************************************
FILE NAME: keypad.c
DESCRIPTION: Keypad Init, Scan, and Debounce Functions
AUTHOR: Scott Chen, Amy Wentzell
DATE: March 5, 2024

*************************************************************************/

#include "keypad.h"
#include "uart.h"
#include "pb.h"
#include "utility.h"

#define TRUE 1
#define FALSE 0 //for debouncer


//Row-Column Record (should be static)
static int rowNum = 5;
static int colNum;


// [rowNum][colNum]
char Input[4][4] = {
	{'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'},
	{'7', '8', '9', 'C'},
	{'*', '0', '#', 'D'},
};


void Keypad_Init(void)
{
	RCC -> AHBENR |= RCC_AHBENR_GPIOBEN;  //enable GPIO Port B
	
	CLEAR_BITS(GPIOx(B) -> PUPDR, 0xFFFFUL); //CLEAR PB0 - PB7
	
	SET_BITS(GPIOx(B) -> PUPDR, 0x5500UL); //PB0 - PB3 to no pull, PB4 - PB7 to pull up
	
	CLEAR_BITS(GPIOx(B) -> MODER, 0xFFFFUL); // CLEAR PB0 - PB7
	
	SET_BITS(GPIOx(B) -> MODER, 0x0055UL); // PB0 - PB3 to output, PB4 - PB7 to input	
	
	CLEAR_BITS(GPIOx(B) -> OTYPER, 0xFFUL); // CLEAR PB4 - PB7 
	
	SET_BITS(GPIOx(B) -> OTYPER, 0xFUL); //PB0 - PB3 to open drain
}


void debounce(void)
{
	Delay_ms(10);
}

char scanKey(void)
{
	unsigned char columnReading;
	unsigned char rowReading;
	
	//1. Set R1 - R4 = 0 (write 0x0 into PB0 - PB3)
		//1.1 Reset the Row-Column Record (both set to 0)
		CLEAR_BITS(GPIOx(B)->ODR, 0xFUL); //rows
	//2. Read C1 - C4 (PB4 - PB7)
		//2.1 If all 1 (0xF), no key is pressed
			// return '\0' (NULL char)
			// exit the function 
	 if ((GPIOx(B)->IDR & 0xF0) == 0xF0)
		 return('\0');

	//2.2 If not 0xF, convert:
		//0xE (0x1110) = PB4 or C1 is ON
		//0xD (0x1101) = PB5 or C2 is ON
	//0xB (0x1011) = PB6 or C3 is ON
	//0x7 (0x0111) = PB7 or C4 is ON
	 
	 columnReading = GPIOB->IDR & (0xF0);
	 
	 switch (columnReading)
	 {
		case 0xE0:
			colNum = 0;
			break;
		case 0xD0:
			colNum = 1;
			break;
		case 0xB0:
			colNum = 2;
			break;
		case 0x70:
			colNum = 3;
			break;
		default:
			break;
	 }
	
	 //UARTprintf(" COL NUM: %d\n\n", colNum);
		//return Column (C#) number
		//return(colNum);
	
	//Start Row Scanning
	
	
	//3. Debounce (call debounce())
			debounce();
			
			
	//4. Row Scanning
		//4.1 	Set PB0 - PB3 to 0xE
			CLEAR_BITS(GPIOx(B) -> ODR, 0xF);
			SET_BITS(GPIOx(B) -> ODR, 0xE);
			Delay_ms(10);
			// This targets row 1 
			// Monitor the column number (C#)
			// If C# turns 0, then record rowNum = 0
			columnReading = (GPIOB->IDR & 0xF0);
			if (columnReading != 0xF0)
				rowNum = 0;
			
			// break
			// OTHERWISE next row

		//4.2 	Set PB0 - PB3 to 0xD
			CLEAR_BITS(GPIOx(B) -> ODR, 0xF);
			SET_BITS(GPIOx(B)->ODR, 0xD);
			Delay_ms(10);
			// This targets row 2
			// Monitor the column number (C#)
			// If C# turns 0, then record rowNum = 1
			columnReading = (GPIOB->IDR & 0xF0);
			if (columnReading != 0xF0)
				rowNum = 1;
			// break
			// OTHERWISE next row
			
		//4.3 	Set PB0 - PB3 to 0xB
			CLEAR_BITS(GPIOx(B) -> ODR, 0xF);
			SET_BITS(GPIOx(B)->ODR, 0xB);
			Delay_ms(10);
			// This targets row 3
			// Monitor the column number (C#)
			// If C# turns 0, then record rowNum = 2
			columnReading = (GPIOB->IDR & 0xF0);
			if (columnReading != 0xF0)
				rowNum = 2;
			// break
			// OTHERWISE next row
			
		//4.4 	Set PB0 - PB3 to 0x7
			CLEAR_BITS(GPIOx(B) -> ODR, 0xF);
			SET_BITS(GPIOx(B)->ODR, 0x7);
			Delay_ms(10);
			// This targets row 4
			// Monitor the column number (C#)
			// If C# turns 0, then record rowNum = 3
			columnReading = (GPIOB->IDR & 0xF0);
			if (columnReading != 0xF0)
				rowNum = 3;
			else if (rowNum == 5)
				return('\0');
			// BUT IF C# is still 1
			// break
		
		//4.5 Return NULL ('\0')
			
	//After Step 4, we know the matrix of the input 
	
	//5. Use C# and R# to look for corresponding chars
		//5.1 return Input[rowNum][colNum]
		//UARTprintf("ROW NUM: %d\n\n", rowNum);
		return(Input[rowNum][colNum]);
			
}


