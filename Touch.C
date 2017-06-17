#include "lpc17xx.h"
#include "stdint.h"
#include "touch.h" 
#include "glcd.h"
#include "stdlib.h"
#include "math.h"
#include "sys.h"
#include "i2c.h"

const int touch_x1_l = 1*CHAR_W;
const int touch_x1_h = touch_x1_l+3*CHAR_W;
const int touch_x2_l = 5*CHAR_W;
const int touch_x2_h = touch_x2_l+3*CHAR_W;
const int touch_x3_l = 9*CHAR_W;
const int touch_x3_h = touch_x3_l+3*CHAR_W;
				
const int touch_y1_l = 1*CHAR_H-OFFSET_U;
const int touch_y1_h = touch_y1_l+CHAR_H+OFFSET_L;
const int touch_y2_l = 3*CHAR_H-OFFSET_U;					  
const int touch_y2_h = touch_y2_l+CHAR_H+OFFSET_L;
const int touch_y3_l = 5*CHAR_H-OFFSET_U;
const int touch_y3_h = touch_y3_l+CHAR_H+OFFSET_L;
const int touch_y4_l = 7*CHAR_H-OFFSET_U;
const int touch_y4_h = touch_y4_l+CHAR_H+OFFSET_L;

const int touch_general_x1_l = 1*CHAR_W;
const int touch_general_x1_h = touch_general_x1_l+11*CHAR_W;

const int touch_general_y1_l = 1*CHAR_H-OFFSET_U;
const int touch_general_y1_h = touch_general_y1_l+CHAR_H+OFFSET_L;
const int touch_general_y2_l = 3*CHAR_H-OFFSET_U;
const int touch_general_y2_h = touch_general_y2_l+CHAR_H+OFFSET_L;
const int touch_general_y3_l = 5*CHAR_H-OFFSET_U;
const int touch_general_y3_h = touch_general_y3_l+CHAR_H+OFFSET_L;

const int touch_modes_x1_l = 1*CHAR_W;
const int touch_modes_x1_h = touch_modes_x1_l+6*CHAR_W;

const int touch_modes_y1_l = 1*CHAR_H-OFFSET_U;
const int touch_modes_y1_h = touch_modes_y1_l+CHAR_H+OFFSET_L;
const int touch_modes_y2_l = 3*CHAR_H-OFFSET_U;
const int touch_modes_y2_h = touch_modes_y2_l+CHAR_H+OFFSET_L;
const int touch_modes_y3_l = 5*CHAR_H-OFFSET_U;
const int touch_modes_y3_h = touch_modes_y3_l+CHAR_H+OFFSET_L;

const int touch_music_x1_l = 1*CHAR_W;
const int touch_music_x1_h = touch_music_x1_l+6*CHAR_W;;
const int touch_music_x2_l = 10*CHAR_W;
const int touch_music_x2_h = touch_music_x2_l+6*CHAR_W;;
				
const int touch_music_y1_l = 1*CHAR_H-OFFSET_U;
const int touch_music_y1_h = touch_music_y1_l+CHAR_H+OFFSET_L;
const int touch_music_y2_l = 3*CHAR_H-OFFSET_U;
const int touch_music_y2_h = touch_music_y2_l+CHAR_H+OFFSET_L;
const int touch_music_y3_l = 5*CHAR_H-OFFSET_U;
const int touch_music_y3_h = touch_music_y3_l+CHAR_H+OFFSET_L;
const int touch_music_y4_l = 7*CHAR_H-OFFSET_U;
const int touch_music_y4_h = touch_music_y4_l+CHAR_H+OFFSET_L;

Pen_Holder Pen_Point;
typedef signed long  s32;

u8 pass_len;

void delay_us(unsigned int delay_usdata)
{
	while(delay_usdata--);
}


void delay_ms(unsigned int delay_msdata)
{
	unsigned int i,j,k;
	for(i=0;i<delay_msdata;i++)
		for(j=0;j<1000;j++)
			for(k=0;k<100;k++);
}


