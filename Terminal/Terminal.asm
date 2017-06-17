;--------------------------------------------------
UART        EQU     03F8H

ENTER       EQU     060H
 
WAVE_REQ    EQU     0C1H
WAVE_ACC    EQU     0C2H
WAVE_END    EQU     0C4H
          
START       EQU     0A0H
NODE        EQU     0A8H
LEAF        EQU     0B0H
STOP        EQU     0B8H
;--------------------------------------------------

;--------------------------------------------------                        
STACK	SEGMENT	STACK
	        DW  100 DUP(0)
TOP	        LABEL	WORD
STACK	ENDS
;--------------------------------------------------

;--------------------------------------------------
DATA 	SEGMENT
;INPUT FOR DELAY FUNCTION
DELAY_DATA  DW      DUP(0)

;TONE FREQUENCY
TONE        DW      DUP(0)

;MUSIC BUTTONS
PLAY        DB              'PLAY    [5]  PAUSE   [0]  STOP    [.]  $'
TRACK       DB      0DH,0AH,'TRACK-  [4]  TRACK+  [6]  $' 
VOLUME      DB      0DH,0AH,'VOLUME- [2]  VOLUME+ [8]  $' 
EMPTY_LINE  DB      0DH,0AH,'$'

;MOUSE BUTTONS STATE AND CURSOR POSITION
MOUSE_X_LEN DW      DUP(0)
MOUSE_Y_LEN DW      DUP(0)
MOUSE_X     DB      DUP(0)
MOUSE_Y     DB      DUP(0)
MOUSE_BUTTON DW      DUP(0)

;DATA READ FROM SERIAL  
READ_DATA   DB      DUP(0)
            
;GRAPH WRITE FLAG, IF SET DATA IS VALID
FLAG        DB      DUP(0)

;RECEIVED BYTES COUNTER
B_COUNTER   DW      DUP(0)
B_TMP_CNT   DW      DUP(0)
BYTES       DB  100 DUP(0) 

;LEAVES COUNTER
L_COUNTER   DW      DUP(0)
L_TMP_CNT   DW      DUP(0)
LEAVES      DB  100 DUP(0)

;DIRECTION OF NEXT MOVE    
DIRECTION   DB      DUP(0)
OFFSET_X    DB      DUP(0)
OFFSET_Y    DB      DUP(0)

;POSITION AND LEN OF NODE TO BE DRAWN
NODE_LEN    DW      DUP(0)
NODE_X      DW      DUP(0)
NODE_Y      DW      DUP(0)

;POSITION, TYPE AND LEN OF EDGE TO BE DRAWN
EDGE_LEN    DW      DUP(0)
EDGE_TYPE   DB      DUP(0)
EDGE_X      DW      DUP(0)
EDGE_Y      DW      DUP(0)

;POSITION AND SIZE OF IMAGE TO BE DRAWN
IMAGE_X_LEN DW      DUP(0)
IMAGE_Y_LEN DW      DUP(0)
IMAGE_X     DW      DUP(0)
IMAGE_Y     DW      DUP(0)

;IMAGE FILE NAME, HANDLE, BUFFER AND NUMBER OF BYTES TO READ
FILE_NAME   DW      DUP(0)
FILE_HANDLE DW      DUP(0)
FILE_BUFFER DB 30000 DUP(0)
BYTES_NUM   DW      DUP(0)

;READING IMAGE FILE TEMP VARIABLES 
BUFFER_PTR  DW      DUP(0)
TMP_COUNTER DW      DUP(0)
TMP_RES     DB      DUP(0)
TMP_BYTE    DB      DUP(0)
TMP_SHIFT   DB      DUP(0)

;IMAGE FILE NAMES
BLACK_IMAGE DB      'BLACK.TXT',0
RED_IMAGE   DB      'RED.TXT',0
GREEN_IMAGE DB      'GREEN.TXT',0
YELLOW_IMAGE DB     'YELLOW.TXT',0
BLUE_IMAGE  DB      'BLUE.TXT',0
WHITE_IMAGE DB      'WHITE.TXT',0 
SAEED       DB      'SAEED.TXT',0

DATA	ENDS
;--------------------------------------------------

;--------------------------------------------------
CODE 	SEGMENT
ASSUME 	CS:CODE,DS:DATA,ES:DATA,SS:STACK
START:	
	CALL INIT 
	CALL INIT_UART
	
	;WAIT FOR PASSWORD
	CALL VERIFY_PASS
	
	;PLAY SYSTEM STARTUP MELODY
    CALL PLAY_MELODY
    
    ;SHOW BUTTONS AND FILES ON SCREEN	
	CALL INIT_SCREEN
	              	   
