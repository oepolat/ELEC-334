/* BSP.C                   */
/* Author: Ömer Emre Polat */
/* Student No: 1801022037  */

#include "bsp.h"

void ADC_COMP_IRQHandler(void)
{
	uint8_t analog_data = (uint8_t) (ADC1->DR);

	switch(state)
	{
		case play: /* play data using the ADC data */
			if((track_index + 1) < MaxTrackIndex)
			{
				TIM17->CCR1 = track_1[track_index]; /* set the new duty cycle for PWM */
				track_index++;
			}
			else
			{
				/* do nothing */
			}
			break;
		case record: /* record data using the ADC data */
			if((track_index + 1) < MaxTrackIndex)
			{
				track_1[track_index] = analog_data;
				track_index++;
			}
			else
			{
				/* do nothing */
			}
			break;
		default:
			/* do nothing */
			break;
	}

	if(((state == record)) && ((((ms_ticks + 1) / 1000) + 1) > 5))
	{
		state = idle; /* switch back to idle is more than 5 seconds passed */
		track_index = 0;
		recording_state = recording_done;
		track_is_full[track_selection] = 1; /* mark the track as full */
	}

	if(((state == play)) && ((((ms_ticks + 1) / 1000) + 1) > 5))
	{
		state = idle; /* switch back to idle is more than 5 seconds passed */
		track_index = 0;
		playing_state = playing_done;
		TIM17->BDTR &= ~(1U << 15); /* main output disable for pwm */
	}

	ADC1->ISR |= (1U << 3); /* reset pending */
}

void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
	TIM1->SR &= ~(1U << 0); /* reset status register */

	if(ms_ticks >= 10000) /* 10 seconds */
	{
		state = idle; /* go back to idle */
		clock_ticks = 0; /* reset clock ticks */
		ms_ticks = 0; /* reset millisecond ticks */
	}
	else
	{
		clock_ticks += 3200;
		ms_ticks = (clock_ticks + 1) / (16000);
	}

	ssd_print();
}