//ba'd az yekbar adjust shodan tu in address ha minevise mokhtassat ro
void Save_Adjdata(void)
{
	s32 temp_data;
	u8  temp[8];

	temp_data=Pen_Point.xfac*100000000;
	temp[0]=(u8)(temp_data & 0xff);
	temp[1]=(u8)((temp_data >> 8) & 0xff);
	temp[2]=(u8)((temp_data >> 16) & 0xff);	
	temp[3]=(u8)((temp_data >> 24) & 0xff);		      
    I2C_WriteNByte(0xA0,1,0X00,temp,8);
	 
	temp_data=Pen_Point.yfac*100000000;
	temp[0]=(u8)(temp_data & 0xff);
	temp[1]=(u8)((temp_data >> 8) & 0xff);
	temp[2]=(u8)((temp_data >> 16) & 0xff);	
	temp[3]=(u8)((temp_data >> 24) & 0xff);			       	      
    I2C_WriteNByte(0xA0,1,0X08,temp,8);

	temp_data=Pen_Point.xoff;
	temp[0]=(u8)(temp_data &0xff);
	temp[1]=(u8)((temp_data >>8)&0xff);
    I2C_WriteNByte(0xA0,1,0X16,temp,8);

	temp_data=Pen_Point.yoff;
	temp[0]=(u8)(temp_data &0xff);
	temp[1]=(u8)((temp_data >>8)&0xff);
    I2C_WriteNByte(0xA0,1,0X24,temp,8);

	temp_data=Pen_Point.xoff0;
	temp[0]=(u8)(temp_data & 0xff);
	temp[1]=(u8)((temp_data >> 8) & 0xff);
	temp[2]=(u8)((temp_data >> 16) & 0xff);	
	temp[3]=(u8)((temp_data >> 24) & 0xff);		      
    I2C_WriteNByte(0xA0,1,0X32,temp,8);

	temp_data=Pen_Point.xoff1;
	temp[0]=(u8)(temp_data & 0xff);
	temp[1]=(u8)((temp_data >> 8) & 0xff);
	temp[2]=(u8)((temp_data >> 16) & 0xff);	
	temp[3]=(u8)((temp_data >> 24) & 0xff);		      
    I2C_WriteNByte(0xA0,1,0X40,temp,8);
	 
	temp_data=Pen_Point.yoff0;
	temp[0]=(u8)(temp_data & 0xff);
	temp[1]=(u8)((temp_data >> 8) & 0xff);
	temp[2]=(u8)((temp_data >> 16) & 0xff);	
	temp[3]=(u8)((temp_data >> 24) & 0xff);		      
    I2C_WriteNByte(0xA0,1,0X48,temp,8);

	temp_data=Pen_Point.yoff1;
	temp[0]=(u8)(temp_data & 0xff);
	temp[1]=(u8)((temp_data >> 8) & 0xff);
	temp[2]=(u8)((temp_data >> 16) & 0xff);	
	temp[3]=(u8)((temp_data >> 24) & 0xff);		      
    I2C_WriteNByte(0xA0,1,0X56,temp,8);

	temp[0]=0xff;
    I2C_WriteNByte(0xA0,1,0X64,temp,1);
}