AGAIN:
    ;CHECK IF MOUSE BUTTON PRESSED      
    CALL CHECK_MOUSE                   
         
    ;CHECK IF KEYBOARD PRESSED              
    CALL CHECK_KEYBOARD
    
    ;CHECK IF DATA RECEIVED FROM MAINBOARD
    CALL CHECK_SERIAL
    
    JMP AGAIN    
                   
;--------------------------------------------------

;--------------------------------------------------
INIT            PROC    NEAR
	MOV AX,DATA
	MOV DS,AX
    MOV ES,AX
    MOV AX,STACK
	MOV SS,AX
	LEA SP,TOP
	
    ;INIT RECEIVED BYTES COUNTER, LEAVES COUNTER AND RECEIVE FLAG
    MOV B_COUNTER,0H
    MOV L_COUNTER,0H
    MOV FLAG,0H
    
    ;NODES AND EDGES LEN
    MOV NODE_LEN,10
    MOV EDGE_LEN,50  
    
    ;INIT MOUSE
	MOV AX,01H
    INT 33H
    
	RET
INIT            ENDP
;--------------------------------------------------

;--------------------------------------------------	          
INIT_UART       PROC    NEAR
    ;DLAB,NO PARITY,1 STOP BIT,8 BIT
    MOV AL,83H   
    MOV DX,UART+3
    OUT DX,AL
          
    ;9600 BAUD RATE
    MOV AX,12
    MOV DX,UART
    OUT DX,AL
    INC DX
    MOV AL,AH
    OUT DX,AL  
    
    ;NOT DLAB,NO PARITY,1 STOP BIT,8 BIT
    MOV AL,03H
    MOV DX,UART+3
    OUT DX,AL
    
    ;DISABLE INTERRUPTS
    MOV AL,0H
    MOV DX,UART+1
    OUT DX,AL
    RET 
    
INIT_UART       ENDP
;--------------------------------------------------

;--------------------------------------------------
VERIFY_PASS     PROC    NEAR
CHECK_PASS:    
    MOV DX,UART+5
    IN  AL,DX
    AND AL,01H
    JNZ READ_PASS
    JMP CHECK_PASS        
    
READ_PASS:
    MOV DX,UART
    IN  AL,DX 
    CMP AL,ENTER
    JE  RIGHT_PASS
    JMP CHECK_PASS
    
RIGHT_PASS:
    RET
    
VERIFY_PASS     ENDP
;--------------------------------------------------

;--------------------------------------------------
PLAY_MELODY     PROC    NEAR
    MOV TONE,272
    CALL PLAY_TONE
    MOV TONE,294
    CALL PLAY_TONE
    MOV TONE,314
    CALL PLAY_TONE
    MOV TONE,330
    CALL PLAY_TONE
    MOV TONE,350
    CALL PLAY_TONE
    MOV TONE,370
    CALL PLAY_TONE
    MOV TONE,392
    CALL PLAY_TONE
    MOV TONE,419
    CALL PLAY_TONE
    MOV TONE,440
    CALL PLAY_TONE
    MOV TONE,475
    CALL PLAY_TONE
    
    MOV TONE,475
    CALL PLAY_TONE
    MOV TONE,440
    CALL PLAY_TONE
    MOV TONE,419
    CALL PLAY_TONE
    MOV TONE,392
    CALL PLAY_TONE
    MOV TONE,370
    CALL PLAY_TONE
    MOV TONE,350
    CALL PLAY_TONE
    MOV TONE,320
    CALL PLAY_TONE
    MOV TONE,314
    CALL PLAY_TONE
    MOV TONE,294
    CALL PLAY_TONE
    MOV TONE,272
    CALL PLAY_TONE
    RET
    
PLAY_MELODY     ENDP
;--------------------------------------------------
 
;--------------------------------------------------
PLAY_TONE       PROC    NEAR 
    MOV AL,10110110b
    OUT 43H,AL
    MOV AX,TONE
    OUT 42H,AL
    MOV AL,AH
    OUT 42H,AL
    IN  AL,61H
    OR  AL,00000011b 
    OUT 61H,AL
    MOV BX,04
    MOV DELAY_DATA,BX
    CALL DELAY
    AND AL,11111100b
    OUT 61H,AL
    RET
         
PLAY_TONE       ENDP
;--------------------------------------------------

