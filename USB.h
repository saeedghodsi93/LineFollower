#ifndef  USBHOST_MAIN_H
#define  USBHOST_MAIN_H
#include "usbhost_inc.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Common:

#define PASSFILENAME_R  "PASS.txt"
#define FILENAME_W  	"MSWRITE.txt"

#define MAX_BUFFER_SIZE             (4000)
#define WRITE_SIZE          (10 * 1000000)

extern int 		USB_Init(void);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Pass:

extern int 		USB_CheckPass(uint32_t len, uint8_t *buffer);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Wave:

typedef union 
{
	uint16_t Val;
	struct
	{ 
	uint16_t low	 :8;				
	uint16_t high	 :8;			
	}bits;	    
}wav;

extern uint8_t  WAV[25000];
extern int		index;
extern wav 	 	temp;

extern uint8_t  wave_read_flag;
extern int 	 	wave_counter;
extern uint8_t	wave_play_flag;
extern uint8_t	wave_volume;
extern uint8_t	wave_track;

extern void 	TIMER0_IRQHandler(void);
extern void 	DAC_Configuration(void);
extern void 	TIM_Configuration(void);

extern void		USB_ShowMusicFiles(void);

extern int32_t 	USB_InitWave(uint8_t track, uint8_t volume);
extern void 	USB_UpdateWave(int32_t fdr);

extern void		USB_SetVolume(uint8_t music);
extern int32_t	USB_SetPlayFlag(uint8_t music, int32_t fdr);
extern int32_t	USB_ChangeTrack(uint8_t music, int32_t fdr);
extern int32_t	USB_SetTrack(int32_t track, int32_t fdr);

extern int32_t 	USB_MusicOperation(uint8_t music, int32_t fdr);

#endif
