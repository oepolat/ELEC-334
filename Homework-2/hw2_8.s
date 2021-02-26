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
	
	LDR r1, =0x20000100 ; setting up data to read to
	LDR r2, =0xA
	STR r2, [r1]

	LDR r1, =0x20000104
	LDR r2, =0x4
	STR r2, [r1]

	LDR r1, =0x20000108
	LDR r2, =0x7
	STR r2, [r1]
	
	LDR r1, =0x2000010C
	LDR r2, =0x0
	STR r2, [r1]
	

	LDR r0, =0x20000000 ; start address
	LDR r1, =0x0 ; start value
	LDR r2, =0x0 ; number of elements
	LDR r3, =0x00000100 ; offset value
	
average_loop
	
	LDR r4, [r0, r3] ; reading the value on the register with offset
	ADDS r1, r1, r4 ; add the value to start value
	ADDS r2, r2, #0x1 ; add 1 to element counter
	ADDS r3, r3, #0x4 ; add 4 to address offset
	CMP r4, #0x0 ; check if its 0
	BNE average_loop
	SUBS r2, r2, #0x1 ; 0 is counted too so we remove 1 from the element counter
	BL divide ; jump to division branch
	
divide 

	CMP r1, r2
    BLT divide_stop ; compare if the r1 value is lower than r2 if so jump out of division
    SUBS r1, r1, r2 ; subtract r2 from r1
    ADDS r5, r5, #1 ; add one to multiplication result
    B divide ; 

divide_stop

	STR r5, [r0]
	
    ; ////////////
    B .
    ENDP

    END
