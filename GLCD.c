#include "glcd.h"
#include "sys.h"
#include "Font_24x16.h"

/*---------------------------- Global variables ------------------------------*/
unsigned short TextColor = Black, BackColor = White;
static unsigned short driverCode;

/************************ Local auxiliary functions ***************************/

/*******************************************************************************
* Delay in while loop cycles                                                   *
*   Parameter:    cnt:    number of while cycles to delay                      *
*   Return:                                                                    *
*******************************************************************************/

static void delay (int cnt) {

  cnt <<= DELAY_2N;
  while (cnt--);
}

__asm void wait()
{
    nop
    BX lr
}

void wait_delay(int count)
{
  while(count--);
}

/*******************************************************************************
* Send 1 byte over serial communication                                        *
*   Parameter:    byte:   byte to be sent                                      *
*   Return:                                                                    *
*******************************************************************************/

static __inline unsigned char lcd_send (unsigned short byte) {

  LPC_GPIO2->FIODIR |= 0x000000ff;  //P2.0...P2.7 Output
  LCD_DIR(1)		   				//Interface A->B
  LCD_EN(0)	                        //Enable 2A->2B
  LPC_GPIO2->FIOPIN =  byte;        //Write D0..D7
  LCD_LE(1)                         
  LCD_LE(0)							//latch D0..D7
  LPC_GPIO2->FIOPIN =  byte >> 8;   //Write D8..D15 
  return(1);
}


/*******************************************************************************
* read 1 byte over serial communication                                        *
*   Parameter:    byte:   byte to be sent                                      *
*   Return:                                                                    *
*******************************************************************************/

static __inline unsigned short lcd_read (void) {
  unsigned short id;
  LPC_GPIO2->FIODIR &= 0xffffff00;                //P2.0...P2.7 Input
  LCD_DIR(0)		   				              //Interface B->A
  LCD_EN(0)	                                      //Enable 2B->2A
  wait_delay(80);							      //delay some times
  id = LPC_GPIO2->FIOPIN & 0x00ff;                //Read D8..D15                         
  LCD_EN(1)	                                      //Enable 1B->1A
  wait_delay(80);							      //delay some times
  id = (id << 8) | (LPC_GPIO2->FIOPIN & 0x00ff);  //Read D0..D7                         
  LCD_DIR(1)						
  return(id); 
}

/*******************************************************************************
* Write command to LCD controller                                              *
*   Parameter:    c:      command to be written                                *
*   Return:                                                                    *
*******************************************************************************/

static __inline void wr_cmd (unsigned char c) {

  LCD_RS(0)
  LCD_RD(1)
  lcd_send(c);
  LCD_WR(0)
  wait();
  LCD_WR(1)
}


/*******************************************************************************
* Write data to LCD controller                                                 *
*   Parameter:    c:      data to be written                                   *
*   Return:                                                                    *
*******************************************************************************/

static __inline void wr_dat (unsigned short c) {

  LCD_RS(1)
  LCD_RD(1)
  lcd_send(c);
  LCD_WR(0)
  wait();
  LCD_WR(1)
}

/*******************************************************************************
* Read data from LCD controller                                                *
*   Parameter:                                                                 *
*   Return:               read data                                            *
*******************************************************************************/

static __inline unsigned short rd_dat (void) {
  unsigned short val = 0;

  LCD_RS(1)
  LCD_WR(1)
  LCD_RD(0)
  val = lcd_read();
  LCD_RD(1)
  return val;
}

/*******************************************************************************
* Start of data writing to LCD controller                                      *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

static __inline void wr_dat_start (void) {

  LCD_RS(1)
}


/*******************************************************************************
* Stop of data writing to LCD controller                                       *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

static __inline void wr_dat_stop (void) {

  LCD_CS(1)
}


/*******************************************************************************
* Data writing to LCD controller                                               *
*   Parameter:    c:      data to be written                                   *
*   Return:                                                                    *
*******************************************************************************/

static __inline void wr_dat_only (unsigned short c) {

  lcd_send(c);
  LCD_WR(0)
  wait();
  LCD_WR(1)
}


/*******************************************************************************
* Write to LCD register                                                        *
*   Parameter:    reg:    register to be read                                  *
*                 val:    value to write to register                           *
*******************************************************************************/

static __inline void wr_reg (unsigned char reg, unsigned short val) {

  LCD_CS(0)
  wr_cmd(reg);
  wr_dat(val);
  LCD_CS(1)
}