;--------------------------------------------------
INIT_SCREEN     PROC    NEAR
    ;OPEN NEW SCREEN
    MOV AX,0012H
    INT 10H
    
    CALL SHOW_BUTTONS

    ;SEND REQUEST FOR WAVE FILE NAMES               
SEND_REQ:
    MOV DX,UART+5
    IN  AL,DX
    AND AL,020H
    JZ  SEND_REQ
    MOV DX,UART 
    MOV AL,WAVE_REQ
    OUT DX,AL
    JMP CHECK_ACC

    ;WAIT FOR ACCEPT FROM MAIN BOARD               
CHECK_ACC:    
    MOV DX,UART+5
    IN  AL,DX
    AND AL,01H
    JNZ READ_ACC
    JMP CHECK_ACC       
    
READ_ACC:
    MOV DX,UART
    IN  AL,DX
    CMP AL,WAVE_ACC                        
    JNE CHECK_ACC               
    JMP CHECK_WAVE         

    ;GET WAVE FILES AND PRINT               
CHECK_WAVE:    
    MOV DX,UART+5
    IN  AL,DX
    AND AL,01H
    JNZ READ_WAVE
    JMP CHECK_WAVE       
    
READ_WAVE:
    MOV DX,UART
    IN  AL,DX
    CMP AL,WAVE_END
    JNE PRINT  
    RET
      
PRINT:
    MOV DL,AL
    MOV AH,02H
    INT 21H
    JMP CHECK_WAVE
    
INIT_SCREEN     ENDP
;--------------------------------------------------

;--------------------------------------------------
SHOW_BUTTONS    PROC    NEAR
    LEA DX,PLAY
    MOV AH,09H
    INT 21H
    LEA DX,EMPTY_LINE
    MOV AH,09H
    INT 21H 
    LEA DX,TRACK
    MOV AH,09H
    INT 21H 
    LEA DX,EMPTY_LINE
    MOV AH,09H
    INT 21H 
    LEA DX,VOLUME
    MOV AH,09H
    INT 21H
    LEA DX,EMPTY_LINE
    MOV AH,09H
    INT 21H
    LEA DX,EMPTY_LINE
    MOV AH,09H
    INT 21H
    RET
    
SHOW_BUTTONS    ENDP 
;--------------------------------------------------

;--------------------------------------------------
CHECK_MOUSE     PROC    NEAR
    ;SHOW CURSOR
	MOV AX,01H
    INT 33H
    
    ;READ MOUSE STATE
    MOV AX,03H
    INT 33H
    MOV MOUSE_X_LEN,CX
    MOV MOUSE_Y_LEN,DX
    
    ;MOUSE BUTTONS
    AND BX,01H
    MOV MOUSE_BUTTON,BX
    CMP MOUSE_BUTTON,01H
    JE  READ_MOUSE 
    RET
    
READ_MOUSE:
    ;CURSOR WIDTH
    MOV AX,MOUSE_X_LEN
    MOV BL,16
    DIV BL
    MOV MOUSE_X,AL
    
    ;CURSOR HEIGHT
    MOV AX,MOUSE_Y_LEN
    MOV BL,16
    DIV BL
    MOV MOUSE_Y,AL
        
    MOV AL,MOUSE_Y
    CMP AL,0
    JE  CHECK_PLAY
    CMP AL,2
    JE  CHECK_TRACK
    CMP AL,4
    JE  CHECK_VOLUME
    CMP AL,6
    JAE MOUSE_SELECT
    RET           
               
;MUSIC COMMANDS            
CHECK_PLAY:
    MOV AL,MOUSE_X
    CMP AL,6
    JB  MOUSE_PLAY
    CMP AL,12
    JB  MOUSE_PAUSE
    CMP AL,18
    JB  MOUSE_STOP
    RET
CHECK_TRACK:
    MOV AL,MOUSE_X
    CMP AL,6
    JB  MOUSE_TRACK_DEC
    CMP AL,12
    JB  MOUSE_TRACK_INC
    RET
CHECK_VOLUME:
    MOV AL,MOUSE_X
    CMP AL,6
    JB  MOUSE_VOLUME_DEC
    CMP AL,12
    JB  MOUSE_VOLUME_INC
    RET          
    
MOUSE_PLAY: 
    MOV AL,'5'
    CALL SEND_COMMAND
    RET
MOUSE_PAUSE: 
    MOV AL,'0'
    CALL SEND_COMMAND
    RET
MOUSE_STOP: 
    MOV AL,'.'
    CALL SEND_COMMAND
    RET 
MOUSE_TRACK_DEC: 
    MOV AL,'2'
    CALL SEND_COMMAND
    RET 
