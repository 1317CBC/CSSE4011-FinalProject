/*
 * Copyright (c) 2022 Libre Solar Technologies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef AHU_BLE_H   /* NOLINT */
#define AHU_BLE_H

#include <zephyr/types.h>
#include <zephyr/sys/printk.h>
#include <zephyr/settings/settings.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>

#define NAME_LEN 30

#define IBEACON_PREFIX_LEN 10

#define DBG 0
#define ERR 1
#define INF 2
#define WRN 3

extern struct k_msgq ble_uact_msgq;

extern void logging_message(char *msg, int type);
extern void ahu_ble_thread();
extern void scan_cb(const bt_addr_le_t *addr, int8_t rssi, uint8_t type,
            struct net_buf_simple *ad);
extern void start_scan_adv();

#endif  // CSSE4011_TESTDIR_PRAC2_MYLIB_AHU_BLE_H_  // NOLINT