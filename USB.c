#include  "usb.h"

uint8_t  WAV[25000];
int		 index;
wav 	 temp;

uint8_t  wave_read_flag;
int 	 wave_counter;
uint8_t	 wave_play_flag;
uint8_t	 wave_volume;
uint8_t	 wave_track;
uint8_t	 wave_track_counter;

int USB_Init (void)
{
    int32_t  rc;
	uint32_t numBlks, blkSize;
	uint8_t  inquiryResult[INQUIRY_LENGTH];
	
    rc = Host_EnumDev();       /* Enumerate the device connected                                            */	
    if (rc == OK) {
		/* Initialize the mass storage and scsi interfaces */
        rc = MS_Init( &blkSize, &numBlks, inquiryResult );
        if (rc == OK) {
            rc = FAT_Init();   /* Initialize the FAT16 file system                                          */
            if (rc == OK) {
				return 1;
			} else {
                return 0;
            }
        } else {
            return 0;
        }
    } else {							
        return 0;
    }
}

int USB_CheckPass (uint32_t len, uint8_t *buffer)
{
	int 	 counter;
    int32_t  fdr;
    uint32_t  bytes_read;
	
	if(len <= 0)
		return 0;

    fdr = FILE_Open(PASSFILENAME_R, RDONLY);

    if (fdr > 0) 
	{
		bytes_read = FILE_Read(fdr, UserBuffer, MAX_BUFFER_SIZE);

		if(bytes_read!=len)
		{
			FILE_Close(fdr);
			return 0;
		}
		
		for(counter=0;counter<bytes_read;counter++)
			if(buffer[counter]!=UserBuffer[counter])
			{
				FILE_Close(fdr);
				return 0;
			}
		
        FILE_Close(fdr);
		return 1; 
    } 
	else 
	{
        return 0;
    }
}

void TIMER0_IRQHandler (void)
{
	if (TIM_GetIntStatus(LPC_TIM0, TIM_MR0_INT) == SET)
	{
		if(wave_play_flag == 0) {
			temp.bits.high = 0;
		    temp.bits.low = 0;
			DAC_UpdateValue (LPC_DAC, temp.Val*wave_volume);		
		}
		else {
			temp.bits.high = 0;
		    temp.bits.low = WAV[index];
			DAC_UpdateValue (LPC_DAC, temp.Val*wave_volume);
			
			index++;
			if( index >= (wave_counter+1)*MAX_BUFFER_SIZE )
			{
				if(wave_read_flag == 0) {
					wave_read_flag = 1;
					wave_counter++;
					if(index >= 6*MAX_BUFFER_SIZE) {
						index = 0;
						wave_counter = 0;
					}
				}
				else {
					index--;
				}	
			}
		}
	}
	TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
}

void DAC_Configuration (void)
{
	PINSEL_CFG_Type PinCfg;

	/*
	 * Init DAC pin connect
	 * AOUT on P0.26
	 */
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 26;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);

	/* init DAC structure to default
	 * Maximum	current is 700 uA
	 * First value to AOUT is 0
	 */
	DAC_Init(LPC_DAC);
}

void TIM_Configuration (void)
{
	TIM_TIMERCFG_Type TIM_ConfigStruct;
	TIM_MATCHCFG_Type TIM_MatchConfigStruct;

	/* Initialize timer 0, prescale count time of 100uS */
	TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
	TIM_ConfigStruct.PrescaleValue	= 10;

	/* use channel 0, MR0 */
	TIM_MatchConfigStruct.MatchChannel = 0;
	/* Enable interrupt when MR0 matches the value in TC register */
	TIM_MatchConfigStruct.IntOnMatch   = TRUE;
	/* Enable reset on MR0: TIMER will reset if MR0 matches it */
	TIM_MatchConfigStruct.ResetOnMatch = TRUE;
	/* Stop on MR0 if MR0 matches it */
	TIM_MatchConfigStruct.StopOnMatch  = FALSE;
	/* Toggle MR0.0 pin if MR0 matches it */
	TIM_MatchConfigStruct.ExtMatchOutputType =TIM_EXTMATCH_TOGGLE;
	/* Set Match value, count value of 4 (4 * 10uS = 40us = 1s --> 25 KHz) */
	TIM_MatchConfigStruct.MatchValue   = 4 ;

	/* Set configuration for Tim_config and Tim_MatchConfig */
	TIM_Init(LPC_TIM0, TIM_TIMER_MODE,&TIM_ConfigStruct);
	TIM_ConfigMatch(LPC_TIM0,&TIM_MatchConfigStruct);

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(TIMER0_IRQn, ((0x01<<3)|0x01));
	/* Enable interrupt for timer 0 */
	NVIC_EnableIRQ(TIMER0_IRQn);
	/* To start timer 0 */
	TIM_Cmd(LPC_TIM0,ENABLE);
}

void USB_ShowMusicFiles (void)
{
	//send wave accept
	UART0_SendByte(0xc2);

	//send music files to terminal
   	wave_track_counter = USB_LineUpWaves();

	//send wave end to terminal
	UART0_SendByte(0xc4);
}

int32_t USB_InitWave (uint8_t track, uint8_t volume)
{
	int32_t  temp_fdr;
	uint32_t bytes_read;	
	uint32_t byte_counter;

	temp_fdr = USB_FileOpen(track);
	if (temp_fdr > 0) {
		index = 0;
		for(wave_counter=0;wave_counter<6;wave_counter++) {
			bytes_read = FILE_Read(temp_fdr, UserBuffer, MAX_BUFFER_SIZE);
			for(byte_counter=0;byte_counter<bytes_read;byte_counter++) {
				WAV[wave_counter*MAX_BUFFER_SIZE + byte_counter] = UserBuffer[byte_counter];
			}
		}
		
		wave_read_flag = 0;
		wave_counter = 0;
		wave_play_flag = 0;
		wave_track = track;
		wave_volume = volume;

        return temp_fdr;
	}
	else
		return -1; 
}

void USB_UpdateWave (int32_t fdr)
{
	uint32_t bytes_read;   	
	uint32_t byte_counter;

	if(wave_read_flag == 1) {
		bytes_read = FILE_Read(fdr, UserBuffer, MAX_BUFFER_SIZE);
		for(byte_counter=0;byte_counter<bytes_read;byte_counter++) {
			WAV[((wave_counter+5)%6)*MAX_BUFFER_SIZE + byte_counter] = UserBuffer[byte_counter];
		}
	
		if(bytes_read == 0) {
			FILE_Close(fdr);
			USB_FileOpen(wave_track);
	
			for(byte_counter=0;byte_counter<MAX_BUFFER_SIZE;byte_counter++) {
				WAV[((wave_counter+4)%6)*MAX_BUFFER_SIZE + byte_counter] = 0x00;
			}
	
			bytes_read = FILE_Read(fdr, UserBuffer, MAX_BUFFER_SIZE);
			for(byte_counter=0;byte_counter<bytes_read;byte_counter++) {
				WAV[((wave_counter+5)%6)*MAX_BUFFER_SIZE + byte_counter] = UserBuffer[byte_counter];
			}
		}
		wave_read_flag = 0;
	}
}

void USB_SetVolume (uint8_t music)
{
	if(music == 6) {
		if(wave_volume > 0) {
			wave_volume--;
		}
	}

	if(music == 7) {
	 	if(wave_volume < 4) {
			wave_volume++;
		}
	}
}

int32_t USB_SetPlayFlag (uint8_t music, int32_t fdr)
{
	int32_t temp_fdr;
	temp_fdr = fdr;

	if(music == 1)
		wave_play_flag = 1;

	if(music == 2)
		wave_play_flag = 0;
		
	if(music == 3) {
		FILE_Close(fdr);
		temp_fdr = USB_InitWave(wave_track,wave_volume);
	}
	return temp_fdr;	
}

int32_t USB_ChangeTrack(uint8_t music, int32_t fdr)
{		 
	int32_t temp_fdr;

	if(music == 4) {
		if(wave_track > 0) {
			wave_track--;
		}
	}

	if(music == 5) {
	 	if(wave_track < wave_track_counter-1) {
			wave_track++;
		}
	}

	FILE_Close(fdr);
	temp_fdr = USB_InitWave(wave_track,wave_volume);
	wave_play_flag = 1;
	return temp_fdr;	
}

int32_t USB_SetTrack(int32_t track, int32_t fdr)
{		 
	int32_t temp_fdr;

 	if((track >= 0) && (track < wave_track_counter)) {
		wave_track = track;
		FILE_Close(fdr);
		temp_fdr = USB_InitWave(wave_track,wave_volume);
		wave_play_flag = 1;	 
		return temp_fdr;
	}
	else
		return fdr;	
}

int32_t USB_MusicOperation(uint8_t music, int32_t fdr)
{
	int32_t temp_fdr;
	temp_fdr = fdr;

	switch(music) {
	 	case 1:
		case 2:
		case 3:
			temp_fdr = USB_SetPlayFlag(music,fdr);
			break;

		case 4:
		case 5:
			temp_fdr = USB_ChangeTrack(music,fdr);
			break;
		
		case 6:
		case 7:
			USB_SetVolume(music);
			break;
		
		default:
			break;	
	}
	return temp_fdr;
}
