#ifndef _GLCD_H
#define _GLCD_H
#include "GLCD_DEF.h"

extern unsigned short TextColor, BackColor;
extern unsigned char wave_volume;

extern void GLCD_Init           (void);
extern void GLCD_WindowMax      (void);
extern void GLCD_PutPixel       (unsigned int x, unsigned int y);
extern void GLCD_SetTextColor   (unsigned short color);
extern void GLCD_SetBackColor   (unsigned short color);
extern void GLCD_Clear          (unsigned short color);
extern void GLCD_DrawChar       (unsigned int x, unsigned int y, unsigned short *c);
extern void GLCD_DisplayChar    (unsigned int ln, unsigned int col, unsigned char  c);
extern void GLCD_DisplayString  (unsigned int ln, unsigned int col, unsigned char *s);
extern void GLCD_ClearLn        (unsigned int ln);
extern void GLCD_Bargraph       (unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int val);
extern void GLCD_Bitmap         (unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char *bitmap);
extern void GLCD_Bmp            (unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char *bmp);

extern void GLCD_DrawPoint 		(unsigned int x, unsigned int y); 
extern void GLCD_DrawLine		(unsigned int x1, unsigned int y1,unsigned int x2, unsigned int y2);
extern void GLCD_DrawRectangle	(unsigned int x1, unsigned int y1,unsigned int x2, unsigned int y2);
extern void GLCD_ShowPassKeyboard	(void);
extern void GLCD_ShowPassState	(unsigned int enter);

extern void GLCD_ShowGeneralMenu (unsigned char mode, unsigned short int color);
extern void GLCD_ShowModesMenu 	(void);
extern void GLCD_ShowMusicMenu 	(void);

#endif
