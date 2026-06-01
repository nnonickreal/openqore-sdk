#ifndef APP_IBRT_STUB_H
#define APP_IBRT_STUB_H

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(IBRT)

typedef struct {
    uint8_t  nv_role;
    uint8_t  current_role;
    uint8_t  access_mode;
    uint8_t  init_done;
    uint8_t  mobile_pair_canceled;
    uint8_t  ibrt_in_poweroff;
    void    *p_mobile_remote_dev;
    void    *p_tws_remote_dev;
    uint16_t mobile_conhandle;
    uint16_t tws_conhandle;
    struct { uint8_t address[6]; } mobile_addr;
    struct { uint8_t codec_type; } a2dp_codec;
} ibrt_ctrl_t;

typedef struct {
    uint8_t box_state;
    uint8_t super_state;
} app_ibrt_ui_t;

#define IBRT_ACTION_LOCAL_VOLUP     1
#define IBRT_ACTION_LOCAL_VOLDN     2
#define IBRT_ACTION_PAUSE           3
#define IBRT_ACTION_PLAY            4
#define IBRT_ACTION_FORWARD         5
#define IBRT_ACTION_BACKWARD        6
#define IBRT_ACTION_ANC_NOTIRY_MASTER_EXCHANGE_COEF 7

#define IBRT_IN_BOX_CLOSED          0
#define IBRT_OUT_BOX                1
#define IBRT_CONNECT_MOBILE_FAILED  0
#define IBRT_CONNECTE_TRIGGER       0
#define MOBILE_LINK                 0
#define IBRT_UI_NO_ERROR            0
#define IBRT_FREEMAN_PAIR_TRIGGER   0
#define NO_LINK_TYPE                0
#define IBRT_CLOSE_BOX_EVENT        0

#define IBRT_SLAVE                  0
#define IBRT_MASTER                 1
#define IBRT_UNKNOW                 255
#define BTIF_HCI_INVALID_HANDLE     0xFFFF

static inline ibrt_ctrl_t *app_tws_ibrt_get_bt_ctrl_ctx(void) {
    static ibrt_ctrl_t stub = {
        .nv_role = IBRT_MASTER,
        .current_role = IBRT_MASTER,
        .access_mode = 0,
        .mobile_conhandle = BTIF_HCI_INVALID_HANDLE,
        .tws_conhandle = BTIF_HCI_INVALID_HANDLE,
    };
    return &stub;
}

static inline ibrt_ctrl_t *app_ibrt_if_get_bt_ctrl_ctx(void) {
    return app_tws_ibrt_get_bt_ctrl_ctx();
}

static inline app_ibrt_ui_t *app_ibrt_ui_get_ctx(void) {
    static app_ibrt_ui_t ui = { .box_state = IBRT_OUT_BOX };
    return &ui;
}

static inline int app_tws_ibrt_mobile_link_connected(void) {
    extern int btif_me_get_activeCons(void);
    return btif_me_get_activeCons() > 0;
}

static inline int app_tws_ibrt_tws_link_connected(void) { 
    return 0; 
}

static inline int app_tws_ibrt_slave_ibrt_link_connected(void) { 
    return 0; 
}

static inline void app_tws_ibrt_disconnect_connection(void *dev) { 
    (void)dev; 
}

static inline int app_tws_is_left_side(void) {
    return 1;
}

static inline int app_tws_is_right_side(void) {
    return 0;
}

static inline void app_ibrt_if_start_user_action(uint8_t device_id, uint8_t action) {
    extern void app_bt_volumeup(void);
    extern void app_bt_volumedown(void);
    extern void a2dp_handleKey(uint8_t key);
    
    switch(action) {
        case IBRT_ACTION_LOCAL_VOLUP:
            app_bt_volumeup();
            break;
        case IBRT_ACTION_LOCAL_VOLDN:
            app_bt_volumedown();
            break;
        case IBRT_ACTION_PLAY:
            a2dp_handleKey(AVRCP_KEY_PLAY);
            break;
        case IBRT_ACTION_PAUSE:
            a2dp_handleKey(AVRCP_KEY_PAUSE);
            break;
        case IBRT_ACTION_FORWARD:
            a2dp_handleKey(AVRCP_KEY_FORWARD);
            break;
        case IBRT_ACTION_BACKWARD:
            a2dp_handleKey(AVRCP_KEY_BACKWARD);
            break;
        default:
            break;
    }
}

static inline void app_ibrt_sync_volume_info(void) {}
static inline void app_ibrt_if_force_audio_retrigger_slave_sync(void) {}
static inline void app_ibrt_if_force_audio_retrigger(void) {}
static inline void app_ibrt_customif_test3_cmd_send(void *d, int l) {}
static inline int app_ibrt_ui_get_enter_pairing_mode(void) { return 0; }
static inline void app_ibrt_ui_set_enter_pairing_mode(int m) {}
static inline void app_ibrt_ui_judge_scan_type(int a, int b, int c) {}
static inline void app_ibrt_if_enter_pairing_after_tws_connected(void) {}
static inline void app_ibrt_ui_set_freeman_enable(void) {}
static inline void app_ibrt_ui_event_entry(int evt) {}
static inline void app_tws_if_trigger_role_switch(void) {}

#endif /* !defined(IBRT) */

#ifdef __cplusplus
}
#endif

#endif /* APP_IBRT_STUB_H */