/*******************************************************************************
* Read from LCD register                                                       *
*   Parameter:    reg:    register to be read                                  *
*   Return:               value read from register                             *
*******************************************************************************/

static unsigned short rd_reg (unsigned short reg) {
  unsigned short val = 0;

  LCD_CS(0)
  wr_cmd(reg);
  val = rd_dat(); 
  LCD_CS(1)
  return (val);
}


/************************ Exported functions **********************************/

/*******************************************************************************
* Initialize the Graphic LCD controller                                        *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_Init (void) { 

  /* Configure the LCD Control pins                                           */
  LPC_GPIO0->FIODIR   |= 0x03f80000;
  LPC_GPIO0->FIOSET    = 0x03f80000;	

  delay(5);                             /* Delay 50 ms                        */

  driverCode = rd_reg(0x00);

  if(driverCode == 0x4531)				//2.8" TFT LCD Module,DriverIC is LGDP4531
  {
  	wr_reg(0x00,0x0001);
  	wr_reg(0x10,0x0628);
  	wr_reg(0x12,0x0006);
  	wr_reg(0x13,0x0A32);
  	wr_reg(0x11,0x0040);
  	wr_reg(0x15,0x0050);
  	wr_reg(0x12,0x0016);
  	delay(15);
  	wr_reg(0x10,0x5660);
  	delay(15);
  	wr_reg(0x13,0x2A4E);
  	wr_reg(0x01,0x0100);
  	wr_reg(0x02,0x0300);
	
  	wr_reg(0x03,0x1030);
	
  	wr_reg(0x08,0x0202);
  	wr_reg(0x0A,0x0000);
  	wr_reg(0x30,0x0000);
  	wr_reg(0x31,0x0402);
  	wr_reg(0x32,0x0106);
  	wr_reg(0x33,0x0700);
  	wr_reg(0x34,0x0104);
  	wr_reg(0x35,0x0301);
  	wr_reg(0x36,0x0707);
  	wr_reg(0x37,0x0305);
  	wr_reg(0x38,0x0208);
  	wr_reg(0x39,0x0F0B);
  	delay(15);
  	wr_reg(0x41,0x0002);
  	wr_reg(0x60,0x2700);
  	wr_reg(0x61,0x0001);
  	wr_reg(0x90,0x0119);
  	wr_reg(0x92,0x010A);
  	wr_reg(0x93,0x0004);
  	wr_reg(0xA0,0x0100);
  	delay(15);
  	wr_reg(0xA0,0x0000);
  	delay(20);
  }
  else if(driverCode == 0x9325)		 //2.8" TFT LCD Module,DriverIC is ILI9325
  {
  	    wr_reg(0x00e7,0x0010);      
        wr_reg(0x0000,0x0001);  			//start internal osc
        wr_reg(0x0001,0x0100);     
        wr_reg(0x0002,0x0700); 				//power on sequence                     
        wr_reg(0x0003,(1<<12)|(1<<5)|(1<<4) ); 	//65K 
        wr_reg(0x0004,0x0000);                                   
        wr_reg(0x0008,0x0207);	           
        wr_reg(0x0009,0x0000);         
        wr_reg(0x000a,0x0000); 				//display setting         
        wr_reg(0x000c,0x0001);				//display setting          
        wr_reg(0x000d,0x0000); 				//0f3c          
        wr_reg(0x000f,0x0000);
//Power On sequence //
        wr_reg(0x0010,0x0000);   
        wr_reg(0x0011,0x0007);
        wr_reg(0x0012,0x0000);                                                                 
        wr_reg(0x0013,0x0000);                 
        delay(15);
        wr_reg(0x0010,0x1590);   
        wr_reg(0x0011,0x0227);
        delay(15);
        wr_reg(0x0012,0x009c);                  
        delay(15);
        wr_reg(0x0013,0x1900);   
        wr_reg(0x0029,0x0023);
        wr_reg(0x002b,0x000e);
        delay(15);
        wr_reg(0x0020,0x0000);                                                            
        wr_reg(0x0021,0x0000);           
///////////////////////////////////////////////////////      
        delay(15);
        wr_reg(0x0030,0x0007); 
        wr_reg(0x0031,0x0707);   
        wr_reg(0x0032,0x0006);
        wr_reg(0x0035,0x0704);
        wr_reg(0x0036,0x1f04); 
        wr_reg(0x0037,0x0004);
        wr_reg(0x0038,0x0000);        
        wr_reg(0x0039,0x0706);     
        wr_reg(0x003c,0x0701);
        wr_reg(0x003d,0x000f);
        delay(15);
        wr_reg(0x0050,0x0000);        
        wr_reg(0x0051,0x00ef);   
        wr_reg(0x0052,0x0000);     
        wr_reg(0x0053,0x013f);
        wr_reg(0x0060,0xa700);        
        wr_reg(0x0061,0x0001); 
        wr_reg(0x006a,0x0000);
        wr_reg(0x0080,0x0000);
        wr_reg(0x0081,0x0000);
        wr_reg(0x0082,0x0000);
        wr_reg(0x0083,0x0000);
        wr_reg(0x0084,0x0000);
        wr_reg(0x0085,0x0000);
      
        wr_reg(0x0090,0x0010);     
        wr_reg(0x0092,0x0000);  
        wr_reg(0x0093,0x0003);
        wr_reg(0x0095,0x0110);
        wr_reg(0x0097,0x0000);        
        wr_reg(0x0098,0x0000);  
         //display on sequence     
        wr_reg(0x0007,0x0133);
    
        wr_reg(0x0020,0x0000);                                                            
        wr_reg(0x0021,0x0000);
   }
  else if(driverCode == 0x9320)		 //3.2" TFT LCD Module,DriverIC is ILI9320
  {
    /* Start Initial Sequence --------------------------------------------------*/
	wr_reg(0xE5, 0x8000);                 /* Set the internal vcore voltage     */
	wr_reg(0x00, 0x0001);                 /* Start internal OSC                 */
	wr_reg(0x01, 0x0100);                 /* Set SS and SM bit                  */
	wr_reg(0x02, 0x0700);                 /* Set 1 line inversion               */
	wr_reg(0x03, 0x1030);                 /* Set GRAM write direction and BGR=1 */
	wr_reg(0x04, 0x0000);                 /* Resize register                    */
	wr_reg(0x08, 0x0202);                 /* 2 lines each, back and front porch */
	wr_reg(0x09, 0x0000);                 /* Set non-disp area refresh cyc ISC  */
	wr_reg(0x0A, 0x0000);                 /* FMARK function                     */
	wr_reg(0x0C, 0x0000);                 /* RGB interface setting              */
	wr_reg(0x0D, 0x0000);                 /* Frame marker Position              */
	wr_reg(0x0F, 0x0000);                 /* RGB interface polarity             */
	
	/* Power On sequence -------------------------------------------------------*/
	wr_reg(0x10, 0x0000);                 /* Reset Power Control 1              */
	wr_reg(0x11, 0x0000);                 /* Reset Power Control 2              */
	wr_reg(0x12, 0x0000);                 /* Reset Power Control 3              */
	wr_reg(0x13, 0x0000);                 /* Reset Power Control 4              */
	delay(20);                            /* Discharge cap power voltage (200ms)*/
    wr_reg(0x10, 0x17B0);                 /* SAP, BT[3:0], AP, DSTB, SLP, STB   */
	wr_reg(0x11, 0x0137);                 /* DC1[2:0], DC0[2:0], VC[2:0]        */
	delay(5);                             /* Delay 50 ms                        */
	wr_reg(0x12, 0x0139);                 /* VREG1OUT voltage                   */
	delay(5);                             /* Delay 50 ms                        */
	wr_reg(0x13, 0x1D00);                 /* VDV[4:0] for VCOM amplitude        */
	wr_reg(0x29, 0x0013);                 /* VCM[4:0] for VCOMH                 */
	delay(5);                             /* Delay 50 ms                        */
	wr_reg(0x20, 0x0000);                 /* GRAM horizontal Address            */
	wr_reg(0x21, 0x0000);                 /* GRAM Vertical Address              */
	
	/* Adjust the Gamma Curve --------------------------------------------------*/
	wr_reg(0x30, 0x0006);
	wr_reg(0x31, 0x0101);
	wr_reg(0x32, 0x0003);
	wr_reg(0x35, 0x0106);
	wr_reg(0x36, 0x0B02);
	wr_reg(0x37, 0x0302);
	wr_reg(0x38, 0x0707);
	wr_reg(0x39, 0x0007);
	wr_reg(0x3C, 0x0600);
	wr_reg(0x3D, 0x020B);
	  
	/* Set GRAM area -----------------------------------------------------------*/
	wr_reg(0x50, 0x0000);                 /* Horizontal GRAM Start Address      */
	wr_reg(0x51, (HEIGHT-1));             /* Horizontal GRAM End   Address      */
	wr_reg(0x52, 0x0000);                 /* Vertical   GRAM Start Address      */
	wr_reg(0x53, (WIDTH-1));              /* Vertical   GRAM End   Address      */
	wr_reg(0x60, 0x2700);                 /* Gate Scan Line                     */
	wr_reg(0x61, 0x0001);                 /* NDL,VLE, REV                       */
	wr_reg(0x6A, 0x0000);                 /* Set scrolling line                 */
	
	/* Partial Display Control -------------------------------------------------*/
	wr_reg(0x80, 0x0000);
	wr_reg(0x81, 0x0000);
	wr_reg(0x82, 0x0000);
	wr_reg(0x83, 0x0000);
	wr_reg(0x84, 0x0000);
	wr_reg(0x85, 0x0000);
	
	/* Panel Control -----------------------------------------------------------*/
	wr_reg(0x90, 0x0010);
	wr_reg(0x92, 0x0000);
	wr_reg(0x93, 0x0003);
	wr_reg(0x95, 0x0110);
	wr_reg(0x97, 0x0000);
	wr_reg(0x98, 0x0000);
  }
  
  /* Set GRAM write direction and BGR = 1
     I/D=10 (Horizontal : increment, Vertical : increment)
     AM=1 (address is updated in vertical writing direction)                  */
  wr_reg(0x03, 0x1038);

  wr_reg(0x07, 0x0173);                 /* 262K color and display ON          */ 
  if(driverCode == 0x8989)		 //3.2" TFT LCD Module,DriverIC is SSD1289
  {
  	    wr_reg(0x0000,0x0001);    delay(5);  //打开晶振
    	wr_reg(0x0003,0xA8A4);    delay(5);   //0xA8A4
    	wr_reg(0x000C,0x0000);    delay(5);   
    	wr_reg(0x000D,0x080C);    delay(5);   
    	wr_reg(0x000E,0x2B00);    delay(5);   
    	wr_reg(0x001E,0x00B0);    delay(5);   
    	wr_reg(0x0001,0x2b3F);    delay(5);        //驱动输出控制320*240  0x6B3F  293f	2b3f 6b3f
    	wr_reg(0x0002,0x0600);    delay(5);
    	wr_reg(0x0010,0x0000);    delay(5);
    	wr_reg(0x0011,0x6078);    delay(5);        //0x4030           //定义数据格式  16位色  横屏 0x6058	   6078
    	wr_reg(0x0005,0x0000);    delay(5);
    	wr_reg(0x0006,0x0000);    delay(5);
    	wr_reg(0x0016,0xEF1C);    delay(5);
    	wr_reg(0x0017,0x0003);    delay(5);
    	wr_reg(0x0007,0x0233);    delay(5);        //0x0233       
    	wr_reg(0x000B,0x0000);    delay(5);
    	wr_reg(0x000F,0x0000);    delay(5);        //扫描开始地址
    	wr_reg(0x0041,0x0000);    delay(5);
    	wr_reg(0x0042,0x0000);    delay(5);
    	wr_reg(0x0048,0x0000);    delay(5);
    	wr_reg(0x0049,0x013F);    delay(5);
    	wr_reg(0x004A,0x0000);    delay(5);
    	wr_reg(0x004B,0x0000);    delay(5);
    	wr_reg(0x0044,0xEF00);    delay(5);
    	wr_reg(0x0045,0x0000);    delay(5);
    	wr_reg(0x0046,0x013F);    delay(5);
    	wr_reg(0x0030,0x0707);    delay(5);
    	wr_reg(0x0031,0x0204);    delay(5);
    	wr_reg(0x0032,0x0204);    delay(5);
    	wr_reg(0x0033,0x0502);    delay(5);
    	wr_reg(0x0034,0x0507);    delay(5);
    	wr_reg(0x0035,0x0204);    delay(5);
    	wr_reg(0x0036,0x0204);    delay(5);
    	wr_reg(0x0037,0x0502);    delay(5);
    	wr_reg(0x003A,0x0302);    delay(5);
    	wr_reg(0x003B,0x0302);    delay(5);
    	wr_reg(0x0023,0x0000);    delay(5);
    	wr_reg(0x0024,0x0000);    delay(5);
    	wr_reg(0x0025,0x8000);    delay(5);
    	wr_reg(0x004f,0);        //行首址0
    	wr_reg(0x004e,0);        //列首址0
  }					

  GLCD_Clear  (White);
}


