/* BSP.C                   */
/* Author: Ömer Emre Polat */
/* Student No: 1801022037  */

#include "bsp.h"
#include <math.h>

void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
	TIM1->SR &= ~(1U << 0); /* reset status register */
	ssd_print();
}

/* PA8-PA11 */
void EXTI4_15_IRQHandler(void)
{
	kp_get_input(); /* get the input and write it to global input */

	switch(kp_input)
	{
		case '1':
			TIM17->BDTR |= (1U << 15);
			number_1 = 440; /* A4 440Hz */
			break;
		case '2':
			TIM17->BDTR |= (1U << 15);
			number_1 = 494; /* B4 494Hz */
			break;
		case '3':
			TIM17->BDTR |= (1U << 15);
			number_1 = 523; /* C5 523Hz */
			break;
		case '4':
			TIM17->BDTR |= (1U << 15);
			number_1 = 587; /* D5 587Hz */
			break;
		case '5':
			TIM17->BDTR |= (1U << 15);
			number_1 = 659; /* E5 659Hz */
			break;
		case '6':
			TIM17->BDTR |= (1U << 15);
			number_1 = 698; /* F5 698Hz */
			break;
		case '7':
			TIM17->BDTR |= (1U << 15);
			number_1 = 784; /* G5 784Hz */
			break;
		case '8':
			TIM17->BDTR |= (1U << 15);
			number_1 = 880; /* A5 880Hz */
			break;
		case '9':
			TIM17->BDTR |= (1U << 15);
			number_1 = 987; /* B5 987Hz */
			break;
		case '0':
			TIM17->BDTR |= (1U << 15);
			number_1 = 1047; /* C6 1047Hz */
			break;
		case 'A':
			TIM17->BDTR |= (1U << 15);
			number_1 = 1175; /* D6 1175Hz */
			break;
		case 'B':
			TIM17->BDTR |= (1U << 15);
			number_1 = 1319; /* E6 1319Hz */
			break;
		case 'C':
			TIM17->BDTR |= (1U << 15);
			number_1 = 1397; /* F6 1397Hz */
			break;
		case 'D':
			TIM17->BDTR |= (1U << 15);
			number_1 = 1567; /* G6 1567Hz */
			break;
		case 'F':
			TIM17->BDTR |= (1U << 15);
			number_1 = 1760; /* A6 1760Hz */
			break;
		case 'E':
			TIM17->BDTR &= ~(1U << 15);
			break;
		default:

			break;
	}

	/* set the prescaler for the number_1 frequency */
	TIM17->PSC = ((SystemCoreClock / (long unsigned int) number_1) - 1);

	for(uint32_t i=0; i<333333; i++);

	EXTI->RPR1 |= (15U << 8); /* reset interrupt pending */
}

/*//////////////KEYPAD FUNCTIONS//////////////*/

