/* BSP.H                   */
/* Author: Ömer Emre Polat */
/* Student No: 1801022037  */

#ifndef BSP_H_
#define BSP_H_

#include "stm32g031xx.h"

#define RefreshRate 10000
#define ButtonBounceTime 400

float number_1;
float number_2;
char kp_input;

typedef enum
{
	idle = 0, first_number_input = 1, operation_input = 2, second_number_input = 3, result = 4
}status;

typedef enum
{
	none = 0, first = 1, second = 2, third = 3, fourth = 4
}decimal_point_pos;

typedef enum
{
	addition = 0, subtraction = 1, multiplication = 2, division = 3, logarithm = 4, ln = 5, squareroot = 6, square = 7, sine = 8, cosine = 9, tangent = 10, cotangent = 11, pi = 12
}operation;

status state;

decimal_point_pos dp_pos;

operation op;

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
uint8_t kp_input_is_number(void);

void perform_op(void);

#endif
