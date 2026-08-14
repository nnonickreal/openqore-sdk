#include "tgt_hardware.h"
#include "drc.h"
#include "fir_process.h"
#include "iir_process.h"
#include "limiter.h"
#include "spectrum_fix.h"
#include "hal_iomux.h"
#include "hal_key.h"
#include "aud_section.h"

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
    
    // NC / Noise Cancellation -> P1_6 (UART0_RX pin???? anker?????) - currently disabled (works strange), i'm confused about it, anc function is on double PWR press for now
    // {HAL_KEY_CODE_FN5, {HAL_IOMUX_PIN_P1_6, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE}}
    
    // NFC - disabled because for some reason it triggers on its own
    // {HAL_KEY_CODE_FN2, {HAL_IOMUX_PIN_P2_6, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE}} 
};

// ===================== BATTERY / CHARGER =====================

const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_enable_cfg = {
    HAL_IOMUX_PIN_NUM, HAL_IOMUX_FUNC_AS_GPIO,
    HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE};

const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_detecter_cfg = {
    HAL_IOMUX_PIN_P2_6, HAL_IOMUX_FUNC_AS_GPIO,
    HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE}; // 2_6 is nfc

const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_indicator_cfg = {
    HAL_IOMUX_PIN_NUM, HAL_IOMUX_FUNC_AS_GPIO,
    HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE};

// ===================== BT =====================

const char *BT_LOCAL_NAME = "MyHeadphones";
const char *BLE_DEFAULT_NAME = "MyHeadphones BLE";

uint8_t ble_addr[6] = {0xBE,0x99,0x34,0x45,0x56,0x67};
uint8_t bt_addr[6]  = {0x1e,0x57,0x34,0x45,0x56,0x67};

// ===================== AUDIO =====================

// #define TX_PA_GAIN 1 // stock firmware doesn't touch this i think?

// const struct CODEC_DAC_VOL_T codec_dac_vol[TGT_VOLUME_LEVEL_QTY] = {
//     {TX_PA_GAIN, 0x00, -99},
//     {TX_PA_GAIN, 0x00, -50},
//     {TX_PA_GAIN, 0x00, -46},
//     {TX_PA_GAIN, 0x00, -43},
//     {TX_PA_GAIN, 0x00, -40},
//     {TX_PA_GAIN, 0x00, -37},
//     {TX_PA_GAIN, 0x00, -34},
//     {TX_PA_GAIN, 0x00, -31},
//     {TX_PA_GAIN, 0x00, -29},
//     {TX_PA_GAIN, 0x00, -27},
//     {TX_PA_GAIN, 0x00, -25},
//     {TX_PA_GAIN, 0x00, -23},
//     {TX_PA_GAIN, 0x00, -22},
//     {TX_PA_GAIN, 0x00, -21},
//     {TX_PA_GAIN, 0x00, -20},
//     {TX_PA_GAIN, 0x00, -19},
//     {TX_PA_GAIN, 0x00, -18},
//     {TX_PA_GAIN, 0x00, -17},
// };

// from reverse ver 2

#define TX_PA_GAIN -1
#define SDM_GAIN 3

const struct CODEC_DAC_VOL_T codec_dac_vol[TGT_VOLUME_LEVEL_QTY] = {
    {TX_PA_GAIN, SDM_GAIN, -42}, // [0]  FF D6
    {TX_PA_GAIN, SDM_GAIN, -99}, // [1]  FF 9D
    {TX_PA_GAIN, SDM_GAIN, -62}, // [2]  FF C2
    {TX_PA_GAIN, SDM_GAIN, -54}, // [3]  FF CA
    {TX_PA_GAIN, SDM_GAIN, -46}, // [4]  FF D2
    {TX_PA_GAIN, SDM_GAIN, -39}, // [5]  FF D9
    {TX_PA_GAIN, SDM_GAIN, -34}, // [6]  FF DE
    {TX_PA_GAIN, SDM_GAIN, -29}, // [7]  FF E3
    {TX_PA_GAIN, SDM_GAIN, -25}, // [8]  FF E7
    {TX_PA_GAIN, SDM_GAIN, -21}, // [9]  FF EB
    {TX_PA_GAIN, SDM_GAIN, -17}, // [10] FF EF
    {TX_PA_GAIN, SDM_GAIN, -13}, // [11] FF F3
    {TX_PA_GAIN, SDM_GAIN, -10}, // [12] FF F6
    {TX_PA_GAIN, SDM_GAIN, -8 }, // [13] FF F8
    {TX_PA_GAIN, SDM_GAIN, -6 }, // [14] FF FA
    {TX_PA_GAIN, SDM_GAIN, -4 }, // [15] FF FC
    {TX_PA_GAIN, SDM_GAIN, -2 }, // [16] FF FE
    {TX_PA_GAIN, SDM_GAIN,  0 }  // [17] FF 00 
}; 

