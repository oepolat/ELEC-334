.syntax unified
.cpu cortex-m0
.fpu softvfp
.thumb


/* make linker see this */
.global Reset_Handler

/* get these from linker script */
.word _sdata
.word _edata
.word _sbss
.word _ebss


/* define peripheral addresses from RM0444 */
.equ RCC_IOPENR,    (0x40021034)

.equ GPIOA_MODER,   (0x50000000)
.equ GPIOB_MODER,   (0x50000400)

.equ GPIOA_IDR,     (0x50000010)
.equ GPIOA_ODR,     (0x50000014)

.equ GPIOB_IDR,     (0x50000410)
.equ GPIOB_ODR,     (0x50000414)


/* vector table, +1 thumb mode */
.section .vectors
vector_table:
	.word _estack             /*     Stack pointer */
	.word Reset_Handler +1    /*     Reset handler */
	.word Default_Handler +1  /*       NMI handler */
	.word Default_Handler +1  /* HardFault handler */
	/* add rest of them here if needed */


/* reset handler */
.section .text
Reset_Handler:
	/* set stack pointer */
	ldr r0, =_estack
	mov sp, r0

	/* initialize data and bss 
	 * not necessary for rom only code 
	 * */
	bl init_data
	/* call main */
	bl main
	/* trap if returned */
	b .


/* initialize data and bss sections */
.section .text
init_data:

	/* copy rom to ram */
	ldr r0, =_sdata
	ldr r1, =_edata
	ldr r2, =_sidata
	movs r3, #0
	b LoopCopyDataInit

	CopyDataInit:
		ldr r4, [r2, r3]
		str r4, [r0, r3]
		adds r3, r3, #4

	LoopCopyDataInit:
		adds r4, r0, r3
		cmp r4, r1
		bcc CopyDataInit

	/* zero bss */
	ldr r2, =_sbss
	ldr r4, =_ebss
	movs r3, #0
	b LoopFillZerobss

	FillZerobss:
		str  r3, [r2]
		adds r2, r2, #4

	LoopFillZerobss:
		cmp r2, r4
		bcc FillZerobss

	bx lr


/* default handler */
.section .text
Default_Handler:
	b Default_Handler


/* main function */
.section .text
main:

    /*//////ENABLE GPIOA CLOCK//////*/
	LDR r1, =(RCC_IOPENR) /* RCC with IOPENR offset */
	LDR r0, [r1]
	
	LDR r2, =0x1 /* A port enable */
	ORRS r0, r0, r2 /* change the corresponding bit to 1 */
	STR r0, [r1] /* enable GPIOA clock */
	
	/*//////ENABLE GPIOB CLOCK//////*/
	LDR r1, =(RCC_IOPENR) /* RCC with IOPENR offset */
	LDR r0, [r1]
	
	LDR r2, =0x2 /* for B port */
	ORRS r0, r0, r2 /* change the corresponding bit to 1 */
	STR r0, [r1] /* enable GPIOB clock */
	
	/*//////SETUP A6 MODER//////*/
	LDR r1, =(GPIOA_MODER)
	LDR r0, [r1]
	
	LDR r2, =0x3000
	MVNS r2, r2
	ANDS r0, r0, r2
	STR r0, [r1] /* writing back the modified MODER bits */
	
	/*//////SETUP B PINS MODER//////*/
	LDR r1, =(GPIOB_MODER)
	LDR r0, [r1]
	
	LDR r2, =0xAAAA
	MVNS r2, r2
	ANDS r0, r0, r2
	LDR r2, =0x5555
	ORRS r0, r0, r2
	STR r0, [r1] /* writing back the modified MODER bits */
	
	/*//////SETUP VARIABLES//////*/
	LDR r1, =GPIOB_ODR /* GPIOB with ODR offset */
	LDR r0, =0x83838383 /* rotate value */
	MOVS r2, #0x1 /* rotate counter */
	MOVS r5, #0x0 /* toggle register */
	
LED_SET: /* branch to set the led */
	
	LDR r3, =0xFF
	ANDS r3, r3, r0 /* only take the first 8 bits */
	STR r3, [r1]
	
	LDR r4,=0x138800 /* cycles for 100ms total delay */

ON_DELAY: /* on time delay */
    
	SUBS r4, r4, #1
	CMP r4, #0x0
	BNE ON_DELAY

	/*//////INPUT PIN CHECK//////*/
	LDR r7, =(GPIOA_IDR) /* GPIOA with IDR ofset */
	LDR r6, [r7]
	MOVS r3, #0x40
	ANDS r6, r6, r3
	CMP r6, r3
	BEQ TOGGLE /* if input is present toggle the register */
	B TOGGLE_CHECK /* else go to toggle check */
	
TOGGLE:
	/* toggle the toggle register */
	MVNS r5, r5

TOGGLE_CHECK:
	/* if the toggle register is set rotate left else rotate right */
	CMP r5, #0x0
	BNE ROTATE_LEFT /* if toggle is not 0 then rotate left */

ROTATE_RIGHT:
	/* rotates right once */
	RORS r0, r0, r2
	B LED_SET

ROTATE_LEFT:
	/* rotating right (period-1) times will be the same as rotating 1 left */
	MOVS r3, #0x7
	RORS r0, r0, r3
	B LED_SET

	/* for(;;); */
	b .

	/* this should never get executed */
	nop