/* PA8-PA11 */
void EXTI4_15_IRQHandler(void)
{
	kp_get_input(); /* get the input and write it to global input */

	clock_ticks = 0; /* reset clock ticks */
	ms_ticks = 0; /* reset millisecond ticks */

	switch(state) /* state transitions */
	{
		case start: /* start state */
			switch(kp_input)
			{
				case 'x':
					/* do nothing */
					break;
				case '6':
					state = show_status;
					break;
				default:
					state = idle;
					break;
			}
			break;
		case idle: /* idle state */
			switch(kp_input)
			{
				case '1':
					state = track_selected;
					track_selection = track_one;
					break;
				case '2':
					state = track_selected;
					track_selection = track_two;
					break;
				case '3':
					state = track_selected;
					track_selection = track_three;
					break;
				case '4':
					state = track_selected;
					track_selection = track_four;
					break;
				case '6':
					state = show_status;
					break;
				default:
					/* do nothing */
					break;
			}
			break;
		case track_selected:
			switch(kp_input)
			{
				case '5': /* delete */
					state = delete;
					track_selection = track_one;
					break;
				case '6': /* show status */
					state = show_status;
					break;
				case 'A': /* play/pause */
					if(track_is_full[track_selection])
					{
						TIM17->BDTR |= (1U << 15); /* main output enable for pwm */
						state = play;
						playing_state = playing;
						track_index = 0;
					}
					else
					{
						state = idle;
					}
					break;
				case 'B': /* record */
					if(track_is_full[track_selection])
					{
						state = full;
					}
					else
					{
						state = record;
						recording_state = recording;
						track_is_full[track_selection] = 1;
						empty_tracks--;
						track_index = 0;
					}
					break;
				default:
					/* do nothing */
					break;
			}
			break;
		case play:
			switch(kp_input)
			{
				case 'A': /* play/pause */
					state = pause;
					TIM17->BDTR &= ~(1U << 15); /* main output disable for pwm */
					break;
				default:
					/* do nothing */
					break;
			}
			if((track_index + 1) >= MaxTrackIndex)
			{
				track_index = 0;
				state = idle;
				TIM17->BDTR &= ~(1U << 15); /* main output disable for pwm */
				playing_state = playing_done;
			}
			break;
		case pause:
			switch(kp_input)
			{
				case 'A': /* play/pause */
					state = play;
					TIM17->BDTR |= (1U << 15); /* main output enable for pwm */
					break;
				default:
					/* do nothing */
					break;
			}
			break;
		case record:
			if((track_index + 1) >= MaxTrackIndex)
			{
				track_index = 0;
				state = idle;
				recording_state = recording_done;
			}
			break;
		case delete:

			break;
		case full:

			break;
		case show_status:

			break;
	}

	switch(state) /* performs operations depending on the switched state */
	{
		case start:
			/* do nothing */
			break;
		case idle:
			/* do nothing */
			break;
		case track_selected:

			break;
		case play:

			break;
		case pause:

			break;
		case record:

			break;
		case delete:
			track_is_full[track_selection] = 0;
			empty_tracks++;
			break;
		case full:
			/* do nothing */
			break;
		case show_status:

			break;
	}

	for(volatile uint32_t i=0; i<333333; i++);

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

	kp_input = kp_inputs[(c + (4 * r))]; /* constructing the input with the row and column values */
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

    GPIOB->MODER |= (0x1555U); /* Setup (B0, B6) as output */
    GPIOB->MODER &= ~(0x2AAAU); /* Outputs for seven segments */

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

//    GPIOA->MODER &= ~(3U << 2*12); /* A12 output */
//    GPIOA->MODER |= (1U << 2*12);
//    GPIOA->ODR |= (1U << 12); /* set A12 to high */
//
//    GPIOA->MODER &= ~(3U << 2*15); /* A15 output */
//    GPIOA->MODER |= (1U << 2*15);
//    GPIOA->ODR |= (1U << 15); /* set A15 to high */

    /* timer1 and interrupt setup */

	RCC->APBENR2 |= (1U << 11); /* Enable TIM1 clock */

	TIM1->CR1 = 0; /* resetting control register */
	TIM1->CNT = 0; /* reset the timer counter */

	TIM1->PSC = ((SystemCoreClock/RefreshRate) - 1); /* prescaler set to 1600-1 */
	TIM1->ARR = 1; /* set the autoreload register for 1 milliseconds */

	TIM1->DIER |= (1U << 0); /* update interrupt enable */
	TIM1->CR1 |= (1U << 0); /* Enable TIM1 */

	NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 0U); /* Setting priority for TIM1 */
	NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn); /* Enabling TIM1 */

	/* timer3 setup */

	RCC->APBENR1 |= (1U << 1); /* Enable TIM3 clock */

	TIM3->CR1 = 0; /* resetting control register */
	TIM3->CNT = 0; /* reset the timer counter */

	TIM3->PSC = ((SystemCoreClock/RefreshRate) - 1); /* prescaler set to 1600-1 */
	TIM3->ARR = 1; /* set the autoreload register for 1 milliseconds */

	TIM3->CR2 |= (2U << 4); /* master mode to update for trigger */

	TIM3->CR1 |= (1U << 0); /* Enable TIM3 */

	/* ADC setup */

	RCC->APBENR2 |= (1U << 20); /* ADC clock enable */

	RCC->APBRSTR2 |= (1U << 20); /* reset adc */
	RCC->APBRSTR2 &= ~(1U << 20);

	ADC1->CR |= (1U << 28); /* open voltage reg */
	for(volatile int i = 0; i<16000; i++);

	ADC1->CR |= (1U << 31); /* ADC calibration */
	while((ADC1->CR >> 31) & (0x1U)); /* wait for end of calibration */

	ADC1->CFGR1 |= (2U << 3); /* 8 bit resolution */

	ADC1->CFGR1 &= ~(1U << 13); /* single conversion mode */

	ADC1->SMPR &= ~(1U << 0);

	ADC1->CFGR1 &= ~(7U << 6); /* trigger selection trg3 */
	ADC1->CFGR1 |= (3U << 6);
	ADC1->CFGR1 |= (1U << 10); /* rising edge external trigger */

	ADC1->CHSELR |= (1U << 16); /* PA12 = ADC1/16 channel selection */

	GPIOA->MODER |= (3U << 2*12);

	ADC1->IER |= (1U << 3); /* end of conversion interrupt enable */

	ADC1->CR |= (1U << 0); /* adc enable */
	while(!((ADC1->ISR) & (0x1U)));

	NVIC_SetPriority(ADC1_IRQn, 1U); /* Setting priority for ADC handler */
	NVIC_EnableIRQ(ADC1_IRQn); /* Enabling ADC handler */

	ADC1->CR |= (1U << 2); /* start conversion */

    /* pwm timer 17 AF2 setup */

	GPIOB->MODER &= ~(3U << 2*9); /* PB9 as alternate function */
	GPIOB->MODER |= (2U << 2*9);

	GPIOB->AFR[1] &= ~(0xFU << 4*1); /* choose AF2 from mux */
	GPIOB->AFR[1] |= (0x2U << 4*1);

	RCC->APBENR2 |= (1U << 18); /* Enable TIM17 clock */

	TIM17->CR1 = 0U; /* resetting control register */
	TIM17->CR1 |= (1U << 7); /* ARPE buffering */
	TIM17->CNT = 0U; /* reset the timer counter */

	TIM17->PSC = (5U); /* prescaler set to 5 */
	TIM17->ARR = 255U; /* set the autoreload register for 8 bit res */

	TIM17->CR1 |= (1U << 0); /* Enable TIM17 */

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

