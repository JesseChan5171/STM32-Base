#include "stm32f10x.h"
#include "IERG3810_LED.h"
#include "IERG3810_Buzzer.h"
#include "IERG3810_KEY.h"

void Delay(u32 count)
{
	u32 i;
	for ( i =0; i<count; i++);

}

int main(void)
{
	IERG3810_LED_Init();
	IERG3810_KEY_Init();
	IERG3810_Buzzer_Init();

	
	while(1)
	{
			
		// check key2 input and trigger led ds0
		if (!(GPIOE -> IDR & 1 << 2))
		{
			GPIOB->BRR = 1 << 5;
		}
		else
		{
			GPIOB ->BSRR = 1 << 5;
		}
		
		//check key 1 and trigger led ds1
		if (!(GPIOE -> IDR & 1 << 3))
		{
			if (GPIOE -> ODR & 1 << 5){ // check ds1 state
				GPIOE -> BRR = 1 << 5;
			}
			else{
				GPIOE -> BSRR = 1 << 5;
			}
			Delay(5000000);
		}
		
		// key_up
		if (GPIOA -> IDR & 1 << 0)
		{
			if (GPIOB -> ODR & 1 << 8){ 
				GPIOB -> BRR = 1 << 8;
			}
			else{
				GPIOB -> BSRR = 1 << 8;
			}
			Delay(5000000);
		} 
		
		
	}
	
	
}