#ifndef __IERG3810_TFTLCD_H
#define __IERG3810_TFTLCD_H
#include "stm32f10x.h"


void IERG3810_TFTLCD_WrReg(u16);
void IERG3810_TFTLCD_WrData(u16);
void IERG3810_TFTLCD_SetParameter(void);
void IERG3810_TFTLCD_Init(void);
void IERG3810_TFTLCD_DrawDot(u16, u16, u16);
void IERG3810_TFTLCD_DrawLine(u16 , u16 , u16 , u16);
void IERG3810_TFTLCD_FillRectangle(u16, u16, u16, u16, u16);
void IERG3810_TFTLCD_SevenSegment(u16 , u16 , u16 , u8 );
void IERG3810_TFTLCD_ShowChar(u16 , u16 , u8 , u16 , u16 );
void IERG3810_TFTLCD_ShowChinChar(u16 , u16 , u8 , u16 , u16, u8);
void IERG3810_ShowSID(char* , u16, u16, u16, u8);
void IERG3810_ShowName(u16, u16, u16);


#define black                 ((u16)0)  
#define white                 ((u16)65535)  
#define red                 	((u16)59554)  
#define orange                ((u16)64768)  
#define yellow                ((u16)65504)  
#define green                	((u16)12256)  
#define lightGreen            ((u16)2041)  
#define blue            	    ((u16)415)  
#define purple                ((u16)55327)  


#endif