/*******************************************************************************
* Set draw window region to whole screen                                       *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_WindowMax (void) {
  
  if(driverCode==0x8989)
  {
	wr_reg(0x44, 0);                      /* Horizontal GRAM Start Address      */
    wr_reg(0x44, 0 |((HEIGHT-1)<<8));     /* Horizontal GRAM End   Address (-1) */
    wr_reg(0x45, 0);                      /* Vertical   GRAM Start Address      */
    wr_reg(0x46, WIDTH-1);                /* Vertical   GRAM End   Address (-1) */
  }
  else
  {
  	wr_reg(0x50, 0);                      /* Horizontal GRAM Start Address      */
  	wr_reg(0x51, HEIGHT-1);               /* Horizontal GRAM End   Address (-1) */
  	wr_reg(0x52, 0);                      /* Vertical   GRAM Start Address      */
  	wr_reg(0x53, WIDTH-1);                /* Vertical   GRAM End   Address (-1) */
  }
}


/*******************************************************************************
* Draw a pixel in foreground color                                             *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_PutPixel (unsigned int x, unsigned int y) {
  if(driverCode==0x8989)
  {
  	wr_reg(0x4e, y);
  	wr_reg(0x4f, WIDTH-1-x);
  }
  else
  {
  	wr_reg(0x20, y);
  	wr_reg(0x21, WIDTH-1-x);
  }
  LCD_CS(0)
  wr_cmd(0x22);
  wr_dat(TextColor);
  LCD_CS(1)
}


/*******************************************************************************
* Set foreground color                                                         *
*   Parameter:      color:    foreground color                                 *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_SetTextColor (unsigned short color) {

  TextColor = color;
}


/*******************************************************************************
* Set background color                                                         *
*   Parameter:      color:    background color                                 *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_SetBackColor (unsigned short color) {

  BackColor = color;
}


/*******************************************************************************
* Clear display                                                                *
*   Parameter:      color:    display clearing color                           *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_Clear (unsigned short color) {
  unsigned int   i;

  GLCD_WindowMax();

  if(driverCode==0x8989)
  {
  	wr_reg(0x4e, 0);
  	wr_reg(0x4f, 0);
  }
  else
  {
  	wr_reg(0x20, 0);
  	wr_reg(0x21, 0);
  }
  LCD_CS(0)
  wr_cmd(0x22);
  wr_dat_start();
  for(i = 0; i < (WIDTH*HEIGHT); i++)
    wr_dat_only(Yellow);
  wr_dat_stop();

  GLCD_SetBackColor(DarkGreen);
  GLCD_SetTextColor(Green);
}


/*******************************************************************************
* Draw character on given position                                             *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   c:        pointer to character bitmap                      *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_DrawChar (unsigned int x, unsigned int y, unsigned short *c) {
  int idx = 0, i, j;

  x = WIDTH-x-CHAR_W;
   
  if(driverCode==0x8989)
  {
  	wr_reg(0x44, y);                      /* Horizontal GRAM Start Address      */
    wr_reg(0x44, y |((y+CHAR_H-1)<<8));   /* Horizontal GRAM End   Address (-1) */
    wr_reg(0x45, x);                      /* Vertical   GRAM Start Address      */
    wr_reg(0x46, x+CHAR_W-1);             /* Vertical   GRAM End   Address (-1) */

	wr_reg(0x4e, y);
  	wr_reg(0x4f, x);
  }
  else
  {
  	wr_reg(0x50, y);                      /* Horizontal GRAM Start Address      */
  	wr_reg(0x51, y+CHAR_H-1);             /* Horizontal GRAM End   Address (-1) */
  	wr_reg(0x52, x);                      /* Vertical   GRAM Start Address      */
  	wr_reg(0x53, x+CHAR_W-1);             /* Vertical   GRAM End   Address (-1) */

	wr_reg(0x20, y);
  	wr_reg(0x21, x);
  }
  LCD_CS(0)
  wr_cmd(0x22);
  wr_dat_start();
  for (j = 0; j < CHAR_H; j++) {
    for (i = CHAR_W-1; i >= 0; i--) {
      if((c[idx] & (1 << i)) == 0x00) {
        wr_dat_only(BackColor);
      } else {
        wr_dat_only(TextColor);
      }
    }
    c++;
  }
  wr_dat_stop();
}


