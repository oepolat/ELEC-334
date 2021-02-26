#include "stm32g0xx.h"

void HardFault_Handler(void)
{
	__ASM("LDR r0, =_estack"); /* resetting the stack pointer */
	__ASM("MOV SP, r0");
	__ASM("B Reset_Handler"); /* calling the reset handler */
}

void EXTI0_1_IRQHandler(void)
{
	HardFault_Handler();
	EXTI->RPR1 |= (1U << 1); /* clear pending on B1 */
}

int main(void) {

	__set_PRIMASK(0U); /* set PRIMASK to 0 */

    RCC->IOPENR |= (1U << 1); /* Enable GPIOB clock */

    GPIOB->MODER &= ~(3U << 2*1); /* Setup PB1 as input */

    EXTI->RTSR1 |= (1U << 1); /* Rising edge selection */
    EXTI->EXTICR[0] |= (1U << 8*1); /* 1U to select B from mux */
    EXTI->IMR1 |= (1U << 1); /* interrupt mask register */

    NVIC_SetPriority(EXTI0_1_IRQn, 0); /* Setting priority for EXTI0_1*/
    NVIC_EnableIRQ(EXTI0_1_IRQn); /* Enabling EXTI0_1 */

    EXTI->RPR1 |= (1U << 1); /* clear pending on B1 */

    while(1)
    {

    }
    return 0;
}
