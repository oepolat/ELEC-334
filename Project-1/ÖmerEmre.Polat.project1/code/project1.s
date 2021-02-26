/* Ömer Emre POLAT */
/* 1801022037 */

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
	/* //////ENABLE GPIOA CLOCK////// */
	ldr r1, =(0x40021034) /* RCC with IOPENR offset */
	ldr r0, [r1]

	ldr r2, =0x1 /* A port enable */
	orrs r0, r0, r2 /* change the corresponding bit to 1 */
	str r0, [r1] /* enable GPIOA clock */

	/* //////ENABLE GPIOB CLOCK////// */
	ldr r1, =(0x40021034) /* RCC with IOPENR offset */
	ldr r0, [r1]

	ldr r2, =0x2 /* for B port */
	orrs r0, r0, r2 /* change the corresponding bit to 1 */
	str r0, [r1] /* enable GPIOB clock */

	/* //////SETUP A PINS MODER////// */
	ldr r1, =(0x50000000)
	ldr r0, [r1]

	ldr r2, =0xFFFFFFFC /* set A0 pin to input */
	ands r0, r0, r2

	ldr r2, =0xFFFF55FF /* set A4-7 pins to output */
	ands r0, r0, r2

	ldr r2, =0x5500
	orrs r0, r0, r2
	str r0, [r1] /* writing back the modified MODER bits */

	/* //////SETUP B PINS MODER////// */
	ldr r1, =(0x50000400)
	ldr r0, [r1]

	ldr r2, =0xFFFF5555 /* set B0-7 pins to output */
	ands r0, r0, r2
	ldr r2, =0x5555
	orrs r0, r0, r2
	str r0, [r1] /* writing back the modified MODER bits */

	/* //////SETUP VARIABLES////// */
	ldr r0, =0xD /*seed*/

	/* /////RUNTIME CODE/////// */

	push {r0}

idle_loop:

	bl get_button
	cmp r2, r3
	beq random_generation

	ldr r6, =0x0010 /* print the 2037 on the board */
	bl set_D
	bl set_2
	bl set_none
	ldr r6, =0x0020
	bl set_D
	bl set_0
	bl set_none
	ldr r6, =0x0040
	bl set_D
	bl set_3
	bl set_none
	ldr r6, =0x0080
	bl set_D
	bl set_7
	bl set_none

	b idle_loop

zero_delay:

	bl set_led_on

	ldr r7,=0x12AAA /* cycles for total delay */
    bl second_delay
    b idle_loop

random_generation:

	pop {r0}
	bl generate_random
	push {r0}
	b countdown

countdown:

    ldr r7,=0x26 /* cycles for total delay */
	bl second_delay

	bl set_led_off

	bl get_button
	cmp r2, r3
	beq countdown_halt_loop

	subs r0, #0x1
	cmp r0, #0x0
	bne countdown
	b zero_delay

countdown_halt_loop:

	bl number_extraction
    ldr r7,=0x26 /* cycles for total delay */
	bl second_delay

	bl get_button
	cmp r2, r3
	beq countdown

	b countdown_halt_loop

/* //////////////////////SET FUNCTIONS/////////////////////////////// */

get_button:

	push {lr}
	ldr r1, =0x50000010 /* checking if the button is pressed */
	ldr r2, [r1]

	ldr r3, =0x1
	ands r2, r2, r3
	pop {pc}

set_D:

	push {lr}
	ldr r7, =0x50000014 /* sets the D1 pin high */

	str r6, [r7]
	pop {pc}

/* //////////////////////NUMBER FUNCTIONS//////////////////////////// */

set_base:

    push {lr}
	ldr r7, =0x50000414 /* sets the A,B,C...G pins to show the number */
	ldr r5, [r7]
	ldr r6, =0xFF80
	ands r5, r5, r6
    pop {pc}

set_led_off:

	push {lr}
	bl set_base
	ldr r6, =0xFFFFFF7F
	ands r5, r5, r6
	str r5, [r7]
	pop {pc}

set_led_on:

	push {lr}
	bl set_base
	ldr r6, =0x80
	orrs r5, r5, r6
	str r5, [r7]
	pop {pc}

set_none:

	push {lr}
	bl set_base
	ldr r6, =0x7F
	orrs r5, r5, r6
	str r5, [r7]
	pop {pc}

set_0:

	push {lr}
	bl set_base
	ldr r6, =0x40
	orrs r5, r5, r6
	str r5, [r7]
	pop {pc}

set_1:

	push {lr}
	bl set_base
	ldr r6, =0x79
	orrs r5, r5, r6
	str r5, [r7]
	pop {pc}

set_2:

	push {lr}
	bl set_base
	ldr r6, =0x24
	orrs r5, r5, r6
	str r5, [r7]
	pop {pc}

