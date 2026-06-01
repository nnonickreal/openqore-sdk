#include "led_control.h"
#include "common_apps_imports.h"
#include "ibrt.h"
#include "hal_gpio.h"
#include "hal_iomux.h"

extern uint8_t avrcp_get_media_status(void);
extern APP_STATUS_INDICATION_T app_status_indication_get(void);

static volatile bool is_pairing = false;
static volatile bool is_music_playing = false;

void led_set_pairing_mode(bool enable) {
    is_pairing = enable;
}
void led_set_music_playing(bool enable) {
    is_music_playing = enable;
}

// BUTTON DEBUG
static volatile uint32_t blink_red_timer = 0;
static volatile uint32_t blink_white_timer = 0;

void led_trigger_red_blink(void) {
    blink_red_timer = 25; // 25 ticks * 40ms = 1 second of glowing
}

void led_trigger_white_blink(void) {
    blink_white_timer = 25;
}

// gamma-correction table
static const uint8_t gamma_table[32] = {
    0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 3, 3, 4, 4, 5, 5,
    6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15
};

static uint8_t led_red_duty   = 0;
static uint8_t led_blue_duty  = 0;
static bool is_pwm_running = true;

osTimerId LED_PWM_Timer_id = NULL;
static void LED_PWM_Timer_handler(const void *);
osTimerDef(defLED_PWM_Timer, LED_PWM_Timer_handler);

osTimerId LED_Animation_Timer_id = NULL;
static void LED_Animation_Timer_handler(const void *);
osTimerDef(defLED_Animation_Timer, LED_Animation_Timer_handler);

void LED_statusinit(void) {
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP leds[] = {
        {HAL_IOMUX_PIN_LED1, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_LED2, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P0_3, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
    };
    // pin init
    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)leds, 3);
    hal_gpio_pin_set_dir(HAL_GPIO_PIN_LED1, HAL_GPIO_DIR_OUT, 1);
    hal_gpio_pin_set_dir(HAL_GPIO_PIN_LED2, HAL_GPIO_DIR_OUT, 1);
    hal_gpio_pin_set_dir(HAL_GPIO_PIN_P0_3, HAL_GPIO_DIR_OUT, 0);

    LED_PWM_Timer_id = osTimerCreate(osTimer(defLED_PWM_Timer), osTimerPeriodic, (void *)0);
    LED_Animation_Timer_id = osTimerCreate(osTimer(defLED_Animation_Timer), osTimerPeriodic, (void *)0);

    osTimerStart(LED_PWM_Timer_id, 1);
    osTimerStart(LED_Animation_Timer_id, 40);
}

static void LED_PWM_Timer_handler(const void *) {
    static uint8_t pwm_counter = 0;

    pwm_counter++;
    if (pwm_counter >= 16) {
        pwm_counter = 0;
    }

    // Обработка красного
    if (led_red_duty > 0 && pwm_counter < led_red_duty) {
        hal_gpio_pin_clr(HAL_GPIO_PIN_LED2); 
    } else {
        hal_gpio_pin_set(HAL_GPIO_PIN_LED2); 
    }

    // Обработка синего
    if (led_blue_duty > 0 && pwm_counter < led_blue_duty) {
        hal_gpio_pin_set(HAL_GPIO_PIN_P0_3); 
    } else {
        hal_gpio_pin_clr(HAL_GPIO_PIN_P0_3);
    }
}

// ========== logic ==========
static void LED_Animation_Timer_handler(const void *) {
    static uint32_t tick_cnt = 0;
    static int8_t fade_dir = 1;
    static int8_t fade_idx = 0;

    tick_cnt++;
    fade_idx += fade_dir;
    if (fade_idx >= 31) {
        fade_idx = 31;
        fade_dir = -1;
    } else if (fade_idx <= 0) {
        fade_idx = 0;
        fade_dir = 1;
    }

    bool pwm_needs_run = false;
    
    APP_STATUS_INDICATION_T sys_status = app_status_indication_get();
    bool is_charging = app_battery_is_charging();
    
    if (is_charging && sys_status != APP_STATUS_INDICATION_CHARGING && sys_status != APP_STATUS_INDICATION_FULLCHARGE) {
        is_charging = false; // this should be removed once the charging status is fixed i think
    }

    // ===== BUTTON DEBUG =====
    if (blink_red_timer > 0) {
        blink_red_timer--;
        // red glowing every 5 secs
        led_red_duty = ((blink_red_timer % 10) < 5) ? 255 : 0;
        led_blue_duty = 0;
        pwm_needs_run = true;
    }
    else if (blink_white_timer > 0) {
        blink_white_timer--;
        led_red_duty = 0;
        led_blue_duty = 0;
        pwm_needs_run = true;

        // white glowing
        if ((blink_white_timer % 10) < 5) {
            hal_gpio_pin_clr(HAL_GPIO_PIN_LED1); 
        } else {
            hal_gpio_pin_set(HAL_GPIO_PIN_LED1); 
        }
    }

    // ===== anims =====
    if (is_pairing || sys_status == APP_STATUS_INDICATION_BOTHSCAN) {
        // pairing mode - that's the sound of da police!!!
        led_blue_duty  = gamma_table[fade_idx];
        led_red_duty   = gamma_table[31 - fade_idx];
        pwm_needs_run  = true;
    }
    else if (app_device_bt_is_connected()) {
        // connected
        if (avrcp_get_media_status() == 1 || is_music_playing) {
            led_blue_duty  = gamma_table[fade_idx];
            led_red_duty   = 0;
            pwm_needs_run  = true;
        } else {
            uint32_t cycle_tick = tick_cnt % 125;
            if (cycle_tick < 16) {
                led_blue_duty = gamma_table[cycle_tick * 2];
                pwm_needs_run = true;
            } else if (cycle_tick < 32) {
                led_blue_duty = gamma_table[(31 - cycle_tick) * 2];
                pwm_needs_run = true;
            } else {
                led_blue_duty = 0;
                pwm_needs_run = false;
            }
            led_red_duty = 0;
        }
    }
    else if (is_charging) {
        led_red_duty   = 0;
        led_blue_duty  = 0;
        pwm_needs_run  = false; 
    }
    else {
        uint32_t cycle_tick = tick_cnt % 125;
        if (cycle_tick < 16) {
            led_red_duty = gamma_table[cycle_tick * 2];
            pwm_needs_run = true;
        } else if (cycle_tick < 32) {
            led_red_duty = gamma_table[(31 - cycle_tick) * 2];
            pwm_needs_run = true;
        } else {
            led_red_duty = 0;
            pwm_needs_run = false;
        }
        led_blue_duty = 0;
    }

    // ================= power management =================
    if (pwm_needs_run) {
        if (!is_pwm_running) {
            osTimerStart(LED_PWM_Timer_id, 1);
            is_pwm_running = true;
        }
    } else {
        if (is_pwm_running) {
            osTimerStop(LED_PWM_Timer_id);
            is_pwm_running = false;
        }
        
        // turning off
        hal_gpio_pin_set(HAL_GPIO_PIN_LED1); 
        hal_gpio_pin_clr(HAL_GPIO_PIN_P0_3); 
        
        // charging 
        if (is_charging) {
            hal_gpio_pin_clr(HAL_GPIO_PIN_LED2); 
        } else {
            hal_gpio_pin_set(HAL_GPIO_PIN_LED2);
        }
    }
}