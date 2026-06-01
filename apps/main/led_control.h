#ifndef __LED_CONTROL_H__
#define __LED_CONTROL_H__

#include <stdint.h>
#include <stdbool.h>

void led_set_pairing_mode(bool enable);
void led_set_music_playing(bool enable);

void LED_statusinit(void);
void startLED_status(int ms);
void stopLED_status(void);
void LED_statusfun(const void *);
// button debug
void led_trigger_red_blink(void);
void led_trigger_white_blink(void);
#endif