MOUSE_TRACK_INC: 
    MOV AL,'8'
    CALL SEND_COMMAND
    RET 
MOUSE_VOLUME_DEC: 
    MOV AL,'4'
    CALL SEND_COMMAND
    RET 
MOUSE_VOLUME_INC: 
    MOV AL,'6'
    CALL SEND_COMMAND
    RET 

        
;SELECTING TRACK        
MOUSE_SELECT:    
    MOV AL,MOUSE_X
    CMP AL,8
    JB  MOUSE_SELECT_TRACK 
    RET
MOUSE_SELECT_TRACK:               
    MOV AL,MOUSE_Y
    CBW
    MOV BL,02H
    DIV BL
    ;ODD LINE NUMBER
    CMP AH,0H
    JNE INVALID_LINE
    ;HEADER STRING LINE
    CMP AL,3
    JBE INVALID_LINE
    SUB AL,3
    CALL SEND_COMMAND
    RET        
    
INVALID_LINE:
    RET
               
CHECK_MOUSE     ENDP
;--------------------------------------------------

;--------------------------------------------------
CHECK_KEYBOARD  PROC    NEAR
    MOV AH,01H
    INT 16H       
    JNZ READ_KEYBOARD
    RET
    
READ_KEYBOARD:       
    MOV AH,0H
    INT 16H          
    CALL SEND_COMMAND         
    RET
    
CHECK_KEYBOARD  ENDP
;--------------------------------------------------

;--------------------------------------------------
SEND_COMMAND    PROC    NEAR
    CMP AL,'5'
    JE  SEND_PLAY
    CMP AL,'0'
    JE  SEND_PAUSE
    CMP AL,'.'
    JE  SEND_STOP      
    CMP AL,'2'
    JE  SEND_TRACK_DEC 
    CMP AL,'8'
    JE  SEND_TRACK_INC
    CMP AL,'4'    
    JE  SEND_VOLUME_DEC
    CMP AL,'6'
    JE  SEND_VOLUME_INC  
    
    JMP SEND_SELECTED_TRACK
    
SEND_PLAY:
    MOV BL,'1'
    JMP SEND
SEND_PAUSE:
    MOV BL,'2'
    JMP SEND
SEND_STOP:
    MOV BL,'3'
    JMP SEND
SEND_TRACK_DEC:
    MOV BL,'4'    
    JMP SEND
SEND_TRACK_INC:
    MOV BL,'5'
    JMP SEND
SEND_VOLUME_DEC:
    MOV BL,'6'
    JMP SEND
SEND_VOLUME_INC:
    MOV BL,'7'
    JMP SEND

SEND_SELECTED_TRACK:
    MOV BL,AL
    JMP SEND
        
;SEND CHARACTERS FOR COMMAND, AND NUMBERS FOR TRACK SELECTION                 
SEND:
    MOV DX,UART+5
    IN  AL,DX
    AND AL,020H
    JZ  SEND
    MOV DX,UART 
    MOV AL,BL
    OUT DX,AL
    
    ;DELAY ( FOR MOUSE DEBAUNCE )
    MOV AX,3
    MOV DELAY_DATA,AX
    CALL DELAY
    RET
    
SEND_COMMAND    ENDP
;--------------------------------------------------

;--------------------------------------------------
CHECK_SERIAL    PROC    NEAR
    MOV DX,UART+5
    IN  AL,DX
    AND AL,01H
    JNZ READ_SERIAL
    RET        
    
READ_SERIAL:
    MOV DX,UART
    IN  AL,DX
    MOV READ_DATA,AL                        
    JMP GET_DATA
                                                                  
GET_DATA:        
    MOV AL,READ_DATA
    AND AL,0E0H
    CMP AL,0A0H
    JE  GRAPH
    MOV DL,'1'
    JMP ERROR
     
;READ GRAPH DATA    
GRAPH:
    MOV AL,READ_DATA    
    AND AL,0F8H
    CMP AL,START
    JE  START_READ    
    CMP AL,NODE
    JE  NODE_READ    
    CMP AL,LEAF
    JE  LEAF_READ
    CMP AL,STOP
    JE  STOP_READ
    MOV DL,'2'
    JMP ERROR
    
START_READ:
    MOV B_COUNTER,0H
    MOV L_COUNTER,0H
    MOV FLAG,01H
    RET

NODE_READ:
    CMP FLAG,01H
    JE  VALID_NODE
    RET
VALID_NODE:
    LEA BX,BYTES
    ADD BX,B_COUNTER
    MOV [BX],01H
    INC B_COUNTER
    RET
    