// this is correct but speakers are clipping again


// ===================== AUDIO INPUT =====================

// #define CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV \
//   (AUD_CHANNEL_MAP_CH4 | AUD_VMIC_MAP_VMIC2)

// #define CFG_HW_AUD_INPUT_PATH_LINEIN_DEV \
//   (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)

// #define CFG_HW_AUD_INPUT_PATH_ASRMIC_DEV \
//   (AUD_CHANNEL_MAP_CH0 | AUD_VMIC_MAP_VMIC1)

// const struct AUD_IO_PATH_CFG_T cfg_audio_input_path_cfg[CFG_HW_AUD_INPUT_PATH_NUM] = {
//     {AUD_INPUT_PATH_MAINMIC, CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV},
//     {AUD_INPUT_PATH_LINEIN,  CFG_HW_AUD_INPUT_PATH_LINEIN_DEV},
//     {AUD_INPUT_PATH_ASRMIC,  CFG_HW_AUD_INPUT_PATH_ASRMIC_DEV},
// };


// const struct AUD_IO_PATH_CFG_T cfg_audio_input_path_cfg[CFG_HW_AUD_INPUT_PATH_NUM] = {
//     {AUD_INPUT_PATH_MAINMIC,  0x00030032}, 
//     {AUD_INPUT_PATH_LINEIN,   0x00000003}, 
//     {AUD_INPUT_PATH_ASRMIC,   0x00010001},
//     {AUD_INPUT_PATH_ANC_WNR,  0x00020010},
//     {AUD_INPUT_PATH_ANC_MIC1, 0x00020010}, 
//     {AUD_INPUT_PATH_ANC_MIC2, 0x00000001},
//     {AUD_INPUT_PATH_ANC_MIC3, 0x00000004},
//     {AUD_INPUT_PATH_ANC_MIC4, 0x00000002}, 
//     {AUD_INPUT_PATH_ANC_MIC5, 0x00000008}, 
// };

// SECOND TEST

#define USER_TALK_MIC AUD_CHANNEL_MAP_CH4
#define USER_TALK_VMIC_CFG AUD_VMIC_MAP_VMIC3

#define CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV (USER_TALK_MIC | USER_TALK_VMIC_CFG)

#ifdef VOICE_DETECTOR_EN
#define CFG_HW_AUD_INPUT_PATH_VADMIC_DEV                                       \
  (AUD_CHANNEL_MAP_CH4 | AUD_VMIC_MAP_VMIC1)
#else
#define CFG_HW_AUD_INPUT_PATH_ASRMIC_DEV                                       \
  (AUD_CHANNEL_MAP_CH4 | AUD_VMIC_MAP_VMIC3)
#endif

const struct AUD_IO_PATH_CFG_T
    cfg_audio_input_path_cfg[CFG_HW_AUD_INPUT_PATH_NUM] = {
        {
            AUD_INPUT_PATH_MAINMIC,
            CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV,
        },
        {
            AUD_INPUT_PATH_LINEIN,
            CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV,
        },
#ifdef VOICE_DETECTOR_EN
        {
            AUD_INPUT_PATH_VADMIC,
            CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV,
        },
#else
        {
            AUD_INPUT_PATH_ASRMIC,
            CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV,
        },
#endif
};

// ===================== AUDIO OUTPUT =====================

const struct AUD_IO_PATH_CFG_T cfg_audio_output_path_cfg[CFG_HW_AUD_OUTPUT_PATH_NUM] = {
    {AUD_OUTPUT_PATH_SPEAKER, CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV},
};

// ===================== TWS =====================

bool tgt_tws_get_channel_is_right(void) {
    return true; // we only have headphones haha
}

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

