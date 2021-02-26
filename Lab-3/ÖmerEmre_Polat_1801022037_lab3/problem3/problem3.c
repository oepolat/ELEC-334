#include "stm32g0xx.h"

#define sec_delay 1600000

void delay(volatile uint32_t time)
{
    for(; time>0; time--);
}

void EXTI0_1_IRQHandler(void)
{
	EXTI->RPR1 |= (1U << 1); /* clear pending on B1 */
	GPIOB->ODR |= (1U << 0); /* turn on LED at B0 */
	delay(sec_delay * 5); /* delay for around 5 seconds */
	GPIOB->ODR &=  ~(1U << 0); /* turn off LED at B0 */
}

void EXTI2_3_IRQHandler(void)
{
	EXTI->RPR1 |= (1U << 2); /* clear pending on B2 */
	GPIOB->ODR |= (1U << 3); /* turn on LED at B3 */
	delay(sec_delay * 5); /* delay for around 5 seconds */
	GPIOB->ODR &=  ~(1U << 3); /* turn off LED at B3 */
}

int main(void) {

	__set_PRIMASK(0U); /* set PRIMASK to 0 */

	RCC->IOPENR |= (1U << 1); /* Enable GPIOB clock */

    GPIOB->MODER &= ~(15U << 2*1); /* Setup PB1 and PB2 as input */

    GPIOB->MODER &= ~(0xC3U << 2*0); /* Setup PB0 and PB3 as output */
    GPIOB->MODER |= (0x41U << 2*0);

    EXTI->RTSR1 |= (3U << 1); /* Rising edge selection B1 and B2 */
    EXTI->EXTICR[0] |= (1U << 8*1); /* 1U to select B1 from mux */
    EXTI->EXTICR[0] |= (1U << 8*2); /* 1U to select B2 from mux */
    EXTI->IMR1 |= (3U << 1); /* interrupt mask register B1 and B2 */

    EXTI->RPR1 |= (1U << 1); /* clear pending on B1 */
    EXTI->RPR1 |= (1U << 2); /* clear pending on B2 */

    NVIC_SetPriority(EXTI0_1_IRQn, 0x0U); /* Setting priority for EXTI0_1 */
    NVIC_EnableIRQ(EXTI0_1_IRQn); /* Enabling EXTI0_1 */

    NVIC_SetPriority(EXTI2_3_IRQn, 0x2U); /* Setting priority for EXTI2_3 */
    NVIC_EnableIRQ(EXTI2_3_IRQn); /* Enabling EXTI2_3 */

    while(1)
    {

    }
    return 0;
}