LEAF_READ:
    CMP FLAG,01H
    JE  VALID_LEAF
    RET
VALID_LEAF:
    LEA BX,BYTES
    ADD BX,B_COUNTER
    MOV [BX],02H
    INC B_COUNTER
    MOV AL,READ_DATA
    AND AL,07H
    LEA BX,LEAVES
    ADD BX,L_COUNTER
    MOV [BX],AL
    INC L_COUNTER
    RET

STOP_READ:
    CALL SHOW_GRAPH
   
    MOV FLAG,0H
    MOV B_COUNTER,0H 
    RET
          
CHECK_SERIAL    ENDP
;--------------------------------------------------
   
;--------------------------------------------------
SHOW_GRAPH      PROC    NEAR
    ;OPEN NEW SCREEN IN SVGA MODE
    MOV AX,4F02H
	MOV BX,107H
    INT 10H
    
    ;START NODE
    MOV AX,10
    MOV NODE_X,AX
    MOV IMAGE_X,AX
    MOV AX,620
    MOV NODE_Y,AX 
    MOV IMAGE_Y,AX
    MOV IMAGE_X_LEN,100
    MOV IMAGE_Y_LEN,50
    LEA AX,BLACK_IMAGE
	MOV FILE_NAME,AX
    CALL DRAW_IMAGE             
    
    MOV B_TMP_CNT,0H
    MOV L_TMP_CNT,0H
    MOV DIRECTION,01H
NEXT_BYTE:
    LEA BX,BYTES
    ADD BX,B_TMP_CNT
    MOV AL,[BX]
    CMP AL,01H
    JE  NODE_BYTE
    CMP AL,02H
    JE  LEAF_BYTE
    MOV DL,'3'
    JMP ERROR
    
NODE_BYTE:
    MOV AL,DIRECTION
    CMP AL,01H
    JE  NODE_LEFT
    CMP AL,02H
    JE  NODE_DOWN
    CMP AL,03H
    JE  NODE_RIGHT
    CMP AL,04H
    JE  NODE_UP
    MOV DL,'4'
    JMP ERROR
    
NODE_LEFT:
    ;SET H_LINE LEN
    CALL SET_EDGE_LEN
    
    MOV EDGE_TYPE,01H
    MOV AX,NODE_X
    MOV EDGE_X,AX
    MOV AX,NODE_Y
    MOV EDGE_Y,AX
    MOV AX,NODE_LEN
    ADD EDGE_X,AX
    CALL DRAW_EDGE                                    
    
    MOV AX,NODE_LEN
    ADD NODE_X,AX
    MOV AX,EDGE_LEN
    ADD NODE_X,AX
    CALL DRAW_NODE 
                                 
    MOV DIRECTION,01H
    ;PUSH 1 IN STACK ( RIGHT SON )
    MOV AX,01H
    PUSH AX
    JMP FINISH
    
NODE_DOWN:
    MOV EDGE_LEN,110    
    MOV EDGE_TYPE,02H
    MOV AX,NODE_X
    MOV EDGE_X,AX
    MOV AX,NODE_Y
    MOV EDGE_Y,AX
    MOV AX,NODE_LEN
    ADD EDGE_X,AX
    CALL DRAW_EDGE   
                       
    MOV AX,EDGE_LEN     
    SUB NODE_Y,AX
    MOV AX,NODE_LEN
    SUB NODE_Y,AX
    CALL DRAW_NODE 
    
    MOV DIRECTION,01H
    ;PUSH 1 IN STACK ( UP SON )
    MOV AX,02H
    PUSH AX
    JMP FINISH

NODE_RIGHT:
    ;SET H_LINE LEN
    CALL SET_EDGE_LEN
          
    MOV AX,NODE_LEN
    SUB NODE_X,AX
    MOV AX,EDGE_LEN
    SUB NODE_X,AX
    
    MOV DIRECTION,02H
    JMP FINISH
    
NODE_UP: 
    ;SET V_LINE LEN
    MOV EDGE_LEN,110
    
    MOV AX,NODE_LEN
    ADD NODE_Y,AX
    MOV AX,EDGE_LEN
    ADD NODE_Y,AX
                
    POP AX
    CMP AX,01H
    JE  RIGHT_DIR
    CMP AX,02H
    JE  UP_DIR
    MOV DL,'5'
    JMP ERROR    
RIGHT_DIR:
    MOV DIRECTION,03H
    JMP FINISH
UP_DIR:
    MOV DIRECTION,04H
    JMP FINISH
              
              
