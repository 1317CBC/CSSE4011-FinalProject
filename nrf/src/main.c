#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/__assert.h>
#include <string.h>

#include "./../../mylib/nrf/ahu_ble.h"
#include "./../../mylib/nrf/ahu_ir.h"

#define MY_STACK_SIZE 4096
#define MY_PRIORITY 5

/**
 * @brief Thread to process the system
*/
K_THREAD_DEFINE(ble_id, MY_STACK_SIZE, ble_thread, NULL, NULL, NULL,
        MY_PRIORITY, 0, 0);
K_THREAD_DEFINE(ir_id, MY_STACK_SIZE, ir_tx_thread, NULL, NULL, NULL,
        MY_PRIORITY, 0, 0);

int main(void)
{
    k_thread_start(ble_id);
    k_thread_start(ir_id);
}