/*******************************************************************************
* Disply character on given line                                               *
*   Parameter:      ln:       line number                                      *
*                   col:      column number                                    *
*                   c:        ascii character                                  *
*   Return:                                                                    *
*******************************************************************************/
// y = ln mod 10, x = col mod 20 
void GLCD_DisplayChar (unsigned int ln, unsigned int col, unsigned char c) {

  c -= 32;
  GLCD_DrawChar(col * CHAR_W, ln * CHAR_H, (unsigned short *)&Font_24x16[c * CHAR_H]);
}


/*******************************************************************************
* Disply string on given line                                                  *
*   Parameter:      ln:       line number                                      *
*                   col:      column number                                    *
*                   s:        pointer to string                                *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_DisplayString (unsigned int ln, unsigned int col, unsigned char *s) {

  //find string border
  unsigned char *temp_string;
  int temp_counter = 0;
  temp_string = s;
  while (*temp_string) {
  	temp_string++;
	temp_counter++;
  }
  GLCD_Bargraph( col*CHAR_W-4, ln*CHAR_H-8, temp_counter*CHAR_W+7, CHAR_H+15, 0);
  GLCD_SetTextColor(Black);
  GLCD_DrawRectangle( col*CHAR_W-4, ln*CHAR_H-8, col*CHAR_W-4 + temp_counter*CHAR_W+7, ln*CHAR_H-8 + CHAR_H+15);
  GLCD_DrawRectangle( col*CHAR_W-5, ln*CHAR_H-9, col*CHAR_W-3 + temp_counter*CHAR_W+7, ln*CHAR_H-7 + CHAR_H+15);
  GLCD_SetTextColor(Green);
  
  GLCD_WindowMax();
  while (*s) {
    GLCD_DisplayChar(ln, col++, *s++);
  }


}


/*******************************************************************************
* Clear given line                                                             *
*   Parameter:      ln:       line number                                      *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_ClearLn (unsigned int ln) {

  GLCD_WindowMax();
  GLCD_DisplayString(ln, 0, "                    ");
}

/*******************************************************************************
* Draw bargraph                                                                *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   w:        maximum width of bargraph (in pixels)            *
*                   val:      value of active bargraph (in 1/1024)             *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_Bargraph (unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int val) {
  int i,j;

  x = WIDTH-x-w;
  if(driverCode==0x8989)
  {
  	wr_reg(0x44, y);                      /* Horizontal GRAM Start Address      */
    wr_reg(0x44, y |((y+h-1)<<8));   /* Horizontal GRAM End   Address (-1) */
    wr_reg(0x45, x);                      /* Vertical   GRAM Start Address      */
    wr_reg(0x46, x+w-1);                  /* Vertical   GRAM End   Address (-1) */
  }
  else
  {
  	wr_reg(0x50, y);                      /* Horizontal GRAM Start Address      */
  	wr_reg(0x51, y+h-1);             /* Horizontal GRAM End   Address (-1) */
  	wr_reg(0x52, x);                      /* Vertical   GRAM Start Address      */
  	wr_reg(0x53, x+w-1);                  /* Vertical   GRAM End   Address (-1) */
  }

  val = (val * w) >> 10;                /* Scale value for 24x12 characters   */
  if(driverCode==0x8989)
  {
  	wr_reg(0x4e, y);
  	wr_reg(0x4f, x);
  }
  else
  {
  	wr_reg(0x20, y);
  	wr_reg(0x21, x);
  }
  LCD_CS(0)
  wr_cmd(0x22);
  wr_dat_start();
  for (i = 0; i < h; i++) {
    for (j = w-1; j >= 0; j--) {
      if(j >= val) {
        wr_dat_only(BackColor);
      } else {
        wr_dat_only(TextColor);
      }
    }
  }
  wr_dat_stop();
}


