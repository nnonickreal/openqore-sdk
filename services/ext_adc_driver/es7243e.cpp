#include "es7243e.h"
#include "hal_i2c.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_gpio.h"
#include "cmsis_os.h"
#include <string.h>

#define ES7243E_I2C_ADDR 0x10

static bool g_ext_adc_es7243e_present = false;

static const uint8_t es7243e_init_cmds[][2] = {
    {0x01, 0x3A}, {0x00, 0x80}, {0x04, 0x02}, {0x04, 0x01},
    {0xF9, 0x01}, {0x00, 0x1E}, {0x01, 0x00}, {0x02, 0x80},
    {0x03, 0x20}, {0x0D, 0x00}, {0xF9, 0x00}, {0x04, 0x02},
    {0x04, 0x03}, {0x05, 0x00}, {0x07, 0x00}, {0x09, 0xC5},
    {0x0A, 0x81}, {0x0B, 0x00}, {0x0C, 0x00}, {0x0E, 0xBF},
    {0x0F, 0xA0}, {0x10, 0x38}, {0x11, 0x10}, {0x14, 0x0C},
    {0x15, 0x0C}, {0x17, 0x02}, {0x18, 0x06}, {0x19, 0x77},
    {0x1A, 0x88}, {0x1B, 0x66}, {0x1C, 0x44}, {0x1D, 0x30},
    {0x1E, 0x00}, {0x1F, 0x0C}, {0x20, 0x14}, {0x21, 0x14},
    {0x00, 0x80}, {0x01, 0x3A}, {0x16, 0x3F}, {0x16, 0x00}
};

static const uint8_t es7243e_sleep_cmds[][2] = {
    {0x04, 0x02}, {0x04, 0x01}, {0xF7, 0x30}, {0xF9, 0x01},
    {0x16, 0xFF}, {0x17, 0x00}, {0x01, 0x38}, {0x20, 0x00},
    {0x21, 0x00}, {0x00, 0x1E}, {0x01, 0x30}
};

static uint8_t adc_es7243e_write(uint8_t reg, uint8_t val) {
    uint8_t buf[2] = {reg, val};
    return (uint8_t)hal_i2c_send(HAL_I2C_ID_0, ES7243E_I2C_ADDR, buf, 1, 1, 0, 0);
}

void app_extern_ad_init(void) {
    TRACE(1, "[ES7243E] app_extern_ad_init starting...");

    osDelay(50);

    struct HAL_I2C_CONFIG_T i2c_cfg;
        memset(&i2c_cfg, 0, sizeof(i2c_cfg));
        i2c_cfg.mode = HAL_I2C_API_MODE_SIMPLE;
        i2c_cfg.as_master = 1;                 
        i2c_cfg.use_sync = 1;
        i2c_cfg.speed = 200000;
        hal_i2c_open(HAL_I2C_ID_0, &i2c_cfg);
    
    osDelay(50);

    for (int i = 0; i < (sizeof(es7243e_init_cmds) / 2); i++) {
        uint8_t ret = adc_es7243e_write(es7243e_init_cmds[i][0], es7243e_init_cmds[i][1]);
        if (ret != 0) {
            TRACE(2, "[ES7243E] I2C send failed at cmd %d, ret=%d", i, ret);
            g_ext_adc_es7243e_present = false;
            return;
        }
    }
    
    TRACE(1, "[ES7243E] init success! hello adc :)");
    g_ext_adc_es7243e_present = true;
    
    hal_i2c_close(HAL_I2C_ID_0);
}

void app_adc_ES7243E_sleep(void) {
    if (!g_ext_adc_es7243e_present) return;
    
    TRACE(1, "[ES7243E] going to sleep... bye adc");
    
    struct HAL_I2C_CONFIG_T i2c_cfg;
        memset(&i2c_cfg, 0, sizeof(i2c_cfg));
        i2c_cfg.mode = HAL_I2C_API_MODE_SIMPLE;
        i2c_cfg.as_master = 1;                 
        i2c_cfg.use_sync = 1;
        i2c_cfg.speed = 200000;
        hal_i2c_open(HAL_I2C_ID_0, &i2c_cfg);

    for (int i = 0; i < (sizeof(es7243e_sleep_cmds) / 2); i++) {
        adc_es7243e_write(es7243e_sleep_cmds[i][0], es7243e_sleep_cmds[i][1]);
    }
    
    hal_i2c_close(HAL_I2C_ID_0);
}

void linein_mute_adc(void) {
    if (g_ext_adc_es7243e_present) {
        struct HAL_I2C_CONFIG_T i2c_cfg;
        memset(&i2c_cfg, 0, sizeof(i2c_cfg));
        i2c_cfg.mode = HAL_I2C_API_MODE_SIMPLE;
        i2c_cfg.as_master = 1;                 
        i2c_cfg.use_sync = 1;
        i2c_cfg.speed = 200000;
        hal_i2c_open(HAL_I2C_ID_0, &i2c_cfg);
        
        adc_es7243e_write(0x0B, 0xC0);
        hal_i2c_close(HAL_I2C_ID_0);
    }
}

void linein_demute_adc(void) {
    if (g_ext_adc_es7243e_present) {
        osDelay(100);
        struct HAL_I2C_CONFIG_T i2c_cfg;
        memset(&i2c_cfg, 0, sizeof(i2c_cfg));
        i2c_cfg.mode = HAL_I2C_API_MODE_SIMPLE;
        i2c_cfg.as_master = 1;                 
        i2c_cfg.use_sync = 1;
        i2c_cfg.speed = 200000;
        hal_i2c_open(HAL_I2C_ID_0, &i2c_cfg);
        
        adc_es7243e_write(0x0B, 0x00);
        hal_i2c_close(HAL_I2C_ID_0);
    }
}
