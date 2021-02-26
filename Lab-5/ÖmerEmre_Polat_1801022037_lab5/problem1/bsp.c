#include "bsp.h"

static volatile uint32_t tick = 0;

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

void SysTick_Handler(void)
{
	if(tick > 0)
	{
		--tick;
	}
}

uint8_t uart_rx(void)
{
    volatile uint8_t data = (uint8_t) USART2->RDR;
    return data;
}

void uart_tx(uint8_t c)
{
    USART2->TDR = (uint16_t) c;
    while(!((USART2->ISR) & (1U << 6)));
}

void BSP_UART_init(uint32_t baud_rate)
{
    //Enable Port A clock
    RCC->IOPENR |= (1U << 0);

    //USART2 Enable
    RCC->APBENR1 |= (1U << 17);

    //PA2 as AF mode for USART
    GPIOA->MODER &= ~(3U << 2*2);
    GPIOA->MODER |= (2U << 2*2);

    //Choose AF1 from alternate functions
    GPIOA->AFR[0] &= ~(0xFU << 4*2);
    GPIOA->AFR[0] |= (0x1 << 4*2);


    //PA3 as AF mode for USART
    GPIOA->MODER &= ~(3U << 2*3);
    GPIOA->MODER |= (2U << 2*3);

    //Choose AF1 from alternate functions
    GPIOA->AFR[0] &= ~(0xFU << 4*3);
    GPIOA->AFR[0] |= (0x1 << 4*3);

    //UART2 Setup
    USART2->CR1 = 0; // reset control register
    
    USART2->CR1 |= (1U << 2); // Read enable
    USART2->CR1 |= (1U << 3); // Transfer enable


    USART2->BRR = (uint16_t) (SystemCoreClock / baud_rate);

    USART2->CR1 |= (1U << 0); // USART2 enable
}

void BSP_system_init()
{
    __disable_irq();
    SysTick_Config(SystemCoreClock / 1000);
    //BSP_IWDG_init(); // Watchdog init
    //__enable_irq();
}
