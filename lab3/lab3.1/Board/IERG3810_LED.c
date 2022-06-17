#include "stm32f10x.h"
#include "IERG3810_LED.h"

// put your procedure and code here
void IERG3810_LED_Init(){
	//	led ds0: Pin 5
	RCC ->APB2ENR |= 1 << 3;
	GPIOB->CRL &= 0xFF0FFFFF;
	GPIOB->CRL |= 0x00300000;


	//	led ds1 : Pin 5
	RCC ->APB2ENR |= 1 << 6;
	GPIOE->CRL &=0xFF0FFFFF;
	GPIOE->CRL |=0x00300000;
	GPIOE->BSRR = 1 << 5;

}