//agar screen adjust shode bashe tu in address ha mokhtassatesh neveshte shode
u8 Get_Adjdata(void)
{
	s32 temp_data;
	u8  temp[8],i;

	//flag(neshun mide screen adjust shode ya na)
	temp_data = I2C_ReadNByte(0xA0,1,0X64,temp,8); 
   	if(temp[0] == 0xff)
   	{
	   for(i=0;i<8;i++)
	   {
	   	 temp[i] = 0;
	   }
	   I2C_ReadNByte(0xA0,1,0X00,temp,8);
	   temp_data = (s32)((temp[3]<<24)|(temp[2]<<16)|(temp[1]<<8)|temp[0]);
	   Pen_Point.xfac = (float)temp_data /100000000;
	
	   I2C_ReadNByte(0xA0,1,0X08,temp,8);
	   temp_data = (s32)((temp[3]<<24)|(temp[2]<<16)|(temp[1]<<8)|temp[0]);
	   Pen_Point.yfac = (float)temp_data /100000000;
	
	   I2C_ReadNByte(0xA0,1,0X16,temp,8);
	   temp_data = (s32)((temp[1]<<8)|temp[0]);
	   Pen_Point.xoff = temp_data;
	
	   I2C_ReadNByte(0xA0,1,0X24,temp,8);
	   temp_data = (s32)((temp[1]<<8)|temp[0]);
	   Pen_Point.yoff = temp_data;

	   I2C_ReadNByte(0xA0,1,0X32,temp,8);
	   temp_data = (s32)((temp[3]<<24)|(temp[2]<<16)|(temp[1]<<8)|temp[0]);
	   Pen_Point.xoff0 = temp_data;

	   I2C_ReadNByte(0xA0,1,0X40,temp,8);
	   temp_data = (s32)((temp[3]<<24)|(temp[2]<<16)|(temp[1]<<8)|temp[0]);
	   Pen_Point.xoff1 = temp_data;

	   I2C_ReadNByte(0xA0,1,0X48,temp,8);
	   temp_data = (s32)((temp[3]<<24)|(temp[2]<<16)|(temp[1]<<8)|temp[0]);
	   Pen_Point.yoff0 = temp_data;

	   I2C_ReadNByte(0xA0,1,0X56,temp,8);
	   temp_data = (s32)((temp[3]<<24)|(temp[2]<<16)|(temp[1]<<8)|temp[0]);
	   Pen_Point.yoff1 = temp_data;
	   return 1;
   	}
   	return 0;
}


