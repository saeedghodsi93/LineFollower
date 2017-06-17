#ifndef __TOUCH_H__
#define __TOUCH_H__
#include "Touch_DEF.h"

typedef struct 
{
	u16 X0;
	u16 Y0;
	u16 X;
	u16 Y;
	u16 xoff0;
	u16 xoff1;
	u16 yoff0;
	u16 yoff1;						   	    
	u8  Key_Sta;			  
	float xfac;
	float yfac;
	short xoff;
	short yoff;
}Pen_Holder;

extern Pen_Holder Pen_Point; 

extern u8	pass_len;

extern void Touch_Init(void);		   
extern u8   Read_ADS(u16 *x,u16 *y);
extern u8   Read_ADS2(u16 *x,u16 *y); 
extern u16  ADS_Read_XY(u8 xy);		
extern u16  ADS_Read_AD(u8 CMD);	 
extern void ADS_Write_Byte(u8 num); 
extern void Drow_Touch_Point(u16 x,u16 y);
extern void Draw_Big_Point(u16 x,u16 y);  
extern void Touch_Adjust(void);         
extern void Save_Adjdata(void);		 
extern u8   Get_Adjdata(void); 		
extern void Pen_Int_Set(u8 en); 		 
extern void Convert_Pos(void);           
extern void InitializeSPI(void);
extern void delay1us(unsigned int delay1usdata);
extern void delay1ms(unsigned int delay1msdata);
extern void SpiDelay(unsigned int DelayCnt);

extern void EINT0_IRQHandler(void);
extern uint32_t EINTInit( void );

extern int Touch_PassKeyboard (uint8_t *pass); 
extern int Touch_ReadPass (uint8_t  *pass);

extern int Touch_ReadGeneralKeyboard (void); 
extern int Touch_ReadModesKeyboard (void);
extern int Touch_ReadMusicKeyboard (void);

#endif
