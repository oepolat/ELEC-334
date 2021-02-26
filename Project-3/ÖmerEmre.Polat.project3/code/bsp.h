/* BSP.H                   */
/* Author: Ömer Emre Polat */
/* Student No: 1801022037  */

#ifndef BSP_H_
#define BSP_H_

#include "stm32g031xx.h"

#define RefreshRate 10000
#define ButtonBounceTime 400
#define MaxTrackIndex 5

uint8_t track_is_full[4];
uint8_t empty_tracks;

uint32_t track_index;
uint8_t track_1[5];

uint32_t clock_ticks;
uint32_t ms_ticks;
char kp_input;

typedef enum
{
	start = 0, idle = 1, track_selected = 2, play = 3, pause = 4, record = 5, delete = 6, full = 7, show_status = 8
}status;

typedef enum
{
	track_one = 0, track_two = 1, track_three = 2, track_four = 3
}track_selector;

typedef enum
{
	recording = 0, recording_done = 1
}recording_states;

typedef enum
{
	playing = 0, playing_done = 1
}play_states;

status state;

track_selector track_selection;

recording_states recording_state;

play_states playing_state;

void BSP_system_init(void);

void BSP_IWDG_init(void);
void BSP_IWDG_refresh(void);

void ssd_print(void);

void ssd_set(volatile uint32_t);
void ssd_set_operation(volatile char);
void ssd_set_id(void);
void ssd_set_idle(void);
void ssd_set_full(void);
void ssd_set_record(void);
void ssd_set_play(void);
void ssd_set_status(void);
void ssd_set_clear(void);

void ssd_digit(volatile uint32_t);
void ssd_digit_clear(void);

void kp_get_input(void);

#endif
