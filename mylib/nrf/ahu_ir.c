/*
 * Copyright (c) 2022 Libre Solar Technologies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "./ahu_ir.h"  // NOLINT

static const struct gpio_dt_spec ir_tx = GPIO_DT_SPEC_GET_OR(DT_PATH(zephyr_user), irt_gpio0, {0});

/*
 *  Function to recognize user target device
 *      and actions
 */
extern void nec_start(uint16_t act) {
    uint8_t device = act >> 8;
    uint8_t action = act & 0xFF;

    // 应该进行相应设备的操作
    uint32_t data = 0x3FE11E;
    // ir burst
    ir_pulse(9600);
    ir_gap(4500);

    for (int i = 0; i < 32; i++) {
        // if (data & 0x80000000) {
        if ((data >> (31 - i)) & 0x01) {
            // active ir output pin
            ir_pulse(560);
            ir_gap(1690);
        } else {
            ir_pulse(560);
            ir_gap(560);
        }
        // data <<= 1;
    }

    // Stop bits
    ir_pulse(560);
    ir_gap(560);
}

/*
 *  IR pin high
 *  Frequency at 38kHz, each cycle for 26 us
 */
extern void ir_pulse(uint64_t ms) {
    uint64_t cyl = ms * 38 / 1000;  //NOLINT
    for (uint64_t i = 0; i < cyl; i++) {
        // Modify to high
        gpio_pin_configure_dt(&ir_tx, GPIO_OUTPUT_ACTIVE);
        k_usleep(13);
        gpio_pin_configure_dt(&ir_tx, GPIO_OUTPUT_INACTIVE);
        k_usleep(13);
    }
}

// Generate empty
extern void ir_gap(uint64_t ms) {  //NOLINT
    gpio_pin_configure_dt(&ir_tx, GPIO_OUTPUT_INACTIVE);
    k_msleep(ms);
}

/*
 *  Init ir gpio pin start with inactive
 */
extern void ir_output_int(void) {
    int err;
    if (!gpio_is_ready_dt(&ir_tx)) {
        printk("\n\rInitial IR error!!!\n\r");
        return;
    }
    err = gpio_pin_configure_dt(&ir_tx, GPIO_OUTPUT_INACTIVE);
    if (err < 0) {
        printk("\n\rGPIO configure faile!!!\n\r");
        return;
    }
}

extern void ir_tx_thread(void) {
    ir_output_int();
    // Value to extrieve current user setting
    uint16_t current_set;
    for (;;) {
        // Event or msgq to receive instructions
        if (k_msgq_get(&ble_uact_msgq, &current_set, K_FOREVER) == 0) {
            // get new instruct
            nec_start(current_set);
        }
        k_msleep(20);
    }
}
