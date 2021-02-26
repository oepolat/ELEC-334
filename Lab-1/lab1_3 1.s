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

	LDR r0, =(0x50000000 + 0x14) ; GPIOA with ODR offset
    LDR r2, [r0] ; take the value from the r0 address
    LDR r1, =0x1800 ; number with only the 12th and 11th bit as a 1
    ORRS r2, r2, r1 ; perform OR operation with the r1 
    STR r2, [r0] ; write back the new value that has its 12th and 11th bit as 1
	
	LDR r0, =(0x50000400 + 0x14) ; GPIOB with ODR offset
    LDR r2, [r0] ; take the value from the r0 address
    LDR r1, =0x30 ; number with only the 4th and 5th bit as a 1
    ORRS r2, r2, r1 ; perform OR operation with the r1 
    STR r2, [r0] ; write back the new value that has its 4th and 5th bit as 1

    ; ////////////
    B .
    ENDP

    END