/*******************************************************************************
* Display graphical bitmap image at position x horizontally and y vertically   *
* (This function is optimized for 16 bits per pixel format, it has to be       *
*  adapted for any other bits per pixel format)                                *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   w:        width of bitmap                                  *
*                   h:        height of bitmap                                 *
*                   bitmap:   address at which the bitmap data resides         *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_Bitmap (unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char *bitmap) {
  unsigned int    i, j;
  unsigned short *bitmap_ptr = (unsigned short *)bitmap;

  x = WIDTH-x-w;
  if(driverCode==0x8989)
  {
  	wr_reg(0x44, y);                      /* Horizontal GRAM Start Address      */
    wr_reg(0x44, y |((y+h-1)<<8));        /* Horizontal GRAM End   Address (-1) */
    wr_reg(0x45, x);                      /* Vertical   GRAM Start Address      */
    wr_reg(0x46, x+w-1);                  /* Vertical   GRAM End   Address (-1) */
  	wr_reg(0x4e, y);
  	wr_reg(0x4f, x);
  }
  else
  {
  	wr_reg(0x50, y);                      /* Horizontal GRAM Start Address      */
    wr_reg(0x51, y+h-1);                  /* Horizontal GRAM End   Address (-1) */
    wr_reg(0x52, x);                      /* Vertical   GRAM Start Address      */
    wr_reg(0x53, x+w-1);                  /* Vertical   GRAM End   Address (-1) */
  	wr_reg(0x20, y);
  	wr_reg(0x21, x);
  }
  LCD_CS(0)
  wr_cmd(0x22);
  wr_dat_start();
  for (j = 0; j < h; j++) {
    bitmap_ptr += w-1;
    for (i = 0; i < w; i++) {
      wr_dat_only(*bitmap_ptr--);
    }
    bitmap_ptr += w+1;
  }
  wr_dat_stop();
}


