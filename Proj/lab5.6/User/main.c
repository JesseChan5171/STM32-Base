#include "stm32f10x.h"
#include "IERG3810_LED.h"
#include "IERG3810_Buzzer.h"
#include "IERG3810_KEY.h"
#include "IERG3810_USART.h"
#include "IERG3810_Clock.h"
//#include "FONT.H"
//#include "SevenSegments.h"
//#include "CFONT.H"
#include "global.h"
#include "IERG3810_TFTLCD.h"
#include "IMFONT.H"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

// Variable:
int game_mode = 0;
int game_speed = 0;
//u8 task1HeartBeat = 0;
int a1 = 0;
int b = 0;
int ran;

void IERG3810_clock_tree_init(void);
void IERG3810_USART2_init(u32, u32);
void IERG3810_USART1_init(u32, u32);
void USART_print(u8, char *);
void lose(int);
	
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
	game_mode = 2;
	IERG3810_TFTLCD_FillRectangle(white, 0, 240, 0, 320);
	EXTI ->PR = 1 << 2; 
}

void EXTI0_IRQHandler(void){
	game_mode = 1;
	IERG3810_TFTLCD_FillRectangle(white, 0, 240, 0, 320);
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

void IERG3810_TIM3_Init(u16 arr, u16 psc){
	
	//Tim3, IRQ#29
	RCC->APB1ENR |= 1 << 1;			 
	TIM3->ARR = arr;						 
	TIM3->PSC = psc;			 
	TIM3->DIER |= 1 << 0;			 
	TIM3->CR1 |= 0x01;				 
	NVIC->IP[29] = 0x45;			 
	NVIC->ISER[0] |= (1 << 29);	 
}

void TIM3_IRQHandler(void){
	GPIOB->BRR = 1 << 5;
	GPIOB->BSRR = 1 << 5;
	GPIOB->BRR = 1 << 5;
	GPIOB->BSRR = 1 << 5;	
	GPIOB->ODR ^= 1 << 5;		
	GPIOB->ODR ^= 1 << 5;
	GPIOB->ODR ^= 1 << 5;
	GPIOB->ODR ^= 1 << 5;
	GPIOB->ODR &= ~(1<<5);	
	GPIOB->ODR |= 1<<5;
	GPIOB->ODR &= ~(1<<5);
	GPIOB->ODR |= 1<<5;
	TIM3->SR &= ~(1<<0);
	TIM3->SR &= ~(1<<0); 
}

void IERG3810_TIM4_Init(u16 arr, u16 psc){
	
	//Tim4, IRQ#30
	RCC->APB1ENR |= 1 << 2;		// TIM4	 
	TIM4->ARR = arr;						 
	TIM4->PSC = psc;			 
	TIM4->DIER |= 1 << 0;			 
	TIM4->CR1 |= 0x01;				 
	NVIC->IP[30] = 0x45;			 
	NVIC->ISER[0] |= (1 << 30);	 
}

void TIM4_IRQHandler(void){
	if(TIM4->SR & 1 << 0){ 				 
		GPIOE->ODR ^= 1 << 5;			 
	}
	TIM4->SR &=~(1 << 0);
}

void DS0_turnOff(){
	GPIOB->BSRR = 1<<5;
}

void DS0_turnOff2(){
	DS0_turnOff();
}

void IERG3810_SYSTICK_Init10ms(void){
	
	SysTick->CTRL = 0;
	SysTick->LOAD = 0x00015F90;
	SysTick->CTRL |= 0x00000003;
}


void IERG3810_TIM3_PwmInit(u16 arr, u16 psc){
	RCC->APB2ENR |= 1 << 3;
	GPIOB ->CRL &= 0xFF0FFFFF;
	GPIOB ->CRL |= 0x00B00000;  
	RCC->APB2ENR |= 1 << 0; 
	AFIO->MAPR &= 0xFFFFF3FF;		 
	AFIO->MAPR |= 1 << 11;			 
	RCC->APB1ENR |= 1 << 1;			 
	TIM3->ARR = arr;						 
	TIM3->PSC = psc;						 
	TIM3->CCMR1 |= 7 << 12;	 
	TIM3->CCMR1 |= 1 << 11;			 
	TIM3->CCER |= 1 << 4;				 
	TIM3->CR1 = 0x0080;  	
	TIM3->CR1 |= 0x01;					 
}

void IERG3810_TFTLCD_ShowColud(u16 x, u16 y, u8 ind_char, u16 color, u16 bgcolor)
{
	u8 i,j;
	u8 index;
	u8 height = 16, length = 16;
		
	IERG3810_TFTLCD_WrReg(0x2A);
	IERG3810_TFTLCD_WrData(x >> 8);
	IERG3810_TFTLCD_WrData(x & 0xFF);
	IERG3810_TFTLCD_WrData((x + length - 1) >> 8);
	IERG3810_TFTLCD_WrData((x + length - 1) & 0xFF);
	
	IERG3810_TFTLCD_WrReg(0x2B);
	IERG3810_TFTLCD_WrData(y >> 8);
	IERG3810_TFTLCD_WrData(y & 0xFF);
	IERG3810_TFTLCD_WrData((y + height - 1) >> 8);
	IERG3810_TFTLCD_WrData((y + height - 1) & 0xFF);
	IERG3810_TFTLCD_WrReg(0x2C);
	
	for(j = 0; j < height /8; j++){
		for(i = 0; i < height /2; i++){
			for(index = 0; index < length ; index ++){
				if( (IM[ind_char][index * 2 + 1 - j] >> i) & 0x01 ){
					IERG3810_TFTLCD_WrData(color);
				}else{
					IERG3810_TFTLCD_WrData(bgcolor);
				}
			}
		}
	}
}


void CountFrom3(void){
	u16 i= 3;
	IERG3810_TFTLCD_FillRectangle(white,80, 80, 80,160);
	while(1){
		IERG3810_TFTLCD_SevenSegment(red,80,80,i);
		while(1){if (task1HeartBeat % 100 == 0) break;}
		IERG3810_TFTLCD_FillRectangle(white,80, 80, 80,160);
		if (i == 0){ // 0
			break;
		}
		i--;
	}
}

void IERG3810_ShowPig(u16 y, u16 color, u16 bgcolor)
{
	int i = 0;
	u8 x_inc = 3;
	u8 x_inc_2 = 5;
	u8 x_inc_3 = 8;
	u8 x_inc_bin = 10;
	int pig_x_pos = 50;
	u8 bin_control = 0;
	char str[100];
	int flag = 1;
	int bin_cont = 0;
	int count = 0;
	
	
	CountFrom3();
	IERG3810_TFTLCD_FillRectangle(white, 0, 240, 0, 320);
	
	for (i = 0; i < 9000; i ++){
		
		if (flag == 0){
			lose(i);
		}
		
		
		IERG3810_ShowSID("Score: ", 300, white, black, 0);
		// sprintf(str, "Score: %d, %d, %d", i, bin_cont, pig_x_pos);
		// IERG3810_ShowSID(str, 300, white, black, 0);
		
		
		
		// bin_cont = 200-x_inc_bin*i*game_speed-bin_control
		
		if (bin_cont < 0){
			count = 0;
			
			bin_cont = 250-x_inc_bin*count*game_speed-bin_control;
			
		}
		else{
			bin_cont = 250-x_inc_bin*count*game_speed-bin_control;
		}
		
		if (pig_x_pos < 0){
			
			pig_x_pos = 8;
		}
		
		
		IERG3810_TFTLCD_ShowColud(pig_x_pos, 100, 2,color,color); // clear pig2
		pig_x_pos = pig_x_pos - 8;
		
		ran = rand() % (230+1 - 200) + 200;
		IERG3810_TFTLCD_ShowColud(140-x_inc*i, y, 0,color,bgcolor); // Cloud
		IERG3810_TFTLCD_ShowColud(140-x_inc_2*i, ran, 0,color,bgcolor); // cloud 2
		IERG3810_TFTLCD_ShowColud(140-x_inc_3*i, ran+20, 0,color,bgcolor); // cloud 3
		IERG3810_TFTLCD_FillRectangle(black, 0, 240, 290, 320);
		
		//IERG3810_TFTLCD_ShowColud(pig_x_pos, 100, 2,white,white); // clear pig2
		IERG3810_TFTLCD_ShowColud(pig_x_pos, 100, 1,color,bgcolor); // pig1
		
		IERG3810_TFTLCD_ShowColud(bin_cont, 100, 3,color,bgcolor); //bin
		
		while(1){
			if (task1HeartBeat % 30 == 0){
					IERG3810_TFTLCD_ShowColud(140-x_inc*i, y, 0,color,color); // Cloud
					IERG3810_TFTLCD_ShowColud(140-x_inc_2*i, ran, 0,color,color); // cloud 2
					IERG3810_TFTLCD_ShowColud(140-x_inc_3*i, ran+20, 0,color,color); // cloud 3
					IERG3810_TFTLCD_ShowColud(bin_cont, 100, 3,color,color); // Clear Bin
					bin_control = 0;
					break;
			
			}
				
			if (task1HeartBeat % 20 == 0){
					IERG3810_TFTLCD_ShowColud(pig_x_pos, 100, 1,color,color); // clear pig1
					IERG3810_TFTLCD_ShowColud(pig_x_pos, 100, 2,color,bgcolor); // make pig2
				
			}
			

			
			if(ps2count >= 11){
			// Jump 
					if(ps2key == 0x69){
						IERG3810_TFTLCD_ShowColud(pig_x_pos, 100, 1,color,color); // clear pig1
						IERG3810_TFTLCD_ShowColud(pig_x_pos, 100, 2,color,color); // clear pig2
						IERG3810_TFTLCD_ShowColud(pig_x_pos+30, 150, 2,color,bgcolor); // make pig2
						while(1){if (task1HeartBeat % 20 == 0) break;}
						
						
						IERG3810_TFTLCD_ShowColud(pig_x_pos+30, 150, 2,color,color); // clear pig2
						IERG3810_TFTLCD_ShowColud(pig_x_pos+50, 100, 2,color,bgcolor); // make pig2
						pig_x_pos = pig_x_pos + 50;
						
						//while(1){if (task1HeartBeat % 60 == 0) break;}
					}
					ps2key = 0;
					ps2count = 0;
					EXTI->PR = 1 << 11; 
					bin_control = 0;
					IERG3810_TFTLCD_ShowColud(bin_cont, 100, 3,color,color); // Clear Bin
					// bin_cont = bin_cont - 30;
			}
			
			sprintf(str, "Score: %d", i);
			IERG3810_ShowSID(str, 300, white, black, 0);
			
			if(abs(bin_cont - pig_x_pos) < 4){
				flag = 0;
			}
			
		}
		count = count + 1;
					
	} 
}
void lose(int score){
	char sc_str[100];

	sprintf(sc_str, "Lose, Score: %d \n", score-1);
	USART_print(1, sc_str); // output to the pc
	
	
	IERG3810_TFTLCD_FillRectangle(red, 0, 250, 0, 280);
	IERG3810_ShowSID("Lose!",150, black, red, 100);
	IERG3810_ShowSID("Press 1 to restart.",130, black, red, 20);
	GPIOB ->BRR = 1 << 5; // red led light 
	while(1){		
		if(ps2count >= 11){
			if(ps2key == 0x69){
				if(ps2key == 0x69){
				IERG3810_TFTLCD_FillRectangle(white, 0, 240, 0, 320);
				game_mode = 0;
				main();
				// IERG3810_ShowPig(250,white, black);
				}
		}
		
	}
}
}

void game_start(){

	//IERG3810_TFTLCD_FillRectangle(yellow, 0, 240, 0, 320);
	while(1){
		IERG3810_TFTLCD_FillRectangle(white, 0, 240, 0, 320);
		
		IERG3810_ShowSID("Score: ", 300, white, black, 0);
		
		IERG3810_ShowPig(250,white, black);
	
		
	
	}

}



int main(void){
	
	IERG3810_LED_Init();
	IERG3810_clock_tree_init();
	IERG3810_USART1_init(72,9600);
	IERG3810_USART2_init(36,9600);
	IERG3810_NVIC_SetPriorityGroup(5);
	IERG3810_PS2key_ExtInit();
	IERG3810_LED_Init();
	IERG3810_TFTLCD_Init();
	IERG3810_SYSTICK_Init10ms();
	IERG3810_key2_ExtInit();
	IERG3810_keyUP_ExtInit();
	

	while(1) {
			GPIOB ->BSRR = 1 << 5;
			GPIOE ->BSRR = 1 << 5;
	
		if (game_mode == 0) {
			
			IERG3810_ShowSID("Group 19", 280, white, black, 70);
			IERG3810_ShowName(250,white, black);
			IERG3810_ShowSID("SID: 1155124983", 230, white, black, 70);
			
			IERG3810_ShowSID("Derek IP", 210, white, black, 70);
			IERG3810_ShowSID("SID: 1155144668", 190, white, black, 70);
			
			
			IERG3810_ShowSID("Press 1 to jump in the game", 150, white, black, 0);
			
			IERG3810_ShowSID("Difficulty Levels: ", 120, white, black, 0);
			IERG3810_ShowSID("Press keyUp for easy mode", 100, white, black, 0);
			IERG3810_ShowSID("Press key2 for hard mode", 80, white, black, 0);
			
			
			
			//easy mode, keyUp, exti0
		} else if (game_mode == 1){  
			game_speed = 1;
			USART_print(1, "Game Start\n");
			IERG3810_TFTLCD_FillRectangle(white, 0, 240, 0, 320);
			game_start();
			
			// hard mode:
		} else if (game_mode == 2) {
			USART_print(1, "Game Start\n");
			game_speed = 2;
			//IERG3810_TFTLCD_FillRectangle(white, 0, 240, 0, 320);
			
			game_start();
		}
		
	}
	
	
}

