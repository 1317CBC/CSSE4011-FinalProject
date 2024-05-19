/*
 * Copyright (c) 2022 Libre Solar Technologies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "./ahu_ir.h"  // NOLINT

#define IR_NODE DT_ALIAS(led0)

// static const struct gpio_dt_spec IR_dev = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec IR_dev = GPIO_DT_SPEC_GET(IR_NODE, gpios);

const uint32_t ACT1[] = {0x003F01FE, 0x212EFF00};   // ON/OFF
const uint32_t ACT2[] = {0x003F21DE, 0x010EE31C};   // Adjust Fan speed / speaker volume up
const uint32_t ACT3[] = {0x003FC13E, 0x010E13EC};   // Adjust Fan modes / speaker volume down
const uint32_t ACT4[] = {0x003FE11E, 0x10E837C};   // Fan spinning / speaker mute
const uint32_t ACT5 = 0x003F619E;                   // Fan turn left
const uint32_t ACT6 = 0x003F41BE;                   // Fan turn right

/*
 *  Function to recognize user target device
 *      and actions
 */
extern void nec_start(uint16_t act) {
    uint8_t device = (act >> 8) - 1;
    uint8_t action = act & 0xFF;
    uint32_t data;
    printk("Current device: %d, action: %d\n\r", device, action);
    char output[100];
    switch (action) {
        case A1:
            data = ACT1[device];
            break;
        case A2:
            data = ACT2[device];
            break;
        case A3:
            data = ACT3[device];
            break;
        case A4:
            data = ACT4[device];
            break;
        case A5:
            if (device == 0) {
                data = ACT5;
                break;
            } else {
                snprintf(output, sizeof(output), "Speaker dose not have function 6");
                logging_message(output, ERR);
                return;
            }
        case A6:
            if (device == 0) {
                data = ACT6;
                break;
            } else {
                snprintf(output, sizeof(output), "Speaker dose not have function 7");
                logging_message(output, ERR);
                return;
            }
        default:
            return;
    }
    printk("start send protocol 0x%08X\n\r", data);
    // ir burst
    ir_pulse(9000);
    ir_gap(4500);

    for (int i = 0; i < 32; i++) {
        if (data & 0x80000000) {
            // active ir output pin
            ir_pulse(560);
            ir_gap(1690);
        } else {
            ir_pulse(560);
            ir_gap(560);
        }
        data <<= 1;
    }

    // Stop bits
    ir_pulse(560);
    ir_gap(560);
    printk("Command finsh\n\r");
}

/*
 *  IR pin high
 *  Frequency at 38kHz, each cycle for 26 us
 */
extern void ir_pulse(uint64_t ms) {
    uint64_t cyl = ms * 38 / 1000;  //NOLINT
    for (uint64_t i = 0; i < cyl; i++) {
        // Modify to high
        gpio_pin_set_dt(&IR_dev, 1);
        k_busy_wait(13);
        gpio_pin_set_dt(&IR_dev, 0);
        k_busy_wait(13);
    }
}

// Generate empty
extern void ir_gap(uint64_t ms) {  //NOLINT
    gpio_pin_set_dt(&IR_dev, 0);
    k_busy_wait(ms);
}

/*
 *  Init ir gpio pin start with inactive
 */
extern void ir_output_int(void) {
    int err;

    // if (!gpio_is_ready_dt(&IR_dev.port)) {
    if (!gpio_is_ready_dt(&IR_dev)) {
        printf("The load switch pin GPIO port is not ready.\n");
        return;
    }

    err = gpio_pin_configure_dt(&IR_dev, GPIO_OUTPUT_INACTIVE);
    // err = gpio_pin_configure_dt(&IR_dev, GPIO_OUTPUT_ACTIVE | GPIO_OUTPUT_LOW);
    // err = gpio_pin_configure_dt(&IR_dev, GPIO_OUTPUT_ACTIVE | GPIO_OPEN_DRAIN | GPIO_OUTPUT_LOW);
    if (err != 0) {
        printf("Configuring GPIO pin failed: %d\n", err);
        return;
    }

    printk("IR pin config completed\n\r");
}

extern void ir_tx_thread(void) {
    ir_output_int();
    // Value to extrieve current user setting
    uint16_t current_set;
    for (;;) {
        // Event or msgq to receive instructions
        // if (k_msgq_get(&ble_uact_msgq, &current_set, K_FOREVER) == 0) {
        //     // get new instruct
        //     nec_start(current_set);
        // }
        current_set = 0x0203;
        for (int i =0 ; i < 3; i++) {
            nec_start(current_set);
            k_msleep(108);
        }
        k_msleep(2000);
    }
}
