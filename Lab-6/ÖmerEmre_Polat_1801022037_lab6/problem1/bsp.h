/* BSP.H                   */
/* Author: Ömer Emre Polat */
/* Student No: 1801022037  */

#ifndef BSP_H_
#define BSP_H_

#include "stm32g031xx.h"

#define RefreshRate 10000
#define ButtonBounceTime 400

float number_1;
char kp_input;

typedef enum
{
	none = 0, first = 1, second = 2, third = 3, fourth = 4
}decimal_point_pos;

decimal_point_pos dp_pos;

void BSP_system_init(void);

void BSP_IWDG_init(void);
void BSP_IWDG_refresh(void);

void ssd_print(void);

void ssd_set(volatile uint32_t);
void ssd_set_operation(volatile char);
void ssd_set_overflow(void);
void ssd_set_clear(void);

void ssd_digit(volatile uint32_t);
void ssd_digit_clear(void);

void kp_get_input(void);

#endif
