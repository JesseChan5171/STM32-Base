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

void Delay(u32 count){
	u32 i;
	for(i = 0; i < count; i++);
}

typedef struct{
	u16 LCD_REG;
	u16 LCD_RAM;
}	LCD_TypeDef;


#define LCD_BASE	((u32)(0x6C000000	|	0x000007FE))
#define LCD				((LCD_TypeDef*)	LCD_BASE)

// Color
#define black                 (u16)	0 
#define white                 (u16)	65535 
#define green                	(u16)	12256
#define red                 	(u16)	59554 
#define blue            	    (u16)	415


void IERG3810_TFTLCD_WrReg(u16 regval){
	LCD->LCD_REG = regval;
}
void IERG3810_TFTLCD_WrData(u16 data){
	LCD->LCD_RAM = data;
}


void IERG3810_TFTLCD_SetParameter(void){
	IERG3810_TFTLCD_WrReg(0x01);
	IERG3810_TFTLCD_WrReg(0x11);
	
	IERG3810_TFTLCD_WrReg(0x3A);
	IERG3810_TFTLCD_WrData(0x55);		
	
	IERG3810_TFTLCD_WrReg(0x29);
	
	IERG3810_TFTLCD_WrReg(0x36);
	IERG3810_TFTLCD_WrData(0xCA);
}




void IERG3810_TFTLCD_Init(void){
	RCC ->AHBENR |= 1 << 8;	
	RCC ->APB2ENR |= 1 << 3;
	RCC ->APB2ENR |= 1 << 5;
	RCC ->APB2ENR |= 1 << 6;
	RCC ->APB2ENR |= 1 << 8;
	GPIOB ->CRL &= 0xFFFFFFF0;
	GPIOB ->CRL |= 0x00000003;
	
	//PORTD
	GPIOD ->CRH &= 0x00FFF000;
	GPIOD ->CRH |= 0xBB000BBB;
	GPIOD ->CRL &= 0xFF00FF00;
	GPIOD ->CRL |= 0x00BB00BB;
	
	//PORTE
	GPIOE ->CRH &= 0x00000000;
	GPIOE ->CRH |= 0xBBBBBBBB;
	GPIOE ->CRL &= 0x0FFFFFFF;
	GPIOE ->CRL |= 0xB0000000;
	
	//PORTG12
	GPIOG ->CRH &= 0xFFF0FFFF;	
	GPIOG ->CRH |= 0x000B0000; 
	GPIOG ->CRL &= 0xFFFFFFF0;
	GPIOG ->CRL |= 0x0000000B;
	
	
	FSMC_Bank1->BTCR[6] = 0x00000000;
	FSMC_Bank1->BTCR[7] = 0x00000000;
	FSMC_Bank1E ->BWTR[6] = 0x00000000;
	FSMC_Bank1 ->BTCR[6] |= 1 << 12;
	FSMC_Bank1 ->BTCR[6] |= 1 << 14;
	FSMC_Bank1 ->BTCR[6] |= 1 << 4;	
	FSMC_Bank1 ->BTCR[7] |= 0 << 28;
	FSMC_Bank1 ->BTCR[7] |= 1 << 0;	
	FSMC_Bank1 ->BTCR[7] |= 0xF << 8;
	FSMC_Bank1E ->BWTR[6] |= 0 << 28;
	FSMC_Bank1E ->BWTR[6] |= 0 << 0;	
	FSMC_Bank1E ->BWTR[6] |= 3 << 8;
	FSMC_Bank1 ->BTCR[6] |= 1 << 0;	
	
	IERG3810_TFTLCD_SetParameter();
	GPIOB ->ODR |= 1 << 0;
	//LCD_LIGHT_ON;
}

void IERG3810_TFTLCD_DrawDot(u16 x, u16 y, u16 color){
	
	IERG3810_TFTLCD_WrReg(0x2A);
	IERG3810_TFTLCD_WrData(x >> 8);
	IERG3810_TFTLCD_WrData(x & 0xFF);
	IERG3810_TFTLCD_WrData(0x01);
	IERG3810_TFTLCD_WrData(0x3F);
	IERG3810_TFTLCD_WrReg(0x2B);
	IERG3810_TFTLCD_WrData(y >> 8);
	IERG3810_TFTLCD_WrData(y & 0xFF);
	IERG3810_TFTLCD_WrData(0x01);
	IERG3810_TFTLCD_WrData(0xDF);
	IERG3810_TFTLCD_WrReg(0x2C);
	IERG3810_TFTLCD_WrData(color);
}

void IERG3810_TFTLCD_DrawLine(u16 x_St, u16 x_End, u16 y, u16 color){
	u16 x = 0;
	for(x = x_St; x < x_End; x++){
		IERG3810_TFTLCD_DrawDot(x,y,color);
	}
}


int main(void)
{	
	IERG3810_LED_Init();
	IERG3810_TFTLCD_Init();
	
	Delay(2000000);
	
	
	IERG3810_TFTLCD_DrawLine(10,30,10,black);	
	IERG3810_TFTLCD_DrawLine(10,30,20,white);	
	IERG3810_TFTLCD_DrawLine(10,30,30,green);
	IERG3810_TFTLCD_DrawLine(10,30,40,red);	
	IERG3810_TFTLCD_DrawLine(10,30,50,blue);	

	
}


