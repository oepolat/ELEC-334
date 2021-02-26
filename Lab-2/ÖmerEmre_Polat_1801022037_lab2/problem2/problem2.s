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
.equ GPIOC_MODER,   (0x50000800)

.equ GPIOA_IDR,     (0x50000010)
.equ GPIOA_ODR,     (0x50000014)

.equ GPIOC_IDR,     (0x50000810)
.equ GPIOC_ODR,     (0x50000814)


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
	ldr r1, =(RCC_IOPENR) /* RCC with IOPENR offset */
	ldr r0, [r1]
	
	ldr r2, =0x1 /* A port enable */
	orrs r0, r0, r2 /* change the corresponding bit to 1 */
	str r0, [r1] /* enable GPIOC clock */

	/*//////ENABLE GPIOC CLOCK//////*/
	ldr r1, =(RCC_IOPENR) /* RCC with IOPENR offset */
	ldr r0, [r1]
	
	ldr r2, =0x4 /* C port enable */
	orrs r0, r0, r2 /* change the corresponding bit to 1 */
	str r0, [r1] /* enable GPIOC clock */
	
	/*//////SETUP A6 MODER//////*/
	ldr r1, =(GPIOA_MODER)
	ldr r0, [r1]
	
	ldr r2, =0x3000
	mvns r2, r2
	ands r0, r0, r2
	str r0, [r1] /* writing back the modified MODER bits */
	
	/*//////SETUP C6 MODER//////*/
	ldr r1, =(GPIOC_MODER)
	ldr r0, [r1]
	
	ldr r2, =0x2000
	mvns r2, r2
	ands r0, r0, r2
	ldr r2, =0x1000
	orrs r0, r0, r2 /* ands and orrs to turn the C6 to output */
	str r0, [r1] /* writing back the modified MODER bits */
	
    LED_OFF:
	
	ldr r1, =(GPIOC_ODR) /* GPIOC with ODR offset */
	ldr r0, [r1]
	ldr r2, =0x40
	mvns r2, r2
	ands r0, r0, r2 /* and operation to set the given bit low */
	str r0, [r1]
	
    CHECK_INPUT_LOOP_1:
	
	ldr r1, =(GPIOA_IDR) /* GPIOA with IDR ofset */
	ldr r0, [r1]
	movs r2, #0x40
	ands r0, r0, r2
	cmp r0, r2
	beq LED_ON
	b CHECK_INPUT_LOOP_1
	
    LED_ON: /* branch to turn the led on */

	ldr r1, =GPIOC_ODR
	ldr r0, [r1]
	ldr r2, =0x40
	orrs r0, r0, r2
	str r0, [r1]
	
    CHECK_INPUT_LOOP_2:
	
	ldr r1, =(GPIOA_IDR) /* GPIOA with IDR ofset */
	ldr r0, [r1]
	movs r2, #0x40
	mvns r3, r2
	orrs r0, r0, r3
	cmp r0, r3
	beq LED_OFF
	b CHECK_INPUT_LOOP_2

	/* for(;;); */
	b .

	/* this should never get executed */
	nop
