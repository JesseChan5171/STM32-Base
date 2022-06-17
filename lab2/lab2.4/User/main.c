#include "stm32f10x.h"
#include "IERG3810_LED.h"
#include "IERG3810_Buzzer.h"
#include "IERG3810_KEY.h"
#include "IERG3810_USART.h"
#include "IERG3810_Clock.h"

void IERG3810_clock_tree_init(void);
void IERG3810_USART2_init(u32, u32);
void IERG3810_USART1_init(u32, u32);
void Delay(u32);
void USART_print(u8, char *);


int main(void)
{
	IERG3810_clock_tree_init();
	IERG3810_USART2_init(36, 9600);
	IERG3810_USART1_init(72, 9600);
	
	
	while(1)
	{
		USART_print(1, "1155124983");
		
	}
	
}


