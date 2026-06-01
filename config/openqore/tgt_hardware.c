#include "tgt_hardware.h"
#include "drc.h"
#include "fir_process.h"
#include "iir_process.h"
#include "limiter.h"
#include "spectrum_fix.h"
#include "hal_iomux.h"
#include "hal_key.h"

// ===================== LED =====================

const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_pinmux_pwl[CFG_HW_PLW_NUM] = {
    {HAL_IOMUX_PIN_NUM, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
#if (CFG_HW_PLW_NUM > 1)
    {HAL_IOMUX_PIN_NUM, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
#endif
}; // software pwm rules!!!!

#define CFG_HW_GPIOKEY_NUM (3)
#define CFG_HW_GPIOKEY_DOWN_LEVEL (0)
#define CFG_HW_GPIOKEY_UP_LEVEL (1)

// ===================== GPIO KEYS =====================

const struct HAL_KEY_GPIOKEY_CFG_T cfg_hw_gpio_key_cfg[CFG_HW_GPIOKEY_NUM] = {
    // Volume Down -> P2_2
    {HAL_KEY_CODE_FN3, {HAL_IOMUX_PIN_P2_2, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE}},
    
    // Volume Up -> P2_3
    {HAL_KEY_CODE_FN4, {HAL_IOMUX_PIN_P2_3, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE}},
    
    // NC / Noise Cancellation -> P1_6
    {HAL_KEY_CODE_FN5, {HAL_IOMUX_PIN_P1_6, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE}, HAL_KEY_GPIOKEY_VAL_LOW}
    
    // NFC - disabled because for some reason it triggers on its own
    // {HAL_KEY_CODE_FN2, {HAL_IOMUX_PIN_P2_6, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE}} 
};

// ===================== BATTERY / CHARGER =====================

const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_enable_cfg = {
    HAL_IOMUX_PIN_NUM, HAL_IOMUX_FUNC_AS_GPIO,
    HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE};

const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_detecter_cfg = {
    HAL_IOMUX_PIN_P2_6, HAL_IOMUX_FUNC_AS_GPIO,
    HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE};

const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_indicator_cfg = {
    HAL_IOMUX_PIN_NUM, HAL_IOMUX_FUNC_AS_GPIO,
    HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE};

// ===================== BT =====================

const char *BT_LOCAL_NAME = "MyHeadphones";
const char *BLE_DEFAULT_NAME = "MyHeadphones BLE";

uint8_t ble_addr[6] = {0xBE,0x99,0x34,0x45,0x56,0x67};
uint8_t bt_addr[6]  = {0x1e,0x57,0x34,0x45,0x56,0x67};

// ===================== AUDIO =====================

#define TX_PA_GAIN 10 // this doesn't do anything and idk why

const struct CODEC_DAC_VOL_T codec_dac_vol[TGT_VOLUME_LEVEL_QTY] = {
    {TX_PA_GAIN, 0x00, -99},
    {TX_PA_GAIN, 0x00, -50},
    {TX_PA_GAIN, 0x00, -45},
    {TX_PA_GAIN, 0x00, -42},
    {TX_PA_GAIN, 0x00, -39},
    {TX_PA_GAIN, 0x00, -36},
    {TX_PA_GAIN, 0x00, -33},
    {TX_PA_GAIN, 0x00, -30},
    {TX_PA_GAIN, 0x00, -27},
    {TX_PA_GAIN, 0x00, -24},
    {TX_PA_GAIN, 0x00, -21},
    {TX_PA_GAIN, 0x00, -18},
    {TX_PA_GAIN, 0x00, -15},
    {TX_PA_GAIN, 0x00, -12},
    {TX_PA_GAIN, 0x00, -9},
    {TX_PA_GAIN, 0x00, -6},
    {TX_PA_GAIN, 0x00, -3},
    {TX_PA_GAIN, 0x00, 0},
};

// ===================== AUDIO INPUT =====================

#define CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV \
  (AUD_CHANNEL_MAP_CH0 | AUD_VMIC_MAP_VMIC1)

#define CFG_HW_AUD_INPUT_PATH_LINEIN_DEV \
  (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)

#define CFG_HW_AUD_INPUT_PATH_ASRMIC_DEV \
  (AUD_CHANNEL_MAP_CH0 | AUD_VMIC_MAP_VMIC1)

const struct AUD_IO_PATH_CFG_T cfg_audio_input_path_cfg[CFG_HW_AUD_INPUT_PATH_NUM] = {
    {AUD_INPUT_PATH_MAINMIC, CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV},
    {AUD_INPUT_PATH_LINEIN,  CFG_HW_AUD_INPUT_PATH_LINEIN_DEV},
    {AUD_INPUT_PATH_ASRMIC,  CFG_HW_AUD_INPUT_PATH_ASRMIC_DEV},
};

// ===================== AUDIO OUTPUT =====================

const struct AUD_IO_PATH_CFG_T cfg_audio_output_path_cfg[CFG_HW_AUD_OUTPUT_PATH_NUM] = {
    {AUD_OUTPUT_PATH_SPEAKER, CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV},
};

// ===================== TWS =====================

bool tgt_tws_get_channel_is_right(void) {
    return true; // we only have headphones haha
}

// ===================== EQ =====================

const int8_t cfg_aud_eq_sbc_band_settings[CFG_HW_AUD_EQ_NUM_BANDS] = {
    0,0,0,0,0,0,0,0
};

const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_tws_channel_cfg = {
    HAL_IOMUX_PIN_NUM, HAL_IOMUX_FUNC_AS_GPIO,
    HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE
};

const struct HAL_IOMUX_PIN_FUNCTION_MAP TOUCH_I2C_SDA = {
    HAL_IOMUX_PIN_NUM, HAL_IOMUX_FUNC_AS_GPIO,
    HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE
};

const struct HAL_IOMUX_PIN_FUNCTION_MAP TOUCH_I2C_SCL = {
    HAL_IOMUX_PIN_NUM, HAL_IOMUX_FUNC_AS_GPIO,
    HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE
};

const struct HAL_IOMUX_PIN_FUNCTION_MAP MuteOutPwl = {
    HAL_IOMUX_PIN_P0_1, HAL_IOMUX_FUNC_AS_GPIO,
    HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE
};

// ===================== EQ =====================

const IIR_CFG_T audio_eq_hw_dac_iir_cfg = {
    .gain0 = 0,
    .gain1 = 0,
    .num = 1,
    .param = {
        {IIR_TYPE_PEAK, 0, 1000, 1},
    }
};

const IIR_CFG_T *const audio_eq_hw_dac_iir_cfg_list[EQ_HW_DAC_IIR_LIST_NUM] = {
    &audio_eq_hw_dac_iir_cfg,
};