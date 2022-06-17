#include "stm32f10x.h"

void Delay(u32 count)
{
	u32 i;
	for ( i =0; i<count; i++);

}

int main(void)
{
	//	led ds0: Pin 5
	RCC ->APB2ENR |= 1 << 3;
	GPIOB->CRL &= 0xFF0FFFFF;
	GPIOB->CRL |= 0x00300000;
	
	//	led ds1 : Pin 5
	RCC ->APB2ENR |= 1 << 6;
	GPIOE->CRL &=0xFF0FFFFF;
	GPIOE->CRL |=0x00300000;
	GPIOE->BSRR = 1 << 5;

	//	key1 : Pin 3
	RCC ->APB2ENR |= 1 << 6;
	GPIOE->CRL &= 0xFFFF0FFF;
	GPIOE->CRL |= 0x00008000;
	GPIOE->BSRR|=1<<3;
	
	//	key2 : Pin 2
	RCC ->APB2ENR |= 1 << 6;
	GPIOE->CRL &= 0xFFFFF0FF;
	GPIOE->CRL |= 0x00000800;
	GPIOE->BSRR|=1<<2;
	
	// key_up : Pin 0
	RCC ->APB2ENR |= 1 << 2;
	GPIOA->CRL &=0xFFFFFFF0;
	GPIOA->CRL |=0x00000008;

	
	//	buzzer : Pin 8 
	RCC ->APB2ENR |= 1 << 3;
	GPIOB->CRH &=0xFFFFFFF0;
	GPIOB->CRH |=0x00000003;
	
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