const int8_t cfg_aud_eq_sbc_band_settings[CFG_HW_AUD_EQ_NUM_BANDS] = {
    0,0,0,0,0,0,0,0
};

// const IIR_CFG_T audio_eq_hw_dac_iir_cfg = {
//     .gain0 = 0,
//     .gain1 = 0,
//     .num = 1,
//     .param = {
//         {IIR_TYPE_PEAK, 0, 1000, 1},
//     }
// };

// const IIR_CFG_T *const audio_eq_hw_dac_iir_cfg_list[EQ_HW_DAC_IIR_LIST_NUM] = {
//     &audio_eq_hw_dac_iir_cfg,
// };

// ==================== ANC testing ====================

#define IIR_COUNTER_FF_L (7)
#define IIR_COUNTER_FF_R (7)
#define IIR_COUNTER_FB_L (8)
#define IIR_COUNTER_FB_R (8)

static const struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_48k_mode0 = {
    .anc_cfg_ff_l = {
        .total_gain = 404,
        .iir_bypass_flag = 0,
        .iir_counter = 7,
        .iir_coef[0].coef_b = {42371479, -84378147, 42022936},
        .iir_coef[0].coef_a = {134217728, -266827131, 132660846},
        .iir_coef[1].coef_b = {134197330, -268324896, 134127581},
        .iir_coef[1].coef_a = {134217728, -268324896, 134107183},
        .iir_coef[2].coef_b = {134680326, -267492964, 132825530},
        .iir_coef[2].coef_a = {134217728, -267492964, 133288128},
        .iir_coef[3].coef_b = {132442353, -260183017, 127880045},
        .iir_coef[3].coef_a = {134217728, -260183017, 126104669},
        .iir_coef[4].coef_b = {126276189, -248583643, 122840832},
        .iir_coef[4].coef_a = {134217728, -248583643, 114899293},
        .iir_coef[5].coef_b = {134199949, -268313530, 134113757},
        .iir_coef[5].coef_a = {134217728, -268313530, 134095978},
        .iir_coef[6].coef_b = {35271582, -64167876, 29417557},
        .iir_coef[6].coef_a = {134217728, -256233876, 122537411},
        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_ff_r = {
        .total_gain = 404,
        .iir_bypass_flag = 0,
        .iir_counter = 7,
        .iir_coef[0].coef_b = {42371479, -84378147, 42022936},
        .iir_coef[0].coef_a = {134217728, -266827131, 132660846},
        .iir_coef[1].coef_b = {134197330, -268324896, 134127581},
        .iir_coef[1].coef_a = {134217728, -268324896, 134107183},
        .iir_coef[2].coef_b = {134680326, -267492964, 132825530},
        .iir_coef[2].coef_a = {134217728, -267492964, 133288128},
        .iir_coef[3].coef_b = {132442353, -260183017, 127880045},
        .iir_coef[3].coef_a = {134217728, -260183017, 126104669},
        .iir_coef[4].coef_b = {126276189, -248583643, 122840832},
        .iir_coef[4].coef_a = {134217728, -248583643, 114899293},
        .iir_coef[5].coef_b = {134199949, -268313530, 134113757},
        .iir_coef[5].coef_a = {134217728, -268313530, 134095978},
        .iir_coef[6].coef_b = {35271582, -64167876, 29417557},
        .iir_coef[6].coef_a = {134217728, -256233876, 122537411},
        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_fb_l = {
        .total_gain = 440,
        .iir_bypass_flag = 0,
        .iir_counter = 8,
        .iir_coef[0].coef_b = {22092920, -42013462, 19973944},
        .iir_coef[0].coef_a = {134217728, -265246020, 131047240},
        .iir_coef[1].coef_b = {134162477, -268213947, 134051515},
        .iir_coef[1].coef_a = {134217728, -268213880, 133996331},
        .iir_coef[2].coef_b = {133081758, -264617931, 131579040},
        .iir_coef[2].coef_a = {134217728, -264617931, 130443071},
        .iir_coef[3].coef_b = {133769344, -265737449, 131991882},
        .iir_coef[3].coef_a = {134217728, -265722722, 131558225},
        .iir_coef[4].coef_b = {517809225, -1002515564, 484900765},
        .iir_coef[4].coef_a = {134217728, -251747730, 117724428},
        .iir_coef[5].coef_b = {108059646, -205798509, 100524857},
        .iir_coef[5].coef_a = {134217728, -205798509, 74366775},
        .iir_coef[6].coef_b = {134242987, -268020005, 133778453},
        .iir_coef[6].coef_a = {134217728, -268020005, 133803712},
        .iir_coef[7].coef_b = {134923187, -267007008, 132094633},
        .iir_coef[7].coef_a = {134217728, -267007008, 132800093},
        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_fb_r = {
        .total_gain = 445,
        .iir_bypass_flag = 0,
        .iir_counter = 8,
        .iir_coef[0].coef_b = {22092920, -42013462, 19973944},
        .iir_coef[0].coef_a = {134217728, -265246020, 131047240},
        .iir_coef[1].coef_b = {134162477, -268213947, 134051515},
        .iir_coef[1].coef_a = {134217728, -268213880, 133996331},
        .iir_coef[2].coef_b = {133081758, -264617931, 131579040},
        .iir_coef[2].coef_a = {134217728, -264617931, 130443071},
        .iir_coef[3].coef_b = {133769344, -265737449, 131991882},
        .iir_coef[3].coef_a = {134217728, -265722722, 131558225},
        .iir_coef[4].coef_b = {517809225, -1002515564, 484900765},
        .iir_coef[4].coef_a = {134217728, -251747730, 117724428},
        .iir_coef[5].coef_b = {108059646, -205798509, 100524857},
        .iir_coef[5].coef_a = {134217728, -205798509, 74366775},
        .iir_coef[6].coef_b = {134242987, -268020005, 133778453},
        .iir_coef[6].coef_a = {134217728, -268020005, 133803712},
        .iir_coef[7].coef_b = {134923187, -267007008, 132094633},
        .iir_coef[7].coef_a = {134217728, -267007008, 132800093},
        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    }
};

static const struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_44p1k_mode0 = {
    .anc_cfg_ff_l = {
        .total_gain = 404,
        .iir_bypass_flag = 0,
        .iir_counter = 7,
        .iir_coef[0].coef_b = {42365162, -84331895, 41985995},
        .iir_coef[0].coef_a = {134217728, -266680866, 132524051},
        .iir_coef[1].coef_b = {134195527, -268315122, 134119612},
        .iir_coef[1].coef_a = {134217728, -268315122, 134097411},
        .iir_coef[2].coef_b = {134721080, -267408691, 132702879},
        .iir_coef[2].coef_a = {134217728, -267408691, 133206231},
        .iir_coef[3].coef_b = {132290559, -259464057, 127338177},
        .iir_coef[3].coef_a = {134217728, -259464057, 125411008},
        .iir_coef[4].coef_b = {125629586, -246916249, 121914521},
        .iir_coef[4].coef_a = {134217728, -246916249, 113326379},
        .iir_coef[5].coef_b = {134198378, -268302736, 134104567},
        .iir_coef[5].coef_a = {134217728, -268302736, 134085217},
        .iir_coef[6].coef_b = {35412101, -63862070, 29065178},
        .iir_coef[6].coef_a = {134217728, -255156471, 121553952},
        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_ff_r = {
        .total_gain = 404,
        .iir_bypass_flag = 0,
        .iir_counter = 7,
        .iir_coef[0].coef_b = {42365162, -84331895, 41985995},
        .iir_coef[0].coef_a = {134217728, -266680866, 132524051},
        .iir_coef[1].coef_b = {134195527, -268315122, 134119612},
        .iir_coef[1].coef_a = {134217728, -268315122, 134097411},
        .iir_coef[2].coef_b = {134721080, -267408691, 132702879},
        .iir_coef[2].coef_a = {134217728, -267408691, 133206231},
        .iir_coef[3].coef_b = {132290559, -259464057, 127338177},
        .iir_coef[3].coef_a = {134217728, -259464057, 125411008},
        .iir_coef[4].coef_b = {125629586, -246916249, 121914521},
        .iir_coef[4].coef_a = {134217728, -246916249, 113326379},
        .iir_coef[5].coef_b = {134198378, -268302736, 134104567},
        .iir_coef[5].coef_a = {134217728, -268302736, 134085217},
        .iir_coef[6].coef_b = {35412101, -63862070, 29065178},
        .iir_coef[6].coef_a = {134217728, -255156471, 121553952},
        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_fb_l = {
        .total_gain = 440,
        .iir_bypass_flag = 0,
        .iir_counter = 8,
        .iir_coef[0].coef_b = {22165793, -41964419, 19861825},
        .iir_coef[0].coef_a = {134217728, -264965752, 130770448},
        .iir_coef[1].coef_b = {134157592, -268194364, 134036826},
        .iir_coef[1].coef_a = {134217728, -268194285, 133976769},
        .iir_coef[2].coef_b = {132982846, -264281407, 131349282},
        .iir_coef[2].coef_a = {134217728, -264281407, 130114401},
        .iir_coef[3].coef_b = {133729418, -265497759, 131796485},
        .iir_coef[3].coef_a = {134217728, -265480328, 131325606},
        .iir_coef[4].coef_b = {516432468, -997012674, 480809286},
        .iir_coef[4].coef_a = {134217728, -250352492, 116363844},
        .iir_coef[5].coef_b = {106314447, -201355379, 98276956},
        .iir_coef[5].coef_a = {134217728, -201355379, 70373675},
        .iir_coef[6].coef_b = {134245217, -267983188, 133739671},
        .iir_coef[6].coef_a = {134217728, -267983188, 133767160},
        .iir_coef[7].coef_b = {134985215, -266880372, 131907961},
        .iir_coef[7].coef_a = {134217728, -266880372, 132675448},
        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_fb_r = {
        .total_gain = 445,
        .iir_bypass_flag = 0,
        .iir_counter = 8,
        .iir_coef[0].coef_b = {22165793, -41964419, 19861825},
        .iir_coef[0].coef_a = {134217728, -264965752, 130770448},
        .iir_coef[1].coef_b = {134157592, -268194364, 134036826},
        .iir_coef[1].coef_a = {134217728, -268194285, 133976769},
        .iir_coef[2].coef_b = {132982846, -264281407, 131349282},
        .iir_coef[2].coef_a = {134217728, -264281407, 130114401},
        .iir_coef[3].coef_b = {133729418, -265497759, 131796485},
        .iir_coef[3].coef_a = {134217728, -265480328, 131325606},
        .iir_coef[4].coef_b = {516432468, -997012674, 480809286},
        .iir_coef[4].coef_a = {134217728, -250352492, 116363844},
        .iir_coef[5].coef_b = {106314447, -201355379, 98276956},
        .iir_coef[5].coef_a = {134217728, -201355379, 70373675},
        .iir_coef[6].coef_b = {134245217, -267983188, 133739671},
        .iir_coef[6].coef_a = {134217728, -267983188, 133767160},
        .iir_coef[7].coef_b = {134985215, -266880372, 131907961},
        .iir_coef[7].coef_a = {134217728, -266880372, 132675448},
        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    }
};

const struct_anc_cfg *anc_coef_list_48k[ANC_COEF_LIST_NUM] = {
    &AncFirCoef_48k_mode0,
#if (ANC_COEF_LIST_NUM == 2)
    &AncFirCoef_48k_mode0,
#endif
#if (ANC_COEF_LIST_NUM == 3)
    &AncFirCoef_48k_mode0, &AncFirCoef_48k_mode0,
#endif
#if (ANC_COEF_LIST_NUM == 4)
    &AncFirCoef_48k_mode0, &AncFirCoef_48k_mode0, &AncFirCoef_48k_mode0,
#endif
};

const struct_anc_cfg *anc_coef_list_44p1k[ANC_COEF_LIST_NUM] = {
    &AncFirCoef_44p1k_mode0,
#if (ANC_COEF_LIST_NUM == 2)
    &AncFirCoef_44p1k_mode0,
#endif
#if (ANC_COEF_LIST_NUM == 3)
    &AncFirCoef_44p1k_mode0, &AncFirCoef_44p1k_mode0,
#endif
#if (ANC_COEF_LIST_NUM == 4)
    &AncFirCoef_44p1k_mode0, &AncFirCoef_44p1k_mode0, &AncFirCoef_44p1k_mode0,
#endif
};

const IIR_CFG_T audio_eq_sw_iir_cfg = {.gain0 = 0,
                                       .gain1 = 0,
                                       .num = 14,
                                       .param = {
                                           {IIR_TYPE_PEAK, -2.18, 10, 2},
                                           {IIR_TYPE_PEAK, 7.27, 21, 2},
                                           {IIR_TYPE_PEAK, 2.13, 42, 2},
                                           {IIR_TYPE_PEAK, -1.53, 83, 2},
                                           {IIR_TYPE_PEAK, -0.9, 120, 2},
                                           {IIR_TYPE_PEAK, -4.23, 166, 2},
                                           {IIR_TYPE_PEAK, -1.08, 333, 2},
                                           {IIR_TYPE_PEAK, -1.37, 577, 2},
                                           {IIR_TYPE_PEAK, 0.8, 1000, 2},
                                           {IIR_TYPE_PEAK, 5.35, 2000, 2},
                                           {IIR_TYPE_PEAK, 3.2, 4000, 2},
                                           {IIR_TYPE_PEAK, -4.96, 8000, 2},
                                           {IIR_TYPE_PEAK, 13.58, 16000, 2},
                                           {IIR_TYPE_PEAK, 8.89, 20000, 2},
                                       }};

const IIR_CFG_T *const audio_eq_sw_iir_cfg_list[EQ_SW_IIR_LIST_NUM] = {
    &audio_eq_sw_iir_cfg,
};

const FIR_CFG_T audio_eq_hw_fir_cfg_44p1k = {.gain = 0.0f,
                                             .len = 384,
                                             .coef = {
                                                 (1 << 23) - 1,
                                             }};

const FIR_CFG_T audio_eq_hw_fir_cfg_48k = {.gain = 0.0f,
                                           .len = 384,
                                           .coef = {
                                               (1 << 23) - 1,
                                           }};

const FIR_CFG_T audio_eq_hw_fir_cfg_96k = {.gain = 0.0f,
                                           .len = 384,
                                           .coef = {
                                               (1 << 23) - 1,
                                           }};

const FIR_CFG_T *const audio_eq_hw_fir_cfg_list[EQ_HW_FIR_LIST_NUM] = {
    &audio_eq_hw_fir_cfg_44p1k,
    &audio_eq_hw_fir_cfg_48k,
    &audio_eq_hw_fir_cfg_96k,
};

// hardware dac iir eq
const IIR_CFG_T audio_eq_hw_dac_iir_cfg = audio_eq_sw_iir_cfg;

const IIR_CFG_T *const POSSIBLY_UNUSED
    audio_eq_hw_dac_iir_cfg_list[EQ_HW_DAC_IIR_LIST_NUM] = {
        &audio_eq_hw_dac_iir_cfg,
};

// hardware dac iir eq
const IIR_CFG_T audio_eq_hw_adc_iir_adc_cfg = audio_eq_sw_iir_cfg;

const IIR_CFG_T *const POSSIBLY_UNUSED
    audio_eq_hw_adc_iir_cfg_list[EQ_HW_ADC_IIR_LIST_NUM] = {
        &audio_eq_hw_adc_iir_adc_cfg,
};

// hardware iir eq
const IIR_CFG_T audio_eq_hw_iir_cfg = audio_eq_sw_iir_cfg;

const IIR_CFG_T *const POSSIBLY_UNUSED
    audio_eq_hw_iir_cfg_list[EQ_HW_IIR_LIST_NUM] = {
        &audio_eq_hw_iir_cfg,
};

const DrcConfig audio_drc_cfg = {.knee = 3,
                                 .filter_type = {14, -1},
                                 .band_num = 2,
                                 .look_ahead_time = 10,
                                 .band_settings = {
                                     {-20, 0, 2, 3, 3000, 1},
                                     {-20, 0, 2, 3, 3000, 1},
                                 }};

const LimiterConfig audio_drc2_cfg = {
    .knee = 2,
    .look_ahead_time = 10,
    .threshold = -20,
    .makeup_gain = 19,
    .ratio = 1000,
    .attack_time = 3,
    .release_time = 3000,
};

const SpectrumFixConfig audio_spectrum_cfg = {
    .freq_num = 9,
    .freq_list = {200, 400, 600, 800, 1000, 1200, 1400, 1600, 1800},
};