LEAF_BYTE:
    LEA BX,LEAVES
    ADD BX,L_TMP_CNT
    MOV AL,[BX]
    CMP AL,0H
    JE  BLACK
    CMP AL,01H
    JE  RED
    CMP AL,02H
    JE  GREEN
    CMP AL,03H
    JE  YELLOW
    CMP AL,04H
    JE  BLUE
    CMP AL,07H
    JE  WHITE
    MOV DL,'6'
    JMP ERROR
    
BLACK:
    LEA AX,BLACK_IMAGE
	MOV FILE_NAME,AX
    MOV IMAGE_X_LEN,100
    MOV IMAGE_Y_LEN,50
    JMP DRAW_LEAF
RED:
    LEA AX,RED_IMAGE
    MOV FILE_NAME,AX
    MOV IMAGE_X_LEN,50
    MOV IMAGE_Y_LEN,50
    JMP DRAW_LEAF
GREEN:
    LEA AX,GREEN_IMAGE
    MOV FILE_NAME,AX 
    MOV IMAGE_X_LEN,50
    MOV IMAGE_Y_LEN,50
    JMP DRAW_LEAF
YELLOW:
    LEA AX,YELLOW_IMAGE
    MOV FILE_NAME,AX
    MOV IMAGE_X_LEN,50
    MOV IMAGE_Y_LEN,50
    JMP DRAW_LEAF
BLUE:
    LEA AX,BLUE_IMAGE
    MOV FILE_NAME,AX 
    MOV IMAGE_X_LEN,50
    MOV IMAGE_Y_LEN,50
    JMP DRAW_LEAF
WHITE:
    LEA AX,WHITE_IMAGE
    MOV FILE_NAME,AX
    MOV IMAGE_X_LEN,100
    MOV IMAGE_Y_LEN,50
    JMP DRAW_LEAF
    
DRAW_LEAF:     
    MOV AL,DIRECTION
    CMP AL,01H
    JE  LEAF_LEFT
    CMP AL,02H
    JE  LEAF_DOWN
    MOV DL,'7'
    JMP ERROR

LEAF_LEFT:
    ;SET H_LINE LEN
    CALL SET_EDGE_LEN
    
    MOV EDGE_TYPE,01H
    MOV AX,NODE_X
    MOV EDGE_X,AX
    MOV AX,NODE_Y
    MOV EDGE_Y,AX
    MOV AX,NODE_LEN
    ADD EDGE_X,AX
    CALL DRAW_EDGE   
    
    MOV AX,NODE_LEN
    ADD NODE_X,AX
    MOV AX,EDGE_LEN
    ADD NODE_X,AX
    
    MOV AX,NODE_X
    MOV IMAGE_X,AX
    MOV AX,NODE_Y
    MOV IMAGE_Y,AX
    CALL DRAW_IMAGE
                                 
    MOV DIRECTION,03H
    JMP FINISH

LEAF_DOWN: 
    ;SET V_LINE LEN
    MOV EDGE_LEN,110
        
    MOV EDGE_TYPE,02H
    MOV AX,NODE_X
    MOV EDGE_X,AX
    MOV AX,NODE_Y
    MOV EDGE_Y,AX
    MOV AX,NODE_LEN
    ADD EDGE_X,AX
    CALL DRAW_EDGE   
                      
    MOV AX,EDGE_LEN     
    SUB NODE_Y,AX
    MOV AX,NODE_LEN
    SUB NODE_Y,AX
    
    MOV AX,NODE_X
    MOV IMAGE_X,AX
    MOV AX,NODE_LEN
    ADD IMAGE_X,AX
    MOV AX,NODE_Y
    MOV IMAGE_Y,AX
    MOV AX,NODE_LEN
    ADD IMAGE_Y,AX    
    MOV AX,IMAGE_Y_LEN
    SUB IMAGE_Y,AX
    CALL DRAW_IMAGE
    
    MOV DIRECTION,04H
    JMP FINISH
    
            
FINISH:   
    ;INC BYTE COUNTER
    INC B_TMP_CNT
    MOV AX,B_TMP_CNT
    CMP AX,B_COUNTER
    JB  NEXT_BYTE
    
    ;ON FINISH, WAIT UNTILL KEY PRESS
WAIT_FOR_KEYBOARD:
    MOV AH,01H
    INT 16H
    JZ  WAIT_FOR_KEYBOARD
    
    ;CLEAR GRAPH AND DISPLAY BUTTONS SCREEN                     
    CALL INIT_SCREEN
    
    RET
                
