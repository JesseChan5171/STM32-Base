#include "stm32f10x.h"
#include "IERG3810_LED.h"
#include "IERG3810_Buzzer.h"
#include "IERG3810_KEY.h"
#include "IERG3810_USART.h"
#include "IERG3810_Clock.h"
#include "FONT.H"
#include "SevenSegments.h"
#include "CFONT.H"

void IERG3810_clock_tree_init(void);
void IERG3810_USART2_init(u32, u32);
void IERG3810_USART1_init(u32, u32);
void Delay(u32);
void USART_print(u8, char *);

void Delay(u32 count){
	u32 i;
	for(i = 0; i < count; i++);
}

void IERG3810_key2_ExtInit(){
	// EXTI-2
	RCC->APB2ENR |= 1 << 6;
	GPIOE ->CRL &= 0xFFFFF0FF;
	GPIOE ->CRL |= 0x00000800;
	GPIOE ->BSRR = 1 << 2;
	RCC->APB2ENR |= 0x01;  
	AFIO ->EXTICR[0] &= 0xFFFFF0FF; 
	AFIO ->EXTICR[0] |= 0x00000400; 
	EXTI ->IMR |= 1 << 2;  
	EXTI ->FTSR |= 1 << 2; 
	//EXTI ->RTSR |= 1 << 2; 
	
	NVIC ->IP[8] = 0x65;  
	NVIC ->ISER[0] &= ~(1 << 8);	 
	NVIC ->ISER[0] |= (1 << 8);	 
	
}

void IERG3810_NVIC_SetPriorityGroup(u8 prigroup){
	u32 temp, temp1;
	temp1 = prigroup & 0x00000007;
	temp1 <<= 8; 
	temp = SCB ->AIRCR;
	temp &= 0x0000F8FF;
	temp |= 0x05FA0000;
	temp |= temp1;
	SCB ->AIRCR = temp;
}

void EXTI2_IRQHandler(void){
	u8 i;
	for(i = 0; i < 10; i++){
		GPIOB ->BRR = 1 << 5; 
		Delay(1000000);
		GPIOB ->BSRR = 1 << 5; 
		Delay(1000000);
	}
	EXTI ->PR = 1 << 2; 
}

void EXTI0_IRQHandler(void){
	u8 i;
	for(i = 0; i < 10; i++){
		GPIOE ->BRR = 1 << 5; // on
		Delay(1000000);
		GPIOE ->BSRR = 1 << 5; // off
		Delay(1000000);
	}
	EXTI ->PR = 1 << 0; 
}



void IERG3810_keyUP_ExtInit(){
	//PA0, KeyUp press  = high, EXTI-0
	RCC->APB2ENR |= 1 << 2;
	GPIOA ->CRL &= 0xFFFFFFF0;
	GPIOA ->CRL |= 0x00000008;
	GPIOA ->ODR |= 1 << 4;
	
	RCC->APB2ENR |= 0x1; 
	AFIO ->EXTICR[0] &= 0xFFFFFFF0; //EXTI-0
	AFIO ->EXTICR[0] |= 0x00000000; //EXTI-0, 0000: PA[x] pin
	
	EXTI ->IMR |= 0x1; //0001 not mask on line 0
	EXTI ->FTSR |= 0x1; // Falling trigger enabled
	
	NVIC ->IP[6] = 0x95; //priority = 0x95
	NVIC ->ISER[0] &= ~(1 << 6);	//enable IRQ 6 for EXTI0
	NVIC ->ISER[0] |= (1 << 6);	//IRQ 6
}

void IERG3810_PS2key_ExtInit(){
	// PS2 data : PC10, PS2 CLK: PC11
	
	RCC->APB2ENR |= 1 << 4;
	GPIOC ->CRH &= 0xFFFF00FF;//PC10, PC11
	GPIOC ->CRH |= 0x00008800; // 1000
	
	GPIOC ->BSRR = 1 << 11; // set hight
	GPIOC ->BSRR = 1 << 10; 
	
	RCC->APB2ENR |= 0x01; 
	AFIO ->EXTICR[2] &= 0xFFFF0FFF; //EXTI11 
	AFIO ->EXTICR[2] |= 0x00002000;// 0010: PC11 pin
	
	EXTI ->IMR |= 1 << 11; // Event request from Line x is not masked
	EXTI ->FTSR |= 1 << 11;// Falling trigger enabled
	
	NVIC ->IP[40] = 0x65; //priority for IRQ 40
	NVIC ->ISER[1] |= (1 << 8);	//enable IRQ 40 for EXTI[15:10]
}

u32 sheep = 0;
u32 timeout = 10000;
u32 ps2key = 0;
u32 tmp = 0;
u32 ps2count = 0;
u8 ps2dataReady = 0;
u8 key_stack[2];


void EXTI15_10_IRQHandler(void){
	
	if (ps2count > 0 && ps2count < 9){ //1:8 , bit 0:7 data
	
		tmp = ps2key >>= 1; //right shift 1
		if ((GPIOC->IDR)&(1<<10)){
			tmp |= 0x80;
		}
	ps2key = tmp;
	ps2count++;
	}
	
	else {
		ps2count++;
	}
	
	
	Delay(10);
	EXTI->PR = 1 << 11;
}

int main(void){
	IERG3810_clock_tree_init();
	//IERG3810_USART2_init(36,9600);
	IERG3810_LED_Init();
	IERG3810_NVIC_SetPriorityGroup(5);
	IERG3810_key2_ExtInit();
	IERG3810_keyUP_ExtInit();
	IERG3810_PS2key_ExtInit();
	//USART_print(2, "123457890");
	GPIOB ->BSRR = 1 << 5; //DS0 OFF
	GPIOE ->BSRR = 1 << 5; //DS1 OFF
	
	
while(1){
		
		sheep++;
		if(ps2count >= 11){
			if(ps2key==0X73){
				GPIOB->BSRR = 1 << 21;
			}
			else{
				GPIOB->BSRR = 1 << 5;
			}
			if(ps2key==0X74){
					
				GPIOE->BSRR = 1 << 21;
			}
			else{
				GPIOE->BSRR = 1 << 5;
			}
			EXTI ->PR = 1 << 11;
		}
		if(timeout == 0){
			timeout = 20000;
			ps2key = 0;
			ps2count = 0;
		}
		timeout --;
		
	}
}