/*******************************************************************************
* Display graphical bmp file image at position x horizontally and y vertically *
* (This function is optimized for 16 bits per pixel format, it has to be       *
*  adapted for any other bits per pixel format)                                *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   w:        width of bitmap                                  *
*                   h:        height of bitmap                                 *
*                   bmp:      address at which the bmp data resides            *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_Bmp (unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char *bmp) {
  unsigned int    i, j;
  unsigned short *bitmap_ptr = (unsigned short *)bmp;

  x = WIDTH-x-w;

  if(driverCode==0x8989)
  {
  	wr_reg(0x44, y);                      /* Horizontal GRAM Start Address      */
    wr_reg(0x44, y |((y+h-1)<<8));        /* Horizontal GRAM End   Address (-1) */
    wr_reg(0x45, x);                      /* Vertical   GRAM Start Address      */
    wr_reg(0x46, x+w-1);                  /* Vertical   GRAM End   Address (-1) */
  	wr_reg(0x4e, y);
  	wr_reg(0x4f, x);
  }
  else
  {
  	wr_reg(0x50, y);                      /* Horizontal GRAM Start Address      */
    wr_reg(0x51, y+h-1);                  /* Horizontal GRAM End   Address (-1) */
    wr_reg(0x52, x);                      /* Vertical   GRAM Start Address      */
    wr_reg(0x53, x+w-1);                  /* Vertical   GRAM End   Address (-1) */
  	wr_reg(0x20, y);
  	wr_reg(0x21, x);
  }
  LCD_CS(0)
  wr_cmd(0x22);
  wr_dat_start();
  bitmap_ptr += (h*w)-1;
  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++) {
      wr_dat_only(*bitmap_ptr--);
    }
  }
  wr_dat_stop();
}