set_3:

	push {lr}
	bl set_base
	ldr r6, =0x30
	orrs r5, r5, r6
	str r5, [r7]
	pop {pc}

set_4:

	push {lr}
	bl set_base
	ldr r6, =0x19
	orrs r5, r5, r6
	str r5, [r7]
	pop {pc}

set_5:

	push {lr}
	bl set_base
	ldr r6, =0x12
	orrs r5, r5, r6
	str r5, [r7]
	pop {pc}

set_6:

	push {lr}
	bl set_base
	ldr r6, =0x02
	orrs r5, r5, r6
	str r5, [r7]
	pop {pc}

set_7:

	push {lr}
	bl set_base
	ldr r6, =0x78
	orrs r5, r5, r6
	str r5, [r7]
	pop {pc}

set_8:

	push {lr}
	bl set_base
	ldr r6, =0x00
	orrs r5, r5, r6
	str r5, [r7]
	pop {pc}

set_9:

	push {lr}
	bl set_base
	ldr r6, =0x10
	orrs r5, r5, r6
	str r5, [r7]
	pop {pc}

/* /////////////////////////FUNCTIONS CONTINUED/////////////////////////////// */

generate_random:

	push {lr}

	ldr r1, =0x41A7 /* a */
	ldr r2, =0x3039 /* c */
	ldr r3, =0x7FFF /* m */

	muls r0, r1, r0 /* seed = ((seed * a) + c) % m */
	adds r0, r0, r2
	movs r4, r3
	bl modulus

	ldr r4, =0x2327 /* seed = seed % 8999 */
	bl modulus

	ldr r5, =0x3E8 /* temporary shift to 1000 - 9999 constant */
	adds r0, r0, r5
	pop {pc}

/* ///////////////////////////////////////////////////////////////// */

modulus: /* uses r0 and r4 for computation of r0 % r4 */

	push {lr}

modulus_loop:

	cmp r0, r4
	blt pop_pc
	subs r0, r0, r4 /* subtract r0 from r4*/
	/* compare if the r0 value is lower than r4 if so jump out of division */
	bge modulus_loop /* jump back*/
    pop {pc} /*

/* ///////////////////////////////////////////////////////////////// */

number_extraction:

	push {lr} /* pushes lr to return back with pop {pc} */
	push {r0} /* pushes the seed into the stack to preserve its value */
	movs r1, #0x0
	movs r2, #0x0
	movs r3, #0x0
	movs r4, #0x0 /* resetting the registers for counting */

	ldr r5, =0x3E8 /* dec: 1000 */

thousands_loop:

	cmp r0, r5 /* removing 1000s until the number is smaller than thousand and counting it to r1 */
	blt hundreds_loop
	subs r0, r0, r5
	adds r1, #0x1
	b thousands_loop

hundreds_loop:

	cmp r0, #0x64 /* removing 100s until the number is smaller than thousand and counting it to r1 */
	blt tens_loop
	subs r0, #0x64
	adds r2, #0x1
	b hundreds_loop

tens_loop:

	cmp r0, #0xA /* removing 10s until the number is smaller than thousand and counting it to r1 */
	blt ones_loop
	subs r0, #0xA
	adds r3, #0x1
	b tens_loop

ones_loop:

	movs r4, r0 /* writing the left value to the ones digit register */
	pop {r0}
	b pop_pc

/* ///////////////////////////////////////////////////////////////// */

second_delay:

	push {lr}
	push {r7}

second_delay_loop:

	bl number_extraction
	bl number_picker

	pop {r7}
	cmp r7, #0x0
	beq pop_pc
	subs r7, r7, #1
	push {r7}
	b second_delay_loop

/* ///////////////////////////////////////////////////////////////// */

pop_pc:

	pop {pc}

/* ///////////////////////////////////////////////////////////////// */

number_picker:

	push {lr} /* pushes lr so that we can use it as a subfunction */

thousands_picker:

	bl set_none /* clears the written number */
	ldr r6, =0x0010
	bl set_D /* sets the digit to the first one */
	cmp r1, #0x0 /* check the thousands digit r1 and runs the according write function */
	beq run_thousands_0
	cmp r1, #0x1
	beq run_thousands_1
	cmp r1, #0x2
	beq run_thousands_2
	cmp r1, #0x3
	beq run_thousands_3
	cmp r1, #0x4
	beq run_thousands_4
	cmp r1, #0x5
	beq run_thousands_5
	cmp r1, #0x6
	beq run_thousands_6
	cmp r1, #0x7
	beq run_thousands_7
	cmp r1, #0x8
	beq run_thousands_8
	cmp r1, #0x9
	beq run_thousands_9

run_thousands_0:

	bl set_0
	b hundreds_picker

