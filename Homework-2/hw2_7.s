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

	LDR r0, =0x14224 ; setting the test values to the addresses
	LDR r1, =0x1A
	STR r1, [r0]
	
	LDR r0, =0x14228
	LDR r1, =0x19
	STR r1, [r0]

	LDR r0, =0x14224 ; address values for read and write
	LDR r1, =0x14228
	LDR r2, =0x1422C
	
	LDR r3, [r0] ; getting the values from given addresses
	LDR r4, [r1]
	
	EORS r3, r3, r4 ; performing XOR operation on two values
	
	LDR r5, =0x0 ; loop counter
	
kernighan_loop

	CMP r3, #0x0
	BEQ loop_stop
	SUBS r4, r3, #0x1
	ANDS r3, r3, r4
	ADDS r5, r5, #0x1
	B kernighan_loop
	
loop_stop

	STR r5, [r2] ; writing the hamming distance to given address
	
	; ////////////
	B .
	ENDP

	END