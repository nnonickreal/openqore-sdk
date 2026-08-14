#ifndef __ES7243E_H__
#define __ES7243E_H__

#include "plat_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void app_extern_ad_init(void);
void app_adc_ES7243E_sleep(void);
void linein_mute_adc(void);
void linein_demute_adc(void);

#ifdef __cplusplus
}
#endif

#endif // __ES7243E_H__