run_thousands_1:

	bl set_1
	b hundreds_picker

run_thousands_2:

	bl set_2
	b hundreds_picker

run_thousands_3:

	bl set_3
	b hundreds_picker

run_thousands_4:

	bl set_4
	b hundreds_picker

run_thousands_5:

	bl set_5
	b hundreds_picker

run_thousands_6:

	bl set_6
	b hundreds_picker

run_thousands_7:

	bl set_7
	b hundreds_picker

run_thousands_8:

	bl set_8
	b hundreds_picker

run_thousands_9:

	bl set_9
	b hundreds_picker

hundreds_picker:

	bl set_none /* clears the written number */
	ldr r6, =0x0020
	bl set_D /* sets the digit to the second one */
	cmp r2, #0x0 /* check the hundreds digit r2 and runs the according write function */
	beq run_hundreds_0
	cmp r2, #0x1
	beq run_hundreds_1
	cmp r2, #0x2
	beq run_hundreds_2
	cmp r2, #0x3
	beq run_hundreds_3
	cmp r2, #0x4
	beq run_hundreds_4
	cmp r2, #0x5
	beq run_hundreds_5
	cmp r2, #0x6
	beq run_hundreds_6
	cmp r2, #0x7
	beq run_hundreds_7
	cmp r2, #0x8
	beq run_hundreds_8
	cmp r2, #0x9
	beq run_hundreds_9

run_hundreds_0:

	bl set_0
	b tens_picker

run_hundreds_1:

	bl set_1
	b tens_picker

run_hundreds_2:

	bl set_2
	b tens_picker

run_hundreds_3:

	bl set_3
	b tens_picker

run_hundreds_4:

	bl set_4
	b tens_picker

run_hundreds_5:

	bl set_5
	b tens_picker

run_hundreds_6:

	bl set_6
	b tens_picker

run_hundreds_7:

	bl set_7
	b tens_picker

run_hundreds_8:

	bl set_8
	b tens_picker

run_hundreds_9:

	bl set_9
	b tens_picker

tens_picker:

	bl set_none /* clears the written number */
	ldr r6, =0x0040
	bl set_D /* sets the digit to the third one */
	cmp r3, #0x0 /* check the tens digit r3 and runs the according write function */
	beq run_tens_0
	cmp r3, #0x1
	beq run_tens_1
	cmp r3, #0x2
	beq run_tens_2
	cmp r3, #0x3
	beq run_tens_3
	cmp r3, #0x4
	beq run_tens_4
	cmp r3, #0x5
	beq run_tens_5
	cmp r3, #0x6
	beq run_tens_6
	cmp r3, #0x7
	beq run_tens_7
	cmp r3, #0x8
	beq run_tens_8
	cmp r3, #0x9
	beq run_tens_9

run_tens_0:

	bl set_0
	b ones_picker

run_tens_1:

	bl set_1
	b ones_picker

run_tens_2:

	bl set_2
	b ones_picker

run_tens_3:

	bl set_3
	b ones_picker

run_tens_4:

	bl set_4
	b ones_picker

run_tens_5:

	bl set_5
	b ones_picker

run_tens_6:

	bl set_6
	b ones_picker

run_tens_7:

	bl set_7
	b ones_picker

run_tens_8:

	bl set_8
	b ones_picker

run_tens_9:

	bl set_9
	b ones_picker

ones_picker:

	bl set_none /* clears the written number */
	ldr r6, =0x0080
	bl set_D /* sets the digit to the fourth one */
	cmp r4, #0x1 /* check the tens digit r3 and runs the according write function */
	beq run_ones_1
	cmp r4, #0x2
	beq run_ones_2
	cmp r4, #0x3
	beq run_ones_3
	cmp r4, #0x4
	beq run_ones_4
	cmp r4, #0x5
	beq run_ones_5
	cmp r4, #0x6
	beq run_ones_6
	cmp r4, #0x7
	beq run_ones_7
	cmp r4, #0x8
	beq run_ones_8
	cmp r4, #0x9
	beq run_ones_9

run_ones_0:

	bl set_0
	b pop_pc

run_ones_1:

	bl set_1
	b pop_pc

run_ones_2:

	bl set_2
	b pop_pc

run_ones_3:

	bl set_3
	b pop_pc

run_ones_4:

	bl set_4
	b pop_pc

run_ones_5:

	bl set_5
	b pop_pc

run_ones_6:

	bl set_6
	b pop_pc

run_ones_7:

	bl set_7
	b pop_pc

run_ones_8:

	bl set_8
	b pop_pc

run_ones_9:

	bl set_9
	b pop_pc

/* ///////////////////////////////////////////////////////////////// */

	/* for(/*)/* */
	b .

	/* this should never get executed */
	nop