void kp_get_input(void)
{
	uint8_t c = 0;
	uint8_t r = 0;

	static const char kp_inputs[] =
	{'1', '2', '3', 'A',
	 '4', '5', '6', 'B',
	 '7', '8', '9', 'C',
	 'E', '0', 'F', 'D'};

	if((EXTI->RPR1 >> 8) & (1U))
	{
		c = 0;
	}
	else if ((EXTI->RPR1 >> 9) & (1U))
	{
		c = 1;
	}
	else if ((EXTI->RPR1 >> 10) & (1U))
	{
		c = 2;
	}
	else if ((EXTI->RPR1 >> 11) & (1U))
	{
		c = 3;
	}

	GPIOA->ODR &= ~(1U << 0); /* shut off A0 */
	for(uint32_t i=0; i<ButtonBounceTime; i++);
	if(((GPIOA->IDR >> (8+c)) & (1U)) ^ 1U) /* check if input is not there */
	{
		r = 0;
	}
	GPIOA->ODR |= (1U << 0); /* turn on A0 */

	GPIOA->ODR &= ~(1U << 1); /* shut off A1 */
	for(uint32_t i=0; i<ButtonBounceTime; i++);
	if(((GPIOA->IDR >> (8+c)) & (1U)) ^ 1U) /* check if input is not there */
	{
		r = 1;
	}
	GPIOA->ODR |= (1U << 1); /* turn on A1 */

	GPIOA->ODR &= ~(1U << 12); /* shut off A12 */
	for(uint32_t i=0; i<ButtonBounceTime; i++);
	if(((GPIOA->IDR >> (8+c)) & (1U)) ^ 1U) /* check if input is not there */
	{
		r = 2;
	}
	GPIOA->ODR |= (1U << 12); /* turn on A12 */

	GPIOA->ODR &= ~(1U << 15); /* shut off A15 */
	for(uint32_t i=0; i<ButtonBounceTime; i++);
	if(((GPIOA->IDR >> (8+c)) & (1U)) ^ 1U) /* check if input is not there */
	{
		r = 3;
	}
	GPIOA->ODR |= (1U << 15); /* turn on A15 */

	kp_input = kp_inputs[(c + (4 * r))];
}

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

    /* input and output pins setup */

    GPIOB->MODER |= (0x5555U); /* Setup (B0, B7) as output */
    GPIOB->MODER &= ~(0xAAAAU); /* Outputs for seven segments */

    GPIOA->MODER |= (0x55U << 2*4); /* Setup (A4, A7) as output */
    GPIOA->MODER &= ~(0xAAU << 2*4); /* Outputs for digit selections */

    GPIOA->MODER &= ~(255U << 2*8); /* Setup (A8, A11) as input */

    GPIOA->PUPDR &= ~(3U << 2*8); /* A8 Pull down */
    GPIOA->PUPDR |= (2U << 2*8);

    GPIOA->PUPDR &= ~(3U << 2*9); /* A9 Pull down */
    GPIOA->PUPDR |= (2U << 2*9);

    GPIOA->PUPDR &= ~(3U << 2*10); /* A10 Pull down */
    GPIOA->PUPDR |= (2U << 2*10);

    GPIOA->PUPDR &= ~(3U << 2*11); /* A11 Pull down */
    GPIOA->PUPDR |= (2U << 2*11);

    GPIOA->MODER &= ~(3U << 2*0); /* A0 output */
    GPIOA->MODER |= (1U << 2*0);
    GPIOA->ODR |= (1U << 0); /* set A0 to high */

    GPIOA->MODER &= ~(3U << 2*1); /* A1 output */
    GPIOA->MODER |= (1U << 2*1);
    GPIOA->ODR |= (1U << 1); /* set A1 to high */

    GPIOA->MODER &= ~(3U << 2*12); /* A12 output */
    GPIOA->MODER |= (1U << 2*12);
    GPIOA->ODR |= (1U << 12); /* set A12 to high */

    GPIOA->MODER &= ~(3U << 2*15); /* A15 output */
    GPIOA->MODER |= (1U << 2*15);
    GPIOA->ODR |= (1U << 15); /* set A15 to high */

    /* timer and interrupt setup */

	RCC->APBENR2 |= (1U << 11); /* Enable TIM1 clock */

	TIM1->CR1 = 0; /* resetting control register */
	TIM1->CR1 |= (1U << 7); /* ARPE buffering */
	TIM1->CNT = 0; /* reset the timer counter */

	TIM1->PSC = ((SystemCoreClock/RefreshRate) - 1); /* prescaler set to 1600-1 */
	TIM1->ARR = 1; /* set the autoreload register for 1 milliseconds */

	TIM1->DIER |= (1U << 0); /* update interrupt enable */
	TIM1->CR1 |= (1U << 0); /* Enable TIM1 */

	NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 0U); /* Setting priority for TIM1 */
	NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn); /* Enabling TIM1 */

    /* pwm timer 17 AF2 setup */

    //PB9 as AF mode
	GPIOB->MODER &= ~(3U << 2*9);
	GPIOB->MODER |= (2U << 2*9);

	//Choose AF2 from mux
	GPIOB->AFR[1] &= ~(0xFU << 4*1);
	GPIOB->AFR[1] |= (0x2U << 4*1);

	RCC->APBENR2 |= (1U << 18); /* Enable TIM17 clock */

	TIM17->CR1 = 0U; /* resetting control register */
	TIM17->CR1 |= (1U << 7); /* ARPE buffering */
	TIM17->CNT = 0U; /* reset the timer counter */

	TIM17->PSC = ((SystemCoreClock/RefreshRate) - 1); /* prescaler set to 1600-1 */
	TIM17->ARR = 1U; /* set the autoreload register for 1 milliseconds */

	TIM17->CR1 |= (1U << 0); /* Enable TIM1 */

	TIM17->CCMR1 |= (6U << 4);

	TIM17->CCMR1 |= (1U << 3);

	TIM17->CCER |= (1U << 0);

	TIM17->CCR1 |= (1U << 0);

    /* interrupt setup */

    RCC->IOPENR |= (3U << 0);

    EXTI->RTSR1 |= (15U << 8); /* Rising edge selection */
    EXTI->EXTICR[2] &= ~(1U << 8*0); /* 1U to select A8 from mux */
    EXTI->EXTICR[2] &= ~(1U << 8*1); /* 1U to select A9 from mux */
    EXTI->EXTICR[2] &= ~(1U << 8*2); /* 1U to select A10 from mux */
    EXTI->EXTICR[2] &= ~(1U << 8*3); /* 1U to select A11 from mux */
    EXTI->IMR1 |= (15U << 8); /* interrupt mask register */

    NVIC_SetPriority(EXTI4_15_IRQn, 1U); /* Setting priority for EXTI0_1 */
    NVIC_EnableIRQ(EXTI4_15_IRQn); /* Enabling EXTI0_1 */

    //BSP_IWDG_init(); // Watchdog init

    TIM1->SR  =  0x00000000U; /* reset status register */

    EXTI->RPR1 = 0x00000000U; /* reset interrupt pending */


    __enable_irq();
}