/*
*******************************************************************************
* My Own Functions:                                                            *
*******************************************************************************
*/

// vorudi ha bayad x:(0,319) va y:(0:239) bashe, az inja bishtar bashe kollan shekl ro rasm nemikone!
void GLCD_DrawPoint (unsigned int x, unsigned int y) 
{
  x = WIDTH-x;
  
  GLCD_WindowMax();

  //set cursor
  if(driverCode==0x8989)
  {
  	wr_reg(0x4e, y);
  	wr_reg(0x4f, x);
  }
  else
  {
  	wr_reg(0x20, y);
  	wr_reg(0x21, x);
  }
  LCD_CS(0)
  wr_cmd(0x22);
  wr_dat_start();
  wr_dat_only(TextColor);
  wr_dat_stop();
}


void GLCD_DrawLine(unsigned int x1, unsigned int y1,unsigned int x2, unsigned int y2)
{
	unsigned int t;
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1;
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1;
	else if(delta_x==0)incx=0;
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )
	{  
		GLCD_DrawPoint(uRow,uCol);
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}


void GLCD_DrawRectangle(unsigned int x1, unsigned int y1,unsigned int x2, unsigned int y2)
{
	GLCD_DrawLine(x1,y1,x2,y1);
	GLCD_DrawLine(x1,y1,x1,y2);
	GLCD_DrawLine(x1,y2,x2,y2);
	GLCD_DrawLine(x2,y1,x2,y2);
}


