#include "stm32g0xx.h"
#include "stdlib.h"

volatile uint32_t CurrentState = 0; /* 0 is idle, 1 is counting */
volatile uint32_t Counter = 0; /* counter value */

/* set seven segment functions */

void SetClear(void)
{
	GPIOB->ODR |= (0x7FU);
}

void Set(volatile uint32_t digit)
{
	switch(digit)
	{
		case 0:
			GPIOB->ODR &= (0x40U);
			break;
		case 1:
			GPIOB->ODR &= (0x79U);
			break;
		case 2:
			GPIOB->ODR &= (0x24U);
			break;
		case 3:
			GPIOB->ODR &= (0x30U);
			break;
		case 4:
			GPIOB->ODR &= (0x19U);
			break;
		case 5:
			GPIOB->ODR &= (0x12U);
			break;
		case 6:
			GPIOB->ODR &= (0x02U);
			break;
		case 7:
			GPIOB->ODR &= (0x78U);
			break;
		case 8:
			GPIOB->ODR &= (0x00U);
			break;
		case 9:
			GPIOB->ODR &= (0x10U);
			break;
	}
}

/* set digit function */

void SetDClear(void)
{
	GPIOA->ODR &= ~(15U << 4); /* digit clear */
}

void SetD(volatile uint32_t digit) /* 0 = Digit1 */
{
	SetDClear();
	SetClear();
	GPIOA->ODR |= (1U << (4 + digit));
}

/* print and refresh functions */

void print(void)
{
	uint32_t* digits = (uint32_t*) calloc(sizeof(uint32_t), 4); /* array to store digits of counter */
	uint32_t counter = Counter; /* copy of the counter */

		*(digits+0) = (counter/1000); /* thousands digit extraction */
		counter -= (*(digits+0) * 1000);
		*(digits+1) = (counter/100); /* hundreds digit extraction */
		counter -= (*(digits+1) * 100);
		*(digits+2) = (counter/10); /* tens digit extraction */
		counter -= (*(digits+2) * 10);
		*(digits+3) = counter; /* ones digit extraction */

	for(uint32_t i=0; i<4; i++)
	{
		SetD(i);
		Set(*(digits+i));
	}
	SetDClear();
	free(digits);
}

/* handler functions */

void EXTI0_1_IRQHandler(void)
{
	EXTI->RPR1 |= (1U << 1); /* clear pending on A1 */
	CurrentState = 1; /* counting state */
	GPIOC->ODR &= ~(1U << 6); /* turn off on-board LED */
}

void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
    TIM1->SR &= ~(1U << 0); /* reset status register */

    if(CurrentState) /* counting */
    {
    	if(Counter == 10000)
    	{
    		CurrentState = 0; /* switch to idle */
    		Counter = 0;
    		GPIOC->ODR |= (1U << 6); /* turn on on-board LED */
    	}
    	else
    	{
    		for(int i=0; i<14; i++)
    		{
    			print(); /* print the global counter */
    		}
    		Counter++;
    	}
    }
    else
    {
    	/* idle state */
    	SetClear();
    }
}

/* main function */

int main(void) {

	/* setting up the seven segment pins and on board LED */

	GPIOB->MODER |= (0x5555U); /* Setup (B0, B7) as output */
	GPIOB->MODER &= ~(0xAAAAU); /* Outputs for seven segments */

	GPIOA->MODER |= (0x55U << 2*4); /* Setup (A4, A7) as output */
	GPIOA->MODER &= ~(0xAAU << 2*4); /* Outputs for digit selections */

	GPIOC->MODER &= ~(2U << 2*6); /* Setup PC6 as output */
	GPIOC->MODER |= (1U << 2*6);

    /* setting up the button input and interrupt */
	__set_PRIMASK(0U); /* set PRIMASK to 0 */

	RCC->IOPENR |= (7U << 0); /* Enable GPIOA,B,C clocks */

	GPIOA->MODER &= ~(3U << 2*1); /* Setup PA1 as input */

	EXTI->RTSR1 |= (1U << 1); /* Rising edge selection A1 */
	EXTI->EXTICR[0] &= ~(1U << 8*1); /* 1U to select A1 from mux */
	EXTI->IMR1 |= (1U << 1); /* interrupt mask register A1 */

	EXTI->RPR1 |= (1U << 1); /* clear pending on A1 */

	NVIC_SetPriority(EXTI0_1_IRQn, 0x0U); /* Setting priority for EXTI0_1 */
	NVIC_EnableIRQ(EXTI0_1_IRQn); /* Enabling EXTI0_1 */

    /* setting up the timer and interrupt */

    RCC->APBENR2 |= (1U << 11); /* Enable TIM1 clock */

    TIM1->CR1 = 0; /* resetting control register */
    TIM1->CR1 |= (1U << 7); /* ARPE buffering */
    TIM1->CNT = 0; /* reset the timer counter */

    TIM1->PSC = 7999; /* prescaler set to 16000-1 */
    TIM1->ARR = 1; /* set the autoreload register for 1 milliseconds */

    TIM1->DIER |= (1U << 0); /* update interrupt enable */
    TIM1->CR1 |= (1U << 0); /* Enable TIM1 */

    NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 0x3U); /* Setting priority for TIM1 */
    NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn); /* Enabling TIM1 */

    while(1)
    {

    }

    return 0;
}