/*
*********************************************************************************************************
* Description: 	Write data.
* Arguments  : 	num : the data value
* Returns    : 	None
*********************************************************************************************************
*/	   
void ADS_Write_Byte(u8 num)    
{  
	u8 count=0;
	
	for(count=0;count<8;count++)  
	{ 	  
		if(num&0x80)
		{
			TDIN(1);
		}  
		else 
		{
			TDIN(0);
		}   
		num<<=1;    
		TCLK(0);
		delay_us(1);	   	 
		TCLK(1);	
	} 			    
} 
/*
*********************************************************************************************************
* Description: 	Reading the value of ADC from 7846/7843/XPT2046/UH7843/UH7846.
* Arguments  : 	CMD  the Command value
* Returns    : 	None
*********************************************************************************************************
*/		   
u16 ADS_Read_AD(u8 CMD)	  
{ 	 
	u8 count=0; 	  
	u16 Num=0;
 
	TCLK(0);	 
	TCS(0); 
	TCLK(0);
	delay_us(10);		 
	ADS_Write_Byte(CMD);
	delay_us(13);		   	    
	TCLK(0);
	delay_us(1);		 
	for(count=0;count<16;count++)  
	{ 				  
		Num<<=1; 	 
		TCLK(0);
		delay_us(1);  	    	   
		TCLK(1);
		if(DOUT)
		Num++; 		 
	}
		 	
	Num>>=4;
	TCS(1);	 
	return(Num);   
}
/*
*********************************************************************************************************
* Description: 	Take an order for the value value.
* Arguments  : 	xy  the Command value
* Returns    : 	None
*********************************************************************************************************
*/	 
#define READ_TIMES 15 
#define LOST_VAL 5	 
u16 ADS_Read_XY(u8 xy)
{
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum=0;
	u16 temp;
	for(i=0;i<READ_TIMES;i++)
	{				 
		buf[i]=ADS_Read_AD(xy);	    
	}				    
	for(i=0;i<READ_TIMES-1; i++)
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 
/*
*********************************************************************************************************
* Description: 	Getting the real value of AD function.
* Arguments  : 	*x ,*y   the cale of AD value
* Returns    : 	None
*********************************************************************************************************
*/	
u8 Read_ADS(u16 *x,u16 *y)
{
	u16 xtemp,ytemp;			 	 		  
	xtemp=ADS_Read_XY(CMD_RDX);
	ytemp=ADS_Read_XY(CMD_RDY);	  												   
	if(xtemp<100||ytemp<100)return 0;							//fail
	*x=xtemp;
	*y=ytemp;
	return 1;													//success
}

/*
*********************************************************************************************************
* Description: 	Getting the real value of AD function.
* Arguments  : 	x ,y  the coordinate value
* Returns    : 	None
*********************************************************************************************************
*/	
#define ERR_RANGE 50 				
u8 Read_ADS2(u16 *x,u16 *y) 
{
	u16 x1,y1;
 	u16 x2,y2;
 	u8 flag;    
    flag=Read_ADS(&x1,&y1);   
    if(flag==0)return(0);
    flag=Read_ADS(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else return 0;	  
} 
/*
*********************************************************************************************************
* Description: 	Getting the touch value of coordinate function.
* Arguments  :  None
* Returns    : 	None
*********************************************************************************************************
*/				   
u8 Read_TP_Once(void)
{
	u8 t=0;	    
	Pen_Int_Set(0);
	Pen_Point.Key_Sta=Key_Up;
	Read_ADS2(&Pen_Point.X,&Pen_Point.Y);
	while(PEN==0&&t<=250)
	{
		t++;
		delay_ms(10);
	}
	Pen_Int_Set(1);
			 
	if(t>=250)
		return 0;
	else 
		return 1;	
}

/*
*********************************************************************************************************
* Description: 	Draw a touch point function.
* Arguments  : 	x ,y  the coordinate value
* Returns    : 	None
*********************************************************************************************************
*/
void Drow_Touch_Point(u16 x,u16 y)
{
	TextColor=Red;
	GLCD_DrawLine(x-15,y,x+15,y);
	GLCD_DrawLine(x,y-15,x,y+15);
	GLCD_DrawPoint(x+1,y+1);
	GLCD_DrawPoint(x-1,y+1);
	GLCD_DrawPoint(x+1,y-1);
	GLCD_DrawPoint(x-1,y-1);
	GLCD_DrawCircle(x,y,13);
}
/*
*********************************************************************************************************
* Description: 	Draw a big point function.
* Arguments  : 	x ,y  the coordinate value
* Returns    : 	None
*********************************************************************************************************
*/			   
void Draw_Big_Point(u16 x,u16 y)
{	
	//x = 320-x;    
	GLCD_DrawPoint(x-1,y-1);
	GLCD_DrawPoint(x-1,y);
	GLCD_DrawPoint(x-1,y+1);
	GLCD_DrawPoint(x,y-1);
	GLCD_DrawPoint(x,y);
	GLCD_DrawPoint(x,y+1);
	GLCD_DrawPoint(x+1,y-1);
	GLCD_DrawPoint(x+1,y);
	GLCD_DrawPoint(x+1,y+1);	 	  	
}
/*
*********************************************************************************************************
* Description: 	Getting the value of coordinate function.
* Arguments  : 	None
* Returns    : 	None
*********************************************************************************************************
*/
void Convert_Pos(void)
{
	float xfac,yfac;
	int xdiff1,xdiff0,ydiff1,ydiff0;

	if(Read_ADS2(&Pen_Point.X,&Pen_Point.Y))
	{
		xdiff1 = Pen_Point.xoff1-Pen_Point.xoff0;
		ydiff1 = Pen_Point.yoff0-Pen_Point.yoff1;
	
		xdiff0 = Pen_Point.X-Pen_Point.xoff0;
		ydiff0 = Pen_Point.Y-Pen_Point.yoff1;
				  
		xfac = (float) xdiff1/xdiff0;
		Pen_Point.Y0 = (u16) ((int)((float) 160/xfac) + Pen_Point.xoff);
		
		yfac = (float) ydiff1/ydiff0;
		Pen_Point.X0 = (u16) ((int)((float) 240/yfac) + Pen_Point.yoff);	
	}
}
/*
*********************************************************************************************************
* Description: 	Touch screen External interrupt initialize function.
* Arguments  : 	None
* Returns    : 	None
*********************************************************************************************************
*/
uint32_t EINTInit( void )
{

  LPC_PINCON->PINSEL4 = 1 << 26;	// set P2.13 as EINT0 and
									// P2.0~7 GPIO output

  LPC_SC->EXTMODE = EINT3_EDGE;		// INT3 edge trigger
  LPC_SC->EXTPOLAR = 0;				// INT3 is falling edge by default

  NVIC_EnableIRQ(EINT3_IRQn);
  return( 1 );
}
/*
*********************************************************************************************************
* Description: 	Touch screen External interrupt function.
* Arguments  : 	None
* Returns    : 	None
*********************************************************************************************************
*/
void EINT3_IRQHandler (void) 
{
	delay_us(1);
	if(!PEN)
	{
 	Pen_Point.Key_Sta=Key_Down;
	}	

    LPC_SC->EXTINT = EINT3;				// clear interrupt
}
/*
*********************************************************************************************************
* Description: 	Touch screen External interrput Enable function.
* Arguments  : 	1 Enable
*				0 Disable
* Returns    : 	None
*********************************************************************************************************
*/	 
void Pen_Int_Set(u8 en)
{
	if(en)
	NVIC_EnableIRQ(EINT3_IRQn);   //Enable		  	
	else 
	NVIC_DisableIRQ(EINT3_IRQn); //Disable 	   
}
/*
*********************************************************************************************************
* Description: 	Touch screen adjust function.Getting four adjust parameter!
* Arguments  : 	None
* Returns    : 	None
*********************************************************************************************************
*/
void Touch_Adjust(void)
{
	u16 pos_temp[4][2];													//Coordinate cache value
	u16 x_avg[2];														//X Coordinate average value
	u16 y_avg[2];														//Y Coordinate average value
	u8  cnt=0;	
	u16 d1,d2;
	u32 tem1,tem2;
	float fac; 	   
	cnt=0;				
	TextColor=Red;
	BackColor =White;
	GLCD_Clear(White);	   
	Drow_Touch_Point(40,40);						   					//Draw the first point	 
	Pen_Point.Key_Sta=Key_Up;						  					//Clear the signal 
	Pen_Point.xfac=0;								   					//Record whether adjust 
	while(1)
	{
		if(Pen_Point.Key_Sta==Key_Down)				   					//Is touched
		{
			//safhe movazi sathe ofogh nabashe mokhtassat ro part neshun mide!
			if(Read_TP_Once())						   					//got the coordinate value
			{  								   
				pos_temp[cnt][0]=Pen_Point.X;
				pos_temp[cnt][1]=Pen_Point.Y;
				cnt++;
			}			 
			switch(cnt)
			{			   
				case 1:
					GLCD_Clear(White); 
					Drow_Touch_Point(40,200);							//Draw the second point
					break;
				
				case 2:
					GLCD_Clear(White);
					Drow_Touch_Point(280,200);							//Draw the third point
					break;
				
				case 3:
					GLCD_Clear(White); 
					Drow_Touch_Point(280,40);							//Draw the fouth point
					break;
				
				case 4:	 												//Have gotten all the value
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);			//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);			//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);
					
					tem1=abs(pos_temp[3][0]-pos_temp[2][0]);			//x4-x3
					tem2=abs(pos_temp[3][1]-pos_temp[2][1]);			//y4-y3
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);

					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05||d1==0||d2==0)				//Unqualified
					{
						cnt=0;
						GLCD_Clear(White);
						TextColor=Red;
						GLCD_DisplayString( 4, 3, "Y Unqualified!");
						delay_ms(500);
						GLCD_Clear(White);
						Drow_Touch_Point(40,40);						//Draw the first point
						continue;
					}

					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);			//x1-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);			//y1-y4
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);
					
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);			//x2-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);			//y2-y3
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);
					
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05||d1==0||d2==0)				//Unqualified
					{
						cnt=0;
						GLCD_Clear(White);
						TextColor=Red;
						GLCD_DisplayString( 4, 3, "X Unqualified!");
						delay_ms(500);
						GLCD_Clear(White);
						Drow_Touch_Point(40,40);						//Draw the first point
						continue;
					}
								  
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);			//x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);			//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);
	
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);			//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);			//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);
					
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)								//Unqualified
					{
						cnt=0;
						GLCD_Clear(White);
						TextColor=Red;
						GLCD_DisplayString( 4, 3, "D Unqualified!");
						delay_ms(500);
						GLCD_Clear(White);
						Drow_Touch_Point(40,40);						//Draw the first point
						continue;
					}

					//calculate average coordinates
					x_avg[0]=(pos_temp[0][0]+pos_temp[3][0])/2;
					x_avg[1]=(pos_temp[1][0]+pos_temp[2][0])/2;
					y_avg[0]=(pos_temp[2][1]+pos_temp[3][1])/2;
					y_avg[1]=(pos_temp[0][1]+pos_temp[1][1])/2;
					
					Pen_Point.xfac=(float)160/(x_avg[1]-x_avg[0]);	 
					Pen_Point.xoff=(240-Pen_Point.xfac*(x_avg[1]-x_avg[0]))/2;
					Pen_Point.xoff0=x_avg[0];	 
					Pen_Point.xoff1=x_avg[1];	 
						  
					Pen_Point.yfac=(float)240/(y_avg[1]-y_avg[0]);
					Pen_Point.yoff=(320-Pen_Point.yfac*(y_avg[1]-y_avg[0]))/2;
					Pen_Point.yoff0=y_avg[0];
					Pen_Point.yoff1=y_avg[1];	 
					
					Save_Adjdata();															//write in i2c
					Get_Adjdata();								 							//read from i2c
					
					TextColor=Blue;
					GLCD_Clear(White);
					GLCD_DisplayString( 4, 4, "Adjust Is OK!");								//Adjust is OK
					delay_ms(500);
					GLCD_Clear(White);

					return;
			}
		}
	} 
}		    

