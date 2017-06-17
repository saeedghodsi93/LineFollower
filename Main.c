#include "string.h"
#include "glcd.h"	
#include "touch.h" 
#include "usb.h"
#include "uart.h"
#include "sys.h"

#define	start	0XA0
#define	node	0XA8
#define	black	0XB0
#define	red		0XB1
#define	green	0XB2
#define	yellow	0XB3
#define	blue	0XB4
#define	white	0XB7
#define	stop	0XB8

//music file descriptor
int32_t fdr;

//password variables
uint8_t pass[20];
int32_t PassLen;
uint8_t enter;

//touch screen and music variables
uint8_t key = 0;
uint8_t mode = 0;
uint8_t music = 0;

//data from terminal
uint8_t terminal = 0;

//data from header
uint8_t header = 0;
uint16_t color = 0;

void Test()
{
	mode = 2;
	
	UART0_SendByte(start);
	delay1ms(100);			  
	UART0_SendByte(node);
	delay1ms(100);			  
	UART0_SendByte(node);
	delay1ms(100);			  
	UART0_SendByte(red);
	delay1ms(100);			  
	UART0_SendByte(node);
	delay1ms(100);			  
	UART0_SendByte(node);
	delay1ms(100);			  
	UART0_SendByte(yellow);
	delay1ms(100);			  
	UART0_SendByte(node);
	delay1ms(100);			  
	UART0_SendByte(node);
	delay1ms(100);			  
	UART0_SendByte(white);
	delay1ms(100);			  
	UART0_SendByte(node);
	delay1ms(100);			  
	UART0_SendByte(blue);
	delay1ms(100);			  
	UART0_SendByte(node);
	delay1ms(100);			  
	UART0_SendByte(node);
	delay1ms(100);			  
	UART0_SendByte(node);
	delay1ms(100);			  
	UART0_SendByte(node);
	delay1ms(100);			  
	UART0_SendByte(node);
	delay1ms(100);			  
	UART0_SendByte(green);
	delay1ms(100);			  
	UART0_SendByte(node);
	delay1ms(100);			  
	UART0_SendByte(yellow);
	delay1ms(100);			  
	UART0_SendByte(node);
	delay1ms(100);			  
	UART0_SendByte(node);
	delay1ms(100);			  
	UART0_SendByte(stop);
	delay1ms(100);	
}

void delay1ms(unsigned int delay1msdata)
{
	unsigned int i,j,k;
	for(i=0;i<delay1msdata;i++)
		for(j=0;j<1000;j++)
			for(k=0;k<100;k++);
}

//mainboard touch screen and music operation
void MainBoard_Operation()
{
	USB_UpdateWave(fdr);
	
	switch(key) {
		case 0:
			key = Touch_ReadGeneralKeyboard();
			break;
	
		case 1:
			mode = Touch_ReadModesKeyboard();
			if(mode != 0) {				
				UART2_SendByte(mode);
				GLCD_ShowGeneralMenu(mode,color);

				Test();

				key = 0;
			}
			break;
	
		case 2:
			music = Touch_ReadMusicKeyboard();	 
			fdr = USB_MusicOperation(music,fdr);
			if(music == 8) {
				GLCD_ShowGeneralMenu(mode,color);
				key = 0;
			}
			break;
	   	
		case 3:
			//for using any test function do this :
			//modify "GLCD_ShowGeneralMenu" function
			//modify "Touch_ReadGeneralKeyboard" --> "key 3" case
			//uncomment bellow line
			//GLCD_ShowGeneralMenu(mode,color);
			key = 0;
			break;
	
		default:
			break;
	}	
}

//terminal keyboard and mouse operation
void Terminal_Operation()
{
	terminal = UART0_CheckInput();
 	
	//check request
	if(terminal == 0xc1) {
		USB_ShowMusicFiles();
		return;
	}

	if((terminal > 0) && (terminal < '0')) {
		fdr = USB_SetTrack(terminal-1,fdr);
		return;	
	}
   	
	if(terminal > 0) {
	 	fdr = USB_MusicOperation(terminal-'0',fdr);
		if(key == 0)
			GLCD_ShowGeneralMenu(mode,color);
	}
}

//header board graph operation
void Header_Operation()
{
	header = UART2_CheckInput();

	if(header > 0) {
		if(mode == 3) {
			UART0_SendByte(header); 	
		}
		switch(header) {
		 	case 0xa0:													
				color = Black;
				break;
			
			case 0xa8:													
				color = Purple;
				break;

			case 0xb0:													
				color = Black;
				break;

			case 0xb1:
				color = Red;
				break;

			case 0xb2:
				color = Green;
				break;

			case 0xb3:
				color = Yellow;
				break;

			case 0xb4:
				color = Blue;
				break;

			case 0xb7:
				color = White;
				break;

			default:
				color = 0;
				break;
		}
		GLCD_ShowGeneralMenu(mode,color);
	}
}

int main (void) 
{
	//init functions
  	SystemInit();
  	GLCD_Init (); 
  	Touch_Init();
    Host_Init();
	UART0_Init();
	UART2_Init();
	while(!USB_Init());

	//wait for password
	do {
		PassLen = Touch_ReadPass(pass);
		enter = USB_CheckPass(PassLen,pass);
		GLCD_ShowPassState(enter);
	}
	while(!enter);

	//verify password
	UART0_SendByte(0x60);
            
	//init wave
	fdr = USB_InitWave(1,1);
	if(fdr >= 0) {
		DAC_Configuration();
		TIM_Configuration();
	}

	 //mode =2;

	//main loop
	GLCD_ShowGeneralMenu(mode,color);
	while(1)
	{
		MainBoard_Operation();
		Terminal_Operation();
		Header_Operation();
	}
}
