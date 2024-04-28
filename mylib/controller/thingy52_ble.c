/*
 * Copyright (c) 2022 Libre Solar Technologies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "thingy52_ble.h"   // NOLINT

static int sendCount = 0;

static uint8_t mfg_data[] = {
    0x4c, 0x00,     // Apple Sign
    0x02, 0x15,     // iBeacon type and length
    0xc3, 0x3e, 0x40, 0x11, 0x9a, 0xc2,     // CSSE4011PAC2
    /* 
     *  device & command
     *  
     */
    0x00, 0x00,
    0x99, 0x99,
    0x99, 0x99,
    0x99, 0x99,
    0x99, 0x99,
    0x00, 0x00,     // Major
    0x00, 0x00,     // Minor
    IBEACON_RSSI    // RSSI
};

/*
 * Set iBeacon demo advertisement data. These values are for
 * sharing the sensor value
 *
 * UUID:  c33e4011-9ac2-A-B-C-D-E-F-G-H-9999
 * Major: 0
 * Minor: 0
 * RSSI:  -56 dBm
 */
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
    BT_DATA(BT_DATA_MANUFACTURER_DATA, mfg_data, ARRAY_SIZE(mfg_data))
};

/*
 *	Set up advertising parameters
 */
extern void start_bt_adv() {
    int err;

    // printf("Bluetooth initialized\n");

    /* Start advertising */
    err = bt_le_adv_start(BT_LE_ADV_NCONN, ad, ARRAY_SIZE(ad),
                    NULL, 0);
    if (err) {
        printf("Advertising failed to start (err %d)\n", err);
        return;
    }
}

/*
 *  Fucntion update delivering data
 *      under current negotiation 
 *          mfg_data[10] for device recognize
 *          mfg_data[11] for action code
 *  
 *  accValue type structure in thingy52_ble.h
 *      include ->  bool value for user action status (user button status)
 *                  double value for z, y, and z axe acc reading
 */
extern void update_mfg_ad(accValue recv_data) {
    int x_data, y_data, z_data;
    x_data = recv_data.xVal * 100;
    y_data = recv_data.yVal * 100;
    z_data = recv_data.zVal * 100;
    uint16_t x, y, z;
    x = x_data;
    y = y_data;
    z = z_data;
    if (recv_data.status) {
        // update ad with new value
        sendCount++;
        mfg_data[10] = 0xFF;
        mfg_data[11] = (x >> 8) & 0xFF;
        mfg_data[12] = x & 0xFF;
        mfg_data[13] = (y >> 8) & 0xFF;
        mfg_data[14] = y & 0xFF;
        mfg_data[15] = (z >> 8) & 0xFF;
        mfg_data[16] = z & 0xFF;
    } else {
        printk("Have send %d messages", sendCount);
        sendCount = 0;
        mfg_data[10] = 0x00;
        mfg_data[11] = 0x00;
        mfg_data[12] = 0x00;
        mfg_data[13] = 0x00;
        mfg_data[14] = 0x00;
        mfg_data[15] = 0x00;
        mfg_data[16] = 0x00;
    }
}

/*
 *   BLE thread, updating adv while receiving
 *      data from message queue "acc_value_msgq"
 */
extern void thingy52_ble_thread() {
    int err, count;
    accValue recv_data;
    count = 0;

    err = bt_enable(NULL);
    if (err) {
        printk("Bluetooth init failed\n\r");
    }

    start_bt_adv();

    for (;;) {
        if (k_msgq_get(&acc_value_msgq, &recv_data, K_FOREVER) == 0) {
            bt_disable();
            k_msleep(10);
            update_mfg_ad(recv_data);
            err = bt_enable(NULL);
            if (err) {
                printk("Bluetooth init failed\n\r");
            }
            k_msleep(10);
            start_bt_adv();
        }
        k_msleep(1);
    }
}