/*///////////////SSD FUNCTIONS///////////////*/

void ssd_set_clear(void)
{
	GPIOB->ODR |= (0x7FU);
}

void ssd_set(volatile uint32_t digit)
{
	switch(digit)
	{
		case 0:
			GPIOB->ODR &= (0xC0U);
			break;
		case 1:
			GPIOB->ODR &= (0xF9U);
			break;
		case 2:
			GPIOB->ODR &= (0xA4U);
			break;
		case 3:
			GPIOB->ODR &= (0xB0U);
			break;
		case 4:
			GPIOB->ODR &= (0x99U);
			break;
		case 5:
			GPIOB->ODR &= (0x92U);
			break;
		case 6:
			GPIOB->ODR &= (0x82U);
			break;
		case 7:
			GPIOB->ODR &= (0xF8U);
			break;
		case 8:
			GPIOB->ODR &= (0x80U);
			break;
		case 9:
			GPIOB->ODR &= (0x90U);
			break;
	}
}

void ssd_set_overflow(void)
{
	ssd_digit(0);
	GPIOB->ODR &= (0xC0U); /* O */
	ssd_digit(1);
	GPIOB->ODR &= (0xE3U); /* v */
	ssd_digit(2);
	GPIOB->ODR &= (0x8EU); /* F */
	ssd_digit(3);
	GPIOB->ODR &= (0xC7U); /* L */
	ssd_digit_clear();
}

void ssd_digit_clear(void)
{
	GPIOA->ODR &= ~(15U << 4); /* digit clear */
}

void ssd_digit(volatile uint32_t digit) /* 0 = Digit1 */
{
	ssd_digit_clear();
	ssd_set_clear();

	if((uint32_t) dp_pos == (digit + 1))
	{
		GPIOB->ODR &= ~(0x80U);
	}
	else
	{
		GPIOB->ODR |= (0x80U);
	}

	GPIOA->ODR |= (1U << (4 + digit));
}

/* print and refresh functions */

void ssd_print(void)
{
	uint32_t digits[4] = {0};
	int number; /* copy of the number */

	if((number_1 < -999))
	{
		dp_pos = none;
		ssd_set_overflow();
		return;
	}
	if((number_1 > 9999))
	{
		dp_pos = none;
		ssd_set_overflow();
		return;
	}

	if(number_1 < 0) /* negative case */
	{
		if((-1 * number_1) < 10) /* float printing */
		{
			dp_pos = second;
			number = (int) (100 * number_1);
		}
		else if((-1 * number_1) < 100)
		{
			dp_pos = third;
			number = (int) (10 * number_1);
		}
		else if((-1 * number_1) < 1000)
		{
			dp_pos = fourth;
			number = (int) number_1;
		}
		else
		{
			dp_pos = none;
		}
	}
	else /* positive case */
	{
		if(number_1 < 10) /* float printing */
		{
			dp_pos = first;
			number = (int) (1000 * number_1);
		}
		else if(number_1 < 100)
		{
			dp_pos = second;
			number = (int) (100 * number_1);
		}
		else if(number_1 < 1000)
		{
			dp_pos = third;
			number = (int) (10 * number_1);
		}
		else
		{
			dp_pos = none;
			number = (int) number_1;
		}
	}

	if(number < 0) /* negative printing */
	{
		number = -1 * number;

		*(digits+1) = (uint32_t) (number/100); /* hundreds digit extraction */
		number -= (int) (*(digits+1) * 100);
		*(digits+2) = (uint32_t) (number/10); /* tens digit extraction */
		number -= (int) (*(digits+2) * 10);
		*(digits+3) = (uint32_t) number; /* ones digit extraction */

		for(uint32_t i=0; i<4; i++)
		{
			ssd_digit(i);
			if(i == 0) /* print - sign */
			{
				GPIOB->ODR &= (0xBFU);
			}
			else
			{
				ssd_set(digits[i]);
			}
		}
		ssd_digit_clear();
	}
	else /* positive printing */
	{
		*(digits+0) = (uint32_t) (number/1000); /* thousands digit extraction */
		number -= (int) (*(digits+0) * 1000);
		*(digits+1) = (uint32_t) (number/100); /* hundreds digit extraction */
		number -= (int) (*(digits+1) * 100);
		*(digits+2) = (uint32_t) (number/10); /* tens digit extraction */
		number -= (int) (*(digits+2) * 10);
		*(digits+3) = (uint32_t) number; /* ones digit extraction */

		for(uint32_t i=0; i<4; i++)
		{
			ssd_digit(i);
			ssd_set(digits[i]);
		}
		ssd_digit_clear();
	}
}
