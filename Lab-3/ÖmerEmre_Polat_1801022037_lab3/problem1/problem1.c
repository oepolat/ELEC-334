#include "stm32g0xx.h"

void HardFault_Handler(void)
{
	__ASM("LDR r0, =_estack"); /* resetting the stack pointer */
	__ASM("MOV SP, r0");
	__ASM("B Reset_Handler"); /* calling the reset handler */
}

int main(void)
{
	RCC->IOPENR |= (1U << 1); /* Enable GPIOB clock */

	GPIOB->MODER &= ~(3U << 2*1); /* Setup PB1 as input */

    while(1)
    {
    	if( (GPIOB->IDR & (1U << 1)) >> 1 )
    	{
    		HardFault_Handler(); /* starting the HardFault Handler */
    	}
    }
    return 0;
}