//	BSP_IWDG_init(); /* Watchdog init */

    TIM1->SR  =  0x00000000U; /* reset status register */

    EXTI->RPR1 = 0x00000000U; /* reset interrupt pending */

    __enable_irq();
}

/*///////////////SSD FUNCTIONS///////////////*/

void ssd_set_clear(void)
{
	GPIOB->ODR |= (0xFFU);
}

void ssd_set(volatile uint32_t digit)
{
	switch(digit)
	{
		case 0:
			GPIOB->ODR &= ~(0x3FU);
			break;
		case 1:
			GPIOB->ODR &= ~(0x6U);
			break;
		case 2:
			GPIOB->ODR &= ~(0x5BU);
			break;
		case 3:
			GPIOB->ODR &= ~(0x4FU);
			break;
		case 4:
			GPIOB->ODR &= ~(0x66U);
			break;
		case 5:
			GPIOB->ODR &= ~(0x6DU);
			break;
		case 6:
			GPIOB->ODR &= ~(0x7DU);
			break;
		case 7:
			GPIOB->ODR &= ~(0x7U);
			break;
		case 8:
			GPIOB->ODR &= ~(0x7FU);
			break;
		case 9:
			GPIOB->ODR &= ~(0x6FU);
			break;
	}
}

void ssd_set_id(void)
{
	ssd_digit(0);
	ssd_set(1); /* 1 */
	ssd_digit(1);
	ssd_set(8); /* 8 */
	ssd_digit(2);
	ssd_set(3); /* 3 */
	ssd_digit(3);
	ssd_set(7); /* 7 */
	ssd_digit_clear();
}

void ssd_set_idle(void)
{
	ssd_digit(0);
	GPIOB->ODR &= ~(0x30U); /* I */
	ssd_digit(1);
	GPIOB->ODR &= ~(0x5EU); /* d */
	ssd_digit(2);
	GPIOB->ODR &= ~(0x38U); /* L */
	ssd_digit(3);
	GPIOB->ODR &= ~(0x79U); /* E */
	ssd_digit_clear();
}

void ssd_set_full(void)
{
	ssd_digit(0);
	GPIOB->ODR &= ~(0x71U); /* F */
	ssd_digit(1);
	GPIOB->ODR &= ~(0x1CU); /* u */
	ssd_digit(2);
	GPIOB->ODR &= ~(0x38U); /* L */
	ssd_digit(3);
	GPIOB->ODR &= ~(0x38U); /* L */
	ssd_digit_clear();
}

void ssd_set_record(void)
{
	ssd_digit(0);
	GPIOB->ODR &= ~(0x50U); /* r */
	ssd_digit(1);
	GPIOB->ODR &= ~(0x39U); /* c */
	ssd_digit(2);
	GPIOB->ODR &= ~(0x5EU); /* d */
	ssd_digit(3);
	ssd_set(((5000 - (ms_ticks + 1)) / 1000) + 1); /* timer calculation */
	ssd_digit_clear();
}

void ssd_set_play(void)
{
	ssd_digit(0);
	GPIOB->ODR &= ~(0x73U); /* P */
	ssd_digit(1);
	GPIOB->ODR &= ~(0x38U); /* L */
	ssd_digit(2);
	GPIOB->ODR &= ~(0x7CU); /* b */
	ssd_digit(3);
	ssd_set(1U + track_selection); /* track enum */
	ssd_digit_clear();
}

void ssd_set_status(void)
{
	ssd_digit(0);
	GPIOB->ODR &= ~(0x77U); /* A */
	ssd_digit(1);
	GPIOB->ODR &= ~(0x1CU); /* v */
	ssd_digit(2);
	GPIOB->ODR &= ~(0x77U); /* A */
	ssd_digit(3);
	ssd_set(4U - empty_tracks); /* show empty tracks */
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

	GPIOA->ODR |= (1U << (4 + digit));
}

/* print and refresh functions */

void ssd_print(void)
{
	switch(state)
	{
		case start: /* print id */
			ssd_set_id();
			break;
		case idle:
			ssd_set_idle();
			break;
		case full:
			ssd_set_full();
			break;
		case record:
			ssd_set_record();
			break;
		case play:
			ssd_set_play();
			break;
		case show_status:
			ssd_set_status();
			break;
		default:
			ssd_digit_clear();
			ssd_set_clear();
	}
}