/*
*********************************************************************************************************
* Description: 	Touch screen initialize function.
* Arguments  : 	None
* Returns    : 	None
*********************************************************************************************************
*/
void Touch_Init(void)
{			    		   
	LPC_GPIO0->FIODIR |= 0x00002C0;  				//P0.6 0.7 0.9 Output
	LPC_GPIO2->FIODIR |= 0x0000000;  				//P2.13 input P2.0...P2.7 Output
 	Read_ADS(&Pen_Point.X,&Pen_Point.Y);			//the first Read initialize			 
	EINTInit();	 									//Setting External interrupt
	GLCD_Clear(White);
  	if ( I2CInit( (uint32_t)I2CMASTER ) == 0 )		/* initialize I2c */
	{
		while ( 1 );								/* Fatal error */
	}

	if(!Get_Adjdata())
		Touch_Adjust(); 
}


/*
*******************************************************************************
* My Own Functions:                                                            *
*******************************************************************************
*/


int Touch_PassKeyboard (uint8_t *pass)
{
	int key;

 	if(Pen_Point.Key_Sta==Key_Down)										//Touch screen is pressed 
	{
		Pen_Int_Set(0);													//Closed interrupt
		do
		{
			Convert_Pos();
			Pen_Point.Key_Sta=Key_Up;
			if(Pen_Point.X0>touch_x1_l && Pen_Point.X0<touch_x1_h)
			{
				if(Pen_Point.Y0<touch_y1_h && Pen_Point.Y0>touch_y1_l)
					key=1;
				else
					if(Pen_Point.Y0<touch_y2_h && Pen_Point.Y0>touch_y2_l)
						key=4;
					else
						if(Pen_Point.Y0<touch_y3_h && Pen_Point.Y0>touch_y3_l)
							key=7;
						else
							if(Pen_Point.Y0<touch_y4_h && Pen_Point.Y0>touch_y4_l)
								key=10;
							else
								key=-1;
			}
			else
				if(Pen_Point.X0>touch_x2_l && Pen_Point.X0<touch_x2_h)
				{
					if(Pen_Point.Y0<touch_y1_h && Pen_Point.Y0>touch_y1_l)
						key=2;
					else
						if(Pen_Point.Y0<touch_y2_h && Pen_Point.Y0>touch_y2_l)
							key=5;
						else
							if(Pen_Point.Y0<touch_y3_h && Pen_Point.Y0>touch_y3_l)
								key=8;
							else
								if(Pen_Point.Y0<touch_y4_h && Pen_Point.Y0>touch_y4_l)
									key=0;
								else
									key=-1;
				}
				else
					if(Pen_Point.X0>touch_x3_l && Pen_Point.X0<touch_x3_h)
					{
						if(Pen_Point.Y0<touch_y1_h && Pen_Point.Y0>touch_y1_l)
							key=3;
						else
							if(Pen_Point.Y0<touch_y2_h && Pen_Point.Y0>touch_y2_l)
								key=6;
							else
								if(Pen_Point.Y0<touch_y3_h && Pen_Point.Y0>touch_y3_l)
									key=9;
								else
									if(Pen_Point.Y0<touch_y4_h && Pen_Point.Y0>touch_y4_l)
										key=11;
									else
										key=-1;
					}
			else
				key=-1;   
		}
		while(PEN==0);                                                 	//While the Touch screen is pressing
		delay1ms(10);													//delay befor opening interrupt ( for debaunce )
		Pen_Int_Set(1);													//Opene the interrupt
		
		//invalid key
		if(key == -1)
			return 0;

		//if Enter pressed
		if(key == 11)
			return 1;
		
		//if Clear pressed
		if(key == 10) {							 
			pass_len--;
			GLCD_DisplayChar( 1, pass_len+13, ' ');
			return 0;
		}

		GLCD_DisplayChar( 1, pass_len+13, key+'0');
		pass[pass_len] = key+'0';
		if(pass_len<6) {
			pass_len++;
			return 0;
		}
		else
			return -1;
	}
	else
		delay1ms(1);
	
	return 0;
}


