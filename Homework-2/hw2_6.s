Stack_Size      EQU     0x00000400

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp

    THUMB

    AREA    RESET, DATA, READONLY
    EXPORT  __Vectors

__Vectors
    DCD     __initial_sp                   ; Top of Stack
    DCD     Reset_Handler                  ; Reset Handler
    DCD     NMI_Handler                    ; NMI Handler
    DCD     HardFault_Handler              ; Hard Fault Handler

    AREA    |.text|, CODE, READONLY


; nmi handler
NMI_Handler    PROC
    EXPORT  NMI_Handler
    B .
    ENDP


; hardfault handler
HardFault_Handler    PROC
    EXPORT  HardFault_Handler
    B .
    ENDP


; entry function
Reset_Handler    PROC
    EXPORT  Reset_Handler
    ; ////////////

LED_ON ; branch to turn the led on

	LDR r3, =(0x50000400 + 0x14)
	LDR r2, [r3]
	LDR r1, =0x1000
	ORRS r2, r2, r1 ; or operation to set the given bit high
	STR r2, [r3]
	
	LDR r4,=0x7A1200 ; cycles for 1 sec total delay
	

ON_DELAY ; on time delay
    
	CMP r4, #0x0
	BEQ LED_OFF
	SUBS r4, r4, #1
	B ON_DELAY

LED_OFF ; branch to turn off the led
    
	LDR r3, =(0x50000400 + 0x14)
	LDR r2, [r3]
	LDR r1, =0xFFFFEFFF
	ANDS r2, r2, r1 ; and operation to set the given bit low
	STR r2, [r3]
	
	LDR r4,=0x249F60

OFF_DELAY ; off time delay
	
	CMP r4, #0x0
	BEQ LED_ON
	SUBS r4, r4, #1
	B OFF_DELAY

    ; ////////////
    B .
    ENDP

    END
