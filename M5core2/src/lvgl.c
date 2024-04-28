#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <lvgl_input_device.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

int main(void)
{
	char count_str[11] = {0};
	const struct device *display_dev;
	lv_obj_t *hello_world_label;
	lv_obj_t *count_label;
	lv_style_t label_style;
	lv_style_init(&label_style);
	
	lv_style_set_text_color(&label_style, lv_color_hex(0xFF0000));

	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Device not ready, aborting test");
		return 0;
	}

	hello_world_label = lv_label_create(lv_scr_act());
	lv_obj_add_style(hello_world_label, &label_style, LV_PART_MAIN);
	lv_label_set_text(hello_world_label, "+");
	lv_obj_align(hello_world_label, LV_ALIGN_CENTER, 0, 0);

	lv_task_handler();
	display_blanking_off(display_dev);

	// while (1) {
    //     // lv_label_set_text(count_label, count_str);
	// 	lv_task_handler();
	// 	k_sleep(K_MSEC(10));
	// }
}