int Touch_ReadPass (uint8_t *pass)
{
	int keyboard;
	pass_len = 0;
	GLCD_ShowPassKeyboard();
	
	do {
		keyboard = Touch_PassKeyboard(pass);
	}
	while(keyboard == 0);

	if(keyboard == 1)
		return pass_len;
	else
		return 0;
}

int Touch_ReadGeneralKeyboard (void)
{
 	int key;

 	if(Pen_Point.Key_Sta==Key_Down)										//Touch screen is pressed 
	{
		Pen_Int_Set(0);													//Closed interrupt
		do
		{
			Convert_Pos();
			Pen_Point.Key_Sta=Key_Up;
			if(Pen_Point.X0>touch_general_x1_l && Pen_Point.X0<touch_general_x1_h)
				if(Pen_Point.Y0<touch_general_y1_h && Pen_Point.Y0>touch_general_y1_l)
					key=1;
				else
					if(Pen_Point.Y0<touch_general_y2_h && Pen_Point.Y0>touch_general_y2_l)
						key=2;
					else
						if(Pen_Point.Y0<touch_general_y3_h && Pen_Point.Y0>touch_general_y3_l)
							key=3;
						else
							key=0;
		}
		while(PEN==0);                                                 	//While the Touch screen is pressing
		delay1ms(10);													//delay befor opening interrupt ( for debaunce )
		Pen_Int_Set(1);													//Opene the interrupt
		
		if(key == 1)
			GLCD_ShowModesMenu();
		if(key == 2)
			GLCD_ShowMusicMenu();
		
		if(key == 3) {
			//MyFunction();
			//GLCD_Clear(White);
			//GLCD_DisplayString( 4, 1, "Test Function! Called");
			//delay_ms(100);
			//GLCD_Clear(White);
		}

		delay1ms(10);
		return key;
	}
	else
		delay1ms(1);
	
	return 0;	
}

