#include "app_key.h"
#include "app_bt.h"
#include "app_bt_func.h"
#include "btapp.h"
#include "hal_trace.h"
#include "app_ibrt_customif_cmd.h"
#include "app_ibrt_customif_ui.h"
#include "app_ibrt_if.h"
#include "app_ibrt_ui_test.h"
#include "app_ibrt_voice_report.h"
#include "app_tws_if.h"
#include "led_control.h"

// Forward declarations
extern void app_bt_key_shutdown(APP_KEY_STATUS *status, void *param);
extern struct BT_DEVICE_T app_bt_device;

// ========== IBRT Actions ==========
void send_vol_up(void) {
  uint8_t action[] = {IBRT_ACTION_LOCAL_VOLUP};
  app_ibrt_if_start_user_action(action, sizeof(action));
}

void send_vol_down(void) {
  uint8_t action[] = {IBRT_ACTION_LOCAL_VOLDN};
  app_ibrt_if_start_user_action(action, sizeof(action));
}

void send_play_pause(void) {
  if (app_bt_device.a2dp_play_pause_flag != 0) {
    uint8_t action[] = {IBRT_ACTION_PAUSE};
    app_ibrt_if_start_user_action(action, sizeof(action));
  } else {
    uint8_t action[] = {IBRT_ACTION_PLAY};
    app_ibrt_if_start_user_action(action, sizeof(action));
  }
}

void send_next_track(void) {
  uint8_t action[] = {IBRT_ACTION_FORWARD};
  app_ibrt_if_start_user_action(action, sizeof(action));
}

void send_prev_track(void) {
  uint8_t action[] = {IBRT_ACTION_BACKWARD};
  app_ibrt_if_start_user_action(action, sizeof(action));
}

// ========== Key Handlers ==========
void app_key_single_tap(APP_KEY_STATUS *status, void *param) {
  TRACE(2, "!!! PLAY/PAUSE !!! event=%d", status->event);
  send_play_pause();
}

void app_key_double_tap(APP_KEY_STATUS *status, void *param) {
  TRACE(2, "!!! NEXT TRACK !!! event=%d", status->event);
  send_next_track();
}

void app_key_triple_tap(APP_KEY_STATUS *status, void *param) {
  TRACE(2, "!!! PREV TRACK !!! event=%d", status->event);
  send_prev_track();
}

void app_key_vol_down(APP_KEY_STATUS *status, void *param) {
  TRACE(2, "!!! VOL DOWN !!! event=%d", status->event);
  send_vol_down();
}

void app_key_vol_up(APP_KEY_STATUS *status, void *param) {
  TRACE(2, "!!! VOL UP !!! event=%d", status->event);
  send_vol_up();
}

void app_key_nc_trigger(APP_KEY_STATUS *status, void *param) {
  TRACE(2, "!!! NC KEY !!! event=%d", status->event);
  // led_trigger_white_blink(); // DEBUG
}

void app_key_nfc_trigger(APP_KEY_STATUS *status, void *param) {
  TRACE(2, "!!! NFC TRIGGERED !!! event=%d", status->event);
}

void app_key_power_handler(APP_KEY_STATUS *status, void *param) {
    TRACE(2, "!!! POWER BTN TRIGGERED !!! event=%d", status->event);
    
    // led_trigger_red_blink(); 
    app_bt_key_shutdown(status, param);
}

void app_key_init(void) {
  uint8_t i = 0;
  TRACE(0, "=== KEY INIT Q35 ===");

  const APP_KEY_HANDLE key_cfg[] = {
      {{APP_KEY_CODE_PWR, APP_KEY_EVENT_LONGPRESS}, "power", app_key_power_handler, NULL},
      {{APP_KEY_CODE_PWR, APP_KEY_EVENT_LONGLONGPRESS}, "power_llp", app_key_power_handler, NULL},
      
      {{HAL_KEY_CODE_FN2, APP_KEY_EVENT_CLICK}, "nfc", app_key_nfc_trigger, NULL},
      
      {{HAL_KEY_CODE_FN3, APP_KEY_EVENT_CLICK}, "vol down", app_key_vol_down, NULL},
      
      {{HAL_KEY_CODE_FN4, APP_KEY_EVENT_CLICK}, "vol up", app_key_vol_up, NULL},
      // play/pause is just vol down + vol up pins (0x18)
      {{HAL_KEY_CODE_FN3 | HAL_KEY_CODE_FN4, APP_KEY_EVENT_CLICK}, "play/pause", app_key_single_tap, NULL},
      {{HAL_KEY_CODE_FN3 | HAL_KEY_CODE_FN4, APP_KEY_EVENT_DOUBLECLICK}, "next", app_key_double_tap, NULL},
      {{HAL_KEY_CODE_FN3 | HAL_KEY_CODE_FN4, APP_KEY_EVENT_TRIPLECLICK}, "prev", app_key_triple_tap, NULL},

      {{HAL_KEY_CODE_FN5, APP_KEY_EVENT_CLICK}, "anc", app_key_nc_trigger, NULL},
  };

  app_key_handle_clear();
  for (i = 0; i < (sizeof(key_cfg) / sizeof(APP_KEY_HANDLE)); i++) {
    app_key_handle_registration(&key_cfg[i]);
  }
}

void app_key_init_on_charging(void) {
  TRACE(0, "Keys disabled while charging");
}