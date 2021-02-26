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


/* define peripheral addresses from RM0444 page 57, Tables 3-4 */
.equ RCC_IOPENR,    (0x40021034)
.equ GPIOC_MODER,   (0x50000800)
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
	/*//////ENABLE GPIOC CLOCK//////*/
	ldr r1, =(RCC_IOPENR) /* RCC with IOPENR offset */
	ldr r0, [r1]
	
	ldr r2, =0x4
	orrs r0, r0, r2 /* change the corresponding bit to 1 */
	str r0, [r1] /* enable GPIOC clock */
	
	/*//////SETUP C6 MODER//////*/
	ldr r1, =(GPIOC_MODER)
	ldr r0, [r1]
	
	ldr r2, =0x2000
	mvns r2, r2
	ands r0, r0, r2
	ldr r2, =0x1000
	orrs r0, r0, r2 /* ANDS and orrs to turn the C6 to output mode */
	str r0, [r1] /* writing back the modified MODER bits */
	
	
    LED_ON: /* branch to turn the led on */

	ldr r1, =GPIOC_ODR
	ldr r0, [r1]
	ldr r2, =0x40
	orrs r0, r0, r2
	str r0, [r1]
	
	ldr r4,=0xC35000 /* cycles for 1 sec total delay */

    ON_DELAY: /* on time delay */
    
	cmp r4, #0x0
	beq LED_OFF
	subs r4, r4, #1
	b ON_DELAY

    LED_OFF: /* branch to turn off the led */
    
	ldr r1, =(GPIOC_ODR) /* GPIOC with ODR offset */
	ldr r0, [r1]
	ldr r2, =0x40
	mvns r2, r2
	ands r0, r0, r2 /* and operation to set the given bit low */
	str r0, [r1]
	
	ldr r4,=0xC35000

    OFF_DELAY: /* off time delay */
	
	cmp r4, #0x0
	beq LED_ON
	subs r4, r4, #1
	b OFF_DELAY

	/* for(;;); */
	b .

	/* this should never get executed */
	nop