void GLCD_DrawCircle(unsigned int x0, unsigned int y0,unsigned int r)
{
	int a,b,di;
	a=0;
	b=r;	  
	di=3-(r<<1);
	while(a<=b)
	{
		GLCD_DrawPoint(x0-b,y0-a);                 
		GLCD_DrawPoint(x0+b,y0-a);                    
		GLCD_DrawPoint(x0-a,y0+b);                
		GLCD_DrawPoint(x0-b,y0-a);                    
		GLCD_DrawPoint(x0-a,y0-b);                    
		GLCD_DrawPoint(x0+b,y0+a);                      
		GLCD_DrawPoint(x0+a,y0-b);          
		GLCD_DrawPoint(x0+a,y0+b);          
		GLCD_DrawPoint(x0-b,y0+a);             
		a++;
		   
		if(di<0)di += 4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 
		GLCD_DrawPoint(x0+a,y0+b);
	}
}

void GLCD_ShowPassKeyboard (void)
{
	GLCD_Clear(White);

	GLCD_DisplayString( 1, 13, "      ");

	GLCD_DisplayString( 1,  1, " 1 ");
	GLCD_DisplayString( 1,  5, " 2 ");
	GLCD_DisplayString( 1,  9, " 3 ");	
	GLCD_DisplayString( 3,  1, " 4 ");
	GLCD_DisplayString( 3,  5, " 5 ");
	GLCD_DisplayString( 3,  9, " 6 ");
	GLCD_DisplayString( 5,  1, " 7 ");
	GLCD_DisplayString( 5,  5, " 8 ");
	GLCD_DisplayString( 5,  9, " 9 ");
	GLCD_DisplayString( 7,  1, " C ");
	GLCD_DisplayString( 7,  5, " 0 ");
	GLCD_DisplayString( 7,  9, " E ");
	
}

void GLCD_ShowPassState (unsigned int enter)
{
	if(enter == 1) {
		GLCD_Clear(White);
		GLCD_DisplayString( 4, 6, "Welcome!");
		delay_ms(300);
		GLCD_Clear(White);
	}
	else {
		GLCD_Clear(White);
		GLCD_DisplayString( 4, 7, "Wrong!");
		delay_ms(300);
		GLCD_Clear(White);
	}		
}

void GLCD_ShowGeneralMenu (uint8_t mode, uint16_t color)
{
	GLCD_Clear(White);
   	
	GLCD_DisplayString( 1,  1, "Modes Menu ");
	GLCD_DisplayString( 3,  1, "Music Menu ");

	GLCD_DisplayString( 1, 13, "Mode: ");
	GLCD_DisplayChar( 1, 18, mode+'0');
	GLCD_DisplayString( 3, 13, "Vol.: ");
	GLCD_DisplayChar( 3, 18, wave_volume+'0');
	
	if(color != 0) {
		GLCD_SetBackColor(color);
		GLCD_DisplayString( 5, 13, "      ");
		GLCD_SetBackColor(DarkGreen); 	
	}

	//for test function
	//GLCD_DisplayString( 5,  1, "Call Test Function");
}

void GLCD_ShowModesMenu (void)
{
	GLCD_Clear(White);

	GLCD_DisplayString( 1,  1, "Mode 1");
	GLCD_DisplayString( 3,  1, "Mode 2");
	GLCD_DisplayString( 5,  1, "Mode 3");
}		   

void GLCD_ShowMusicMenu (void)
{
	GLCD_Clear(White);

	GLCD_DisplayString( 1,  1, "Play  ");
	GLCD_DisplayString( 1, 10, "Back  ");
	GLCD_DisplayString( 3,  1, "Pause ");
	GLCD_DisplayString( 3, 10, "Stop  ");
	GLCD_DisplayString( 5,  1, "Track-");
	GLCD_DisplayString( 5, 10, "Track+");
	GLCD_DisplayString( 7,  1, "Vol-  ");
	GLCD_DisplayString( 7, 10, "Vol+  ");
}