int Touch_ReadModesKeyboard (void)
{
 	int key;
	
 	if(Pen_Point.Key_Sta==Key_Down)										//Touch screen is pressed 
	{
		Pen_Int_Set(0);													//Closed interrupt
		do
		{
			Convert_Pos();
			Pen_Point.Key_Sta=Key_Up;
			if(Pen_Point.X0>touch_modes_x1_l && Pen_Point.X0<touch_modes_x1_h)
			{
				if(Pen_Point.Y0<touch_modes_y1_h && Pen_Point.Y0>touch_modes_y1_l)
					key=1;
				else
					if(Pen_Point.Y0<touch_modes_y2_h && Pen_Point.Y0>touch_modes_y2_l)
						key=2;
					else
						if(Pen_Point.Y0<touch_modes_y3_h && Pen_Point.Y0>touch_modes_y3_l)
							key=3;
						else
							key=0;
			}  
		}
		while(PEN==0);                                                 	//While the Touch screen is pressing
		delay1ms(10);													//delay befor opening interrupt ( for debaunce )
		Pen_Int_Set(1);													//Opene the interrupt
		
		delay1ms(10);
		return key;
	}
	else
		delay1ms(1);
	
	return 0;	
}

int Touch_ReadMusicKeyboard (void)
{
 	int key;

 	if(Pen_Point.Key_Sta==Key_Down)										//Touch screen is pressed 
	{
		Pen_Int_Set(0);													//Closed interrupt
		do
		{
			Convert_Pos();
			Pen_Point.Key_Sta=Key_Up;
			if(Pen_Point.X0>touch_music_x1_l && Pen_Point.X0<touch_music_x1_h)
			{
				if(Pen_Point.Y0<touch_music_y1_h && Pen_Point.Y0>touch_music_y1_l)
					key=1;
				else
					if(Pen_Point.Y0<touch_music_y2_h && Pen_Point.Y0>touch_music_y2_l)
						key=2;
					else
						if(Pen_Point.Y0<touch_music_y3_h && Pen_Point.Y0>touch_music_y3_l)
							key=4;
						else
							if(Pen_Point.Y0<touch_music_y4_h && Pen_Point.Y0>touch_music_y4_l)
								key=6;
							else
								key=-1;
			}
			else
				if(Pen_Point.X0>touch_music_x2_l && Pen_Point.X0<touch_music_x2_h)
				{
					if(Pen_Point.Y0<touch_music_y1_h && Pen_Point.Y0>touch_music_y1_l)
						key=8;
					else
						if(Pen_Point.Y0<touch_music_y2_h && Pen_Point.Y0>touch_music_y2_l)
							key=3;
						else
							if(Pen_Point.Y0<touch_music_y3_h && Pen_Point.Y0>touch_music_y3_l)
								key=5;
							else
								if(Pen_Point.Y0<touch_music_y4_h && Pen_Point.Y0>touch_music_y4_l)
									key=7;
								else
									key=-1;
				}
				else
					key=-1;   
		}
		while(PEN==0);                                                 	//While the Touch screen is pressing
		delay1ms(10);													//delay befor opening interrupt ( for debaunce )
		Pen_Int_Set(1);													//Opene the interrupt

		delay1ms(10);
		return key;
	}
	else
		delay1ms(1);
	
	return 0;	
}
