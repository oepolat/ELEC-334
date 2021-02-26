/* BSP.C                   */
/* Author: Ömer Emre Polat */
/* Student No: 1801022037  */

#include "bsp.h"
#include <math.h>

/*///////////////BSP FUNCTIONS///////////////*/

void BSP_IWDG_init(void)
{
    IWDG->KR = 0x5555;
    IWDG->PR = 1; // prescaler
    while(IWDG->SR & 0x1); // wait while status update
    IWDG->KR = 0xCCCC;
}

void BSP_IWDG_refresh(void)
{
    IWDG->KR = 0xAAAA;
}

void BSP_system_init()
{
    __disable_irq();

    RCC->APBENR2 = 0x0U;

    /* input and output pins setup */

    RCC->IOPENR |= (3U << 0); /* enable A and B clock */

	/* pwm timer 16 AF2 setup */

	GPIOB->MODER &= ~(3U << 2*8); /* PB8 as AF mode */
	GPIOB->MODER |= (2U << 2*8);

	GPIOB->AFR[1] &= ~(0xFU << 4*0); /* Choose AF2 from mux */
	GPIOB->AFR[1] |= (0x2U << 4*0);

	RCC->APBENR2 |= (1U << 17); /* Enable TIM16 clock */

	TIM16->CR1 = 0U; /* resetting control register */
	TIM16->CR1 |= (1U << 7); /* ARPE buffering */
	TIM16->CNT = 0U; /* reset the timer counter */

	TIM16->PSC = ((SystemCoreClock/RefreshRate) - 1); /* prescaler set to 1600-1 */
	TIM16->ARR = 1U; /* set the autoreload register for 1 milliseconds */

	TIM16->CR1 |= (1U << 0); /* Enable TIM16 */

	TIM16->CCMR1 |= (6U << 4);

	TIM16->CCMR1 |= (1U << 3);

	TIM16->CCER |= (1U << 0);

	TIM16->CCR1 |= (1U << 0);

	TIM16->BDTR |= (1U << 15); /* main output enable */

    /* pwm timer 17 AF2 setup */

	GPIOB->MODER &= ~(3U << 2*9); /* PB9 as AF mode */
	GPIOB->MODER |= (2U << 2*9);

	GPIOB->AFR[1] &= ~(0xFU << 4*1); /* Choose AF2 from mux */
	GPIOB->AFR[1] |= (0x2U << 4*1);

	RCC->APBENR2 |= (1U << 18); /* Enable TIM17 clock */

	TIM17->CR1 = 0U; /* resetting control register */
	TIM17->CR1 |= (1U << 7); /* ARPE buffering */
	TIM17->CNT = 0U; /* reset the timer counter */

	TIM17->PSC = ((SystemCoreClock/RefreshRate) - 1); /* prescaler set to 1600-1 */
	TIM17->ARR = 1U; /* set the autoreload register for 1 milliseconds */

	TIM17->CR1 |= (1U << 0); /* Enable TIM17 */

	TIM17->CCMR1 |= (6U << 4);

	TIM17->CCMR1 |= (1U << 3);

	TIM17->CCER |= (1U << 0);

	TIM17->CCR1 |= (1U << 0);

	TIM17->BDTR |= (1U << 15);

    /* ADC setup */

	RCC->APBENR2 |= (1U << 20); /* ADC clock enable */

	RCC->APBRSTR2 |= (1U << 20);
	RCC->APBRSTR2 &= ~(1U << 20);

	ADC1->CFGR1 = 0x0U;

	ADC1->CR |= (1U << 28); /* open voltage reg */
	for(int i = 0; i<16000; i++);

	ADC1->CR |= (1U << 31); /* ADC calibration */
	while(!(ADC1->CR >> 31) & (0x1U)); /* wait for end of calibration */

	ADC1->CFGR1 |= (2U << 3); /* 8 bit resolution */

	ADC1->CFGR1 |= (1U << 13); /* continuous conversion enable */
	ADC1->CFGR1 &= ~(1U << 16); /* discontinuous mode disable */

	ADC1->SMPR &= ~(1U << 0);

	ADC1->CFGR1 &= ~(7U << 6); /* trigger selection trg0 */
	ADC1->CFGR1 &= ~(3U << 10); /* trigger as continuos mode */

	ADC1->CFGR1 |= (9U << 26); /* PA9 channel selection */
	ADC1->CHSELR |= (1U << 9);

	GPIOA->MODER |= (3U << 9*2);

	ADC1->ISR |= (1 << 0); /* reset ready status */

	ADC1->CR |= (1 << 0); /* adc enable */
	while( !((ADC1->ISR) & (0x1U)));

	ADC1->CR |= (1U << 2);

	//BSP_IWDG_init(); // Watchdog init

    __enable_irq();
}
