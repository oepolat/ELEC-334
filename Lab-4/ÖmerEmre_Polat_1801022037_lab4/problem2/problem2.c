#include "stm32g0xx.h"

volatile uint32_t DelayTime = 1000;  /* 1000 ms delay time */

void EXTI0_1_IRQHandler(void)
{
	EXTI->RPR1 |= (1U << 1); /* clear pending on B1 */

    if(DelayTime != 10000)
    {
        DelayTime += 1000; /* increment the delay time by 1000 ms */
    }
    else
    {
        DelayTime = 1000; /* reset back the delay time */
    }

    TIM1->ARR = DelayTime; /* update autoreload time */
}

void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
    TIM1->SR &= ~(1U << 0); /* reset status register */
    GPIOC->ODR ^= (1U << 6); /* toggle LED */
}

int main(void) {

    /* setting up the button input LED output and interrupt */
	__set_PRIMASK(0U); /* set PRIMASK to 0 */

	RCC->IOPENR |= (1U << 1); /* Enable GPIOB clock */
	RCC->IOPENR |= (1U << 2); /* Enable GPIOC clock */

	GPIOB->MODER &= ~(3U << 2*1); /* Setup PB1 as input */

	GPIOC->MODER &= ~(2U << 2*6); /* Setup PC6 as output */
	GPIOC->MODER |= (1U << 2*6);

	EXTI->RTSR1 |= (1U << 1); /* Rising edge selection B1 */
	EXTI->EXTICR[0] |= (1U << 8*1); /* 1U to select B1 from mux */
	EXTI->IMR1 |= (1U << 1); /* interrupt mask register B1 */

	EXTI->RPR1 |= (1U << 1); /* clear pending on B1 */

	NVIC_SetPriority(EXTI0_1_IRQn, 0x0U); /* Setting priority for EXTI0_1 */
	NVIC_EnableIRQ(EXTI0_1_IRQn); /* Enabling EXTI0_1 */

    /* setting up the timer and interrupt */

    RCC->APBENR2 |= (1U << 11); /* Enable TIM1 clock */

    TIM1->CR1 = 0; /* resetting control register */
    TIM1->CR1 |= (1U << 7); /* Autoreload preload enable */
    TIM1->CNT = 0; /* reset the timer counter */

    TIM1->PSC = 15999; /* prescaler 16000 so that we count ever milliseconds */
    TIM1->ARR = DelayTime; /* set the autoreload register */

    TIM1->DIER |= (1U << 0); /* update interrupt enable */
    TIM1->CR1 |= (1U << 0); /* Enable TIM1 */

    NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 0x3U); /* Setting priority for TIM1 */
    NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn); /* Enabling TIM1 */

    while(1)
    {

    }

    return 0;
}
