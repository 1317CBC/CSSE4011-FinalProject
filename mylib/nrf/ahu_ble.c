/*
 * Copyright (c) 2022 Libre Solar Technologies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "./ahu_ble.h"  // NOLINT

K_MSGQ_DEFINE(ble_uact_msgq, sizeof(uint16_t), 10, 4);

// Set up target's prefix
static const uint8_t ibeacon_prefix[] = {
    0x4c, 0x00,     // Apple
    0x02, 0x15,     // iBeacon type and length
    0xc3, 0x3e,     // CSSE
    0x40, 0x11,     // 4011
    0x9a, 0xc2,     // PAC2
};

static uint64_t current = 0;

/*
 *  Scan advertising data, searching for the proper BLE device
 */
extern void scan_cb(const bt_addr_le_t *addr, int8_t rssi, uint8_t type,
            struct net_buf_simple *ad) {
    char addr_str[100];
    bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));
    uint16_t user_act;

    // parse the adv and limited the range in 1 meter
    while ((ad->len > 1)) {
        uint8_t len = net_buf_simple_pull_u8(ad);
        uint8_t type = net_buf_simple_pull_u8(ad);

        if (len < 1) {
            return;
        }

        /*
         *  Only search for target BLE UUID
         */
        if  ((k_uptime_get() - current) > 100) {
            // Check is the target adv, check length and prefix
#ifdef OUTPUT_TRIAL
            char trial[100];
            // cppcheck-suppress invalidPrintfArgType_sint
            snprintf(trial, sizeof(trial), "Get ble, current time: %lldms\n\rmarked time: %lld", k_uptime_get(), current);  // NOLINT
            logging_message(trial, ERR);
#endif
            if ((len >= IBEACON_PREFIX_LEN + sizeof(ibeacon_prefix)) &&
                    memcmp(ad->data, ibeacon_prefix, sizeof(ibeacon_prefix)) == 0) {
                // remove the ibeacon prefix from the memory
                net_buf_simple_pull_mem(ad, sizeof(ibeacon_prefix));

                // target value after prefix
                const uint8_t *uuid = ad->data;
                /*--------------- Wait for the protocol agree-------------------*/
                user_act = ((uuid[0] << 8) | uuid[1]);
                k_msgq_put(&ble_uact_msgq, &user_act, K_NO_WAIT);
                current = k_uptime_get();
            }
        }

        // skip the present ADV
        net_buf_simple_pull_mem(ad, len - 1);
    }
}

/*
 *  Function to create the adv scanning
 */
extern void start_scan_adv() {
    int err;
    struct bt_le_scan_param scan_param = {
		.type       = BT_LE_SCAN_TYPE_PASSIVE,          //NOLINT
		.options    = BT_LE_SCAN_OPT_FILTER_DUPLICATE,  //NOLINT
        .interval   = BT_GAP_SCAN_FAST_INTERVAL,        //NOLINT
        .window     = BT_GAP_SCAN_SLOW_WINDOW_1,        // Fast scan window 11.25 ms
    };
    err = bt_le_scan_start(&scan_param, scan_cb);
    if (err) {
        return;
    }
}

/*
 *  AHU bluetooth thread
 *      use semaphore to trig the action, start or stop scan adv
 */
extern void ahu_ble_thread() {
    int err;

    err = bt_enable(NULL);
    if (err) {
        return;
    }
    start_scan_adv();
    for (;;) {
        k_msleep(1);
    }
}
