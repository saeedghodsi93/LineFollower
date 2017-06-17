#ifndef __SYS_H
#define __SYS_H	
#include "LPC17xx.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Common:

typedef unsigned          char u8;
typedef unsigned short     int u16;
typedef unsigned           int u32;
typedef unsigned       __int64 u64;

volatile  uint8_t  *pass;

#define   GPIOPIN_0	   (1 << 0)
#define   GPIOPIN_1	   (1 << 1)
#define   GPIOPIN_2	   (1 << 2)
#define   GPIOPIN_3	   (1 << 3)
#define   GPIOPIN_4	   (1 << 4)
#define   GPIOPIN_5	   (1 << 5)
#define   GPIOPIN_6	   (1 << 6)
#define   GPIOPIN_7	   (1 << 7)
#define   GPIOPIN_8	   (1 << 8)
#define   GPIOPIN_9	   (1 << 9)
#define   GPIOPIN_10   (1 << 10)
#define   GPIOPIN_11   (1 << 11)
#define   GPIOPIN_12   (1 << 12)
#define   GPIOPIN_13   (1 << 13)
#define   GPIOPIN_14   (1 << 14)
#define   GPIOPIN_15   (1 << 15)
#define   GPIO0_6(x)   ((x) ? (LPC_GPIO0->FIOSET = GPIOPIN_6) : (LPC_GPIO0->FIOCLR = GPIOPIN_6))
#define   GPIO0_7(x)   ((x) ? (LPC_GPIO0->FIOSET = GPIOPIN_7) : (LPC_GPIO0->FIOCLR = GPIOPIN_7))
#define   GPIO0_9(x)   ((x) ? (LPC_GPIO0->FIOSET = GPIOPIN_9) : (LPC_GPIO0->FIOCLR = GPIOPIN_9))
#define   GPIO2_13     (*((volatile unsigned long *) (0x22000000+ (0x2009C055-0x20000000)*32 + 5*4)))
#define   GPIO0_8      (*((volatile unsigned long *) (0x22000000+ (0X2009C015-0x20000000)*32 + 0*4)))
#define   GPIO2_10	   (*((volatile unsigned long *) (0x22000000+ (0x2009C054-0x20000000)*32 + 10*4)))

#define Black           0x0000		/*   0,   0,   0 */
#define Navy            0x000F      /*   0,   0, 128 */
#define DarkGreen       0x03E0      /*   0, 128,   0 */
#define DarkCyan        0x03EF      /*   0, 128, 128 */
#define Maroon          0x7800      /* 128,   0,   0 */
#define Purple          0x780F      /* 128,   0, 128 */
#define Olive           0x7BE0      /* 128, 128,   0 */
#define LightGrey       0xC618      /* 192, 192, 192 */
#define DarkGrey        0x7BEF      /* 128, 128, 128 */
#define Blue            0x001F      /*   0,   0, 255 */
#define Green           0x07E0      /*   0, 255,   0 */
#define Cyan            0x07FF      /*   0, 255, 255 */
#define Red             0xF800      /* 255,   0,   0 */
#define Magenta         0xF81F      /* 255,   0, 255 */
#define Yellow          0xFFE0      /* 255, 255, 0   */
#define White           0xFFFF      /* 255, 255, 255 */

#endif