SHOW_GRAPH      ENDP
;--------------------------------------------------

;--------------------------------------------------
SET_EDGE_LEN    PROC    NEAR
    MOV AX,NODE_Y
    CMP AX,20
    JZ  LEVEL1
    CMP AX,140
    JZ  LEVEL2
    CMP AX,260
    JZ  LEVEL3
    CMP AX,380
    JZ  LEVEL4
    CMP AX,500
    JZ  LEVEL5
    CMP AX,620
    JZ  LEVEL6
    MOV DL,'8'
    JMP ERROR

LEVEL1:    
    MOV EDGE_LEN,20
    RET  
LEVEL2:    
    MOV EDGE_LEN,20
    RET  
LEVEL3:    
    MOV EDGE_LEN,20
    RET  
LEVEL4:    
    MOV EDGE_LEN,90
    RET  
LEVEL5:    
    MOV EDGE_LEN,160
    RET  
LEVEL6:    
    MOV EDGE_LEN,230
    RET  
    
SET_EDGE_LEN    ENDP    
;--------------------------------------------------

;--------------------------------------------------
DRAW_NODE       PROC    NEAR
    MOV AL,30    
    MOV AH,0CH
    MOV BX,NODE_LEN
    INC BX
    
    MOV DX,0    
NODE_ROW:
    MOV CX,0
NODE_COL:
    PUSH CX
    PUSH DX
    ADD CX,NODE_X
    ADD DX,NODE_Y
    INT 10H
    POP DX
    POP CX
    INC CX
    CMP CX,BX
    JNZ NODE_COL
    INC DX
    CMP DX,BX
    JNZ NODE_ROW
    RET
    
DRAW_NODE       ENDP    
;--------------------------------------------------

;--------------------------------------------------
DRAW_EDGE       PROC    NEAR
    MOV AL,30   
    MOV AH,0CH 
    MOV CX,EDGE_X
    MOV DX,EDGE_Y
    MOV BX,EDGE_LEN
    CMP EDGE_TYPE,01H
    JE  H_LINE
    CMP EDGE_TYPE,02H
    JE  V_LINE
    MOV DL,'9'
    JMP ERROR

H_LINE:                  
    INT 10H
    INC CX
    DEC BX
    JNZ H_LINE
    RET
    
V_LINE:
    INT 10H
    DEC DX
    DEC BX
    JNZ V_LINE
    RET
           
DRAW_EDGE       ENDP
;--------------------------------------------------

;--------------------------------------------------
DRAW_IMAGE      PROC    NEAR
    ;READ IMAGE FROM FILE 
	MOV BYTES_NUM,29000
	CALL FILE_OPEN
	CALL FILE_READ
	CALL FILE_CLOSE
    
    
    ;SET SCREEN COLOR PALETTE TO GRAYSCALE
    MOV CX,255
COLOR_LOOP:
    PUSH CX
    MOV AX,1010H
    MOV BX,CX
    MOV CH,CL
    MOV DH,CH
    INT 10H
    POP CX
    DEC CX
    JNZ COLOR_LOOP
            
      
    ;DRAW IMAGE          
    MOV AH,0CH
    LEA SI,FILE_BUFFER
    MOV DX,0    
IMAGE_ROW:
    MOV CX,0
IMAGE_COL:
    MOV AL,[SI]
    SHR AL,1
    SHR AL,1
    PUSH CX
    PUSH DX
    ADD CX,IMAGE_X
    ADD DX,IMAGE_Y
    INT 10H
    POP DX
    POP CX
    INC SI
    INC CX
    CMP CX,IMAGE_X_LEN
    JNZ IMAGE_COL
    INC DX
    CMP DX,IMAGE_Y_LEN
    JNZ IMAGE_ROW
      
            
    ;DRAW IMAGE MARGIN
    MOV AL,06H
    MOV AH,0CH
    
    MOV DX,IMAGE_Y
    MOV CX,0
MARGIN_X_LOW:
    PUSH CX
    ADD CX,IMAGE_X
    INT 10H
    POP CX
    INC CX
    CMP CX,IMAGE_X_LEN
    JNZ MARGIN_X_LOW
    
    MOV DX,IMAGE_Y
    ADD DX,IMAGE_Y_LEN
    MOV CX,0
MARGIN_X_HIGH:
    PUSH CX
    ADD CX,IMAGE_X
    INT 10H
    POP CX
    INC CX
    CMP CX,IMAGE_X_LEN
    JNZ MARGIN_X_HIGH
    
    MOV DX,0
    MOV CX,IMAGE_X
