#ifndef BSP_H_
#define BSP_H_

#include "stm32g031xx.h"

void BSP_system_init(void);

void BSP_IWDG_init(void);
void BSP_IWDG_refresh(void);

void BSP_UART_init(uint32_t);
void uart_tx(uint8_t);
uint8_t uart_rx(void);

#endif
