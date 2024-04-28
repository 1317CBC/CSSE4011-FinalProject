/*
 * Copyright (c) 2022 Libre Solar Technologies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef THINGY52_BLE_H  /* NOLINT */
#define THINGY52_BLE_H

#include <stddef.h>
#include <stdlib.h>
#include <zephyr/sys/util.h>
#include <zephyr/kernel.h>
#include <zephyr/types.h>
#include <zephyr/sys/printk.h>
#include <zephyr/settings/settings.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#ifndef IBEACON_RSSI
#define IBEACON_RSSI 0xc8
#endif

typedef struct {
    bool status;
    double xVal;
    double yVal;
    double zVal;
} accValue;

extern struct k_msgq acc_value_msgq;

extern void start_bt_adv();
extern void update_mfg_ad(accValue recv_data);
extern void thingy52_ble_thread();

#endif  // CSSE4011_TESTDIR_REPO_MYLIB_MOBILE_THINGY52_BLE_H_