MARGIN_Y_LOW:
    PUSH DX
    ADD DX,IMAGE_Y
    INT 10H
    POP DX
    INC DX
    CMP DX,IMAGE_Y_LEN
    JNZ MARGIN_Y_LOW
    
    MOV DX,0
    MOV CX,IMAGE_X     
    ADD CX,IMAGE_X_LEN
MARGIN_Y_HIGH:
    PUSH DX
    ADD DX,IMAGE_Y
    INT 10H
    POP DX
    INC DX
    CMP DX,IMAGE_Y_LEN
    JNZ MARGIN_Y_HIGH
    
      
    INC L_TMP_CNT
    RET
    
DRAW_IMAGE      ENDP
;--------------------------------------------------

;--------------------------------------------------	          
FILE_OPEN       PROC    NEAR
    MOV AH,03DH
    MOV AL,0H 
    MOV DX,FILE_NAME
    INT 21H  
    JC  FILE_OPEN_ERROR
    MOV FILE_HANDLE,AX 
    RET
    
FILE_OPEN_ERROR:
    MOV DL,'A'    
    JMP ERROR
    
FILE_OPEN       ENDP
;--------------------------------------------------

;--------------------------------------------------	          
FILE_READ       PROC    NEAR      
    LEA DX,FILE_BUFFER
    MOV BUFFER_PTR,DX
    MOV TMP_COUNTER,0H
    
READ_NEXT_BYTE: 
    MOV AX,TMP_COUNTER
    CMP AX,BYTES_NUM
    JE  FILE_READ_FINISH
    
    MOV TMP_RES,0H
    MOV TMP_BYTE,0H
    MOV TMP_SHIFT,0H
    
    MOV BX,FILE_HANDLE
    LEA DX,TMP_BYTE
    MOV AH,03FH
    MOV CX,01H
    INT 21H
    JC  FILE_READ_ERROR
    CMP AX,01H
    JNE FILE_READ_FINISH       
    MOV AL,TMP_BYTE
    SUB AL,'0'
    MOV TMP_BYTE,AL    
    
READ_NEXT_DIGIT:
    MOV AL,TMP_BYTE
    MOV TMP_RES,AL
    
    MOV BX,FILE_HANDLE
    LEA DX,TMP_BYTE
    MOV AH,03FH
    MOV CX,1
    INT 21H
    JC  FILE_READ_ERROR 
    CMP AX,01H
    JNE FILE_READ_FINISH       
    MOV AL,TMP_BYTE
    CMP AL,','
    JE  STORE_RES
    
    MOV AL,TMP_BYTE
    SUB AL,'0'
    MOV TMP_BYTE,AL
    MOV AL,TMP_RES
    SHL AL,1
    SHL AL,1
    SHL AL,1
    MOV TMP_SHIFT,AL
    MOV AL,TMP_RES
    SHL AL,1
    ADD AL,TMP_SHIFT
    ADD AL,TMP_BYTE
    MOV TMP_BYTE,AL
    JMP READ_NEXT_DIGIT
    
STORE_RES:
    MOV BX,BUFFER_PTR
    MOV AL,TMP_RES
    MOV [BX],AL
    
    INC BUFFER_PTR
    INC TMP_COUNTER
    JMP READ_NEXT_BYTE 
    
FILE_READ_FINISH:
    RET
    
FILE_READ_ERROR:
    MOV DL,'B'
    JMP ERROR 
    
FILE_READ       ENDP
;--------------------------------------------------

;--------------------------------------------------	          
FILE_CLOSE      PROC    NEAR
    MOV BX,FILE_HANDLE
    MOV AH,03EH
    INT 21H  
    JC  FILE_CLOSE_ERROR
    RET
    
FILE_CLOSE_ERROR:
    MOV DL,'C'    
    JMP ERROR
    
FILE_CLOSE      ENDP
;--------------------------------------------------

;--------------------------------------------------
DELAY           PROC    NEAR 
    MOV AH,0H
    INT 01AH
    MOV CX,DELAY_DATA
    ADD DX,CX
    MOV BX,DX 
DELAY_LOOP: 
    INT 01AH
    CMP DX,BX
    JL  DELAY_LOOP
    RET
    
DELAY           ENDP 
;--------------------------------------------------

;--------------------------------------------------
ERROR:
    MOV AH,02H
    INT 21H
    MOV DL,'E'
    MOV AH,02H
    INT 21H    
    JMP $
;--------------------------------------------------

CODE 	ENDS
;--------------------------------------------------

END 	START