#include  "usbhost_main.h"

/* Private typedef -----------------------------------------------------------*/
typedef union 
{
	uint16_t Val;
	struct
	{ 
	uint16_t low	 :8;				
	uint16_t high	 :8;			
	}bits;	    
}wav;

/* Private variables ---------------------------------------------------------*/
uint8_t WAV[25000];
int		 index;
wav 	 temp;

int32_t  rc;
uint32_t numBlks, blkSize;
uint8_t  inquiryResult[INQUIRY_LENGTH];

uint8_t  read_flag;
uint8_t  file_counter;
int 	 wave_counter;

/* Private function prototypes -----------------------------------------------*/
void DAC_Configuration(void);
void TIM_Configuration(void);


/*********************************************************************//**
 * @brief		TIMER0 interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void TIMER0_IRQHandler(void)
{
	int32_t  fdr;
	int32_t  fdw;
	uint32_t bytes_read;

	uint32_t temp_part_counter;
	uint32_t block_counter;
	uint32_t temp_counter;

	if (TIM_GetIntStatus(LPC_TIM0, TIM_MR0_INT) == SET)
	{
		temp.bits.high = 0;
	    temp.bits.low = WAV[index];

		DAC_UpdateValue (LPC_DAC, temp.Val<<2);
		
		index++;
		if( index >= (wave_counter+1)*MAX_BUFFER_SIZE )
		{
			if(read_flag == 0) {
				read_flag = 1;
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
	TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
}

/*******************************************************************************
* Function Name  : DAC_Configuration
* Description    : DAC_Configuration program.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void DAC_Configuration(void)
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

/*******************************************************************************
* Function Name  : TIM_Configuration
* Description    : TIM_Configuration program.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void TIM_Configuration(void)
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

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/


/*
**************************************************************************************************************
*                                          MAIN FUNCTION
*
* Description: This function is the main function where the execution begins
*
* Arguments  : None
*
* Returns    : 
*
**************************************************************************************************************
*/

int main()
{
	int32_t  fdr;
	int32_t  fdw;
	uint32_t bytes_read;

	uint32_t temp_file_counter;
	uint32_t byte_counter;

	SystemInit();
    Host_Init();               /* Initialize the lpc17xx host controller                                    */
    rc = Host_EnumDev();       /* Enumerate the device connected                                            */
				
    if (rc == OK) {
		/* Initialize the mass storage and scsi interfaces */
        rc = MS_Init( &blkSize, &numBlks, inquiryResult );
        if (rc == OK) {
            rc = FAT_Init();   /* Initialize the FAT16 file system                                          */
            if (rc == OK) {
				fdr = FILE_Open(FILENAME_R, RDONLY);
				if (fdr > 0) {
					index = 0;
					read_flag = 0;
					wave_counter = 0;
					for(file_counter=0;file_counter<6;file_counter++) {
						bytes_read = FILE_Read(fdr, UserBuffer, MAX_BUFFER_SIZE);
						for(byte_counter=0;byte_counter<bytes_read;byte_counter++) {
							WAV[wave_counter*MAX_BUFFER_SIZE + byte_counter] = UserBuffer[byte_counter];
						}
						wave_counter++;	
					}
					wave_counter = 0;
					
					DAC_Configuration();
					TIM_Configuration();

					while(1) {
						if(read_flag == 1) {
							bytes_read = FILE_Read(fdr, UserBuffer, MAX_BUFFER_SIZE);
							for(byte_counter=0;byte_counter<bytes_read;byte_counter++) {
								WAV[((wave_counter+5)%6)*MAX_BUFFER_SIZE + byte_counter] = UserBuffer[byte_counter];
							}
							read_flag = 0;
					    }
					}
			        FILE_Close(fdr); 
			    }
            } else {
                return (0);
            }
        } else {
            return (0);
        }
    } else {							
        return (0);
    }
}

/*
**************************************************************************************************************
*                                      READ DATA FROM DISK
*
* Description: This function is used by the user to read data from the disk
*
* Arguments  : None
*
* Returns    : None
*
**************************************************************************************************************
*/

void  Main_Read (void)
{
    int32_t  fdr;
    uint32_t  bytes_read;
    

    fdr = FILE_Open(FILENAME_R, RDONLY);
    if (fdr > 0) {
        PRINT_Log("Reading from %s...\n", FILENAME_R);
        do {
            bytes_read = FILE_Read(fdr, UserBuffer, MAX_BUFFER_SIZE);
        } while (bytes_read);

        FILE_Close(fdr);
        PRINT_Log("Read Complete\n");
    } else {
        PRINT_Log("Could not open file %s\n", FILENAME_R);
        return;
    }
}

/*
**************************************************************************************************************
*                                      WRITE DATA TO DISK
*
* Description: This function is used by the user to write data to disk
*
* Arguments  : None
*
* Returns    : None
*
**************************************************************************************************************
*/

void  Main_Write (void)
{
    int32_t  fdw;
    int32_t  fdr;
    uint32_t  tot_bytes_written;
    uint32_t  bytes_written;
		
    fdr = FILE_Open(FILENAME_R, RDONLY);
    if (fdr > 0) {
        FILE_Read(fdr, UserBuffer, MAX_BUFFER_SIZE);
        fdw = FILE_Open(FILENAME_W, RDWR);
        if (fdw > 0) {
            tot_bytes_written = 0;
            PRINT_Log("Writing to %s...\n", FILENAME_W);
            do {
                bytes_written = FILE_Write(fdw, UserBuffer, MAX_BUFFER_SIZE);
                tot_bytes_written += bytes_written;
            } while (tot_bytes_written < WRITE_SIZE);
            FILE_Close(fdw);
            PRINT_Log("Write completed\n");
        } else {
            PRINT_Log("Could not open file %s\n", FILENAME_W);
            return;
        }
        FILE_Close(fdr);
    } else {
        PRINT_Log("Could not open file %s\n", FILENAME_R);
        return;
    }
}

/*
**************************************************************************************************************
*                                    COPYING A FILE
*
* Description: This function is used by the user to copy a file
*
* Arguments  : None
*
* Returns    : None
*
**************************************************************************************************************
*/

void  Main_Copy (void)
{
	int32_t  fdr;
	int32_t  fdw;
	uint32_t bytes_read;
		
    fdr = FILE_Open(FILENAME_R, RDONLY);
    if (fdr > 0) {
        fdw = FILE_Open(FILENAME_W, RDWR);
        if (fdw > 0) {
            FILE_Close(fdw);
        } else {
            return;
        }
        FILE_Close(fdr); 
    } else {
        return;
    }
}
