#include "stm32g0xx.h"

volatile unsigned int msTicks = 0; /* variable that counts milliseconds */

void delay_ms(volatile unsigned int DelayTime)
{
	msTicks = 0; /* reset the ticks counter */
	while(1)
	{
		if(msTicks == DelayTime)
		{
			return;
		}
	}
}

void SysTick_Handler(void)
{
	msTicks++; /* every milliseconds this handler will be run and this variable will count */
}

int main(void) {

	__set_PRIMASK(0U); /* set PRIMASK to 0 */

	SysTick_Config(16000); /* set SysTick reload value to 16000 and enable SysTick handler */

	delay_ms(5000); /* wait for 1000ms */

    while(1)
    {

    }

    return 0;
}
