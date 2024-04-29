#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <lvgl_input_device.h>
#include <zephyr/drivers/sensor.h>
#include <math.h>
#include <zephyr/types.h>

// #define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
// #include <zephyr/logging/log.h>
// LOG_MODULE_REGISTER(app);

// int main(void)
// {
// 	char count_str[11] = {0};
// 	const struct device *display_dev;
// 	lv_obj_t *hello_world_label;
// 	lv_obj_t *count_label;
// 	lv_style_t label_style;
// 	lv_style_init(&label_style);
	
// 	lv_style_set_text_color(&label_style, lv_color_hex(0xFF0000));

// 	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
// 	if (!device_is_ready(display_dev)) {
// 		LOG_ERR("Device not ready, aborting test");
// 		return 0;
// 	}

// 	hello_world_label = lv_label_create(lv_scr_act());
// 	lv_obj_add_style(hello_world_label, &label_style, LV_PART_MAIN);
// 	lv_label_set_text(hello_world_label, "+");
// 	lv_obj_align(hello_world_label, LV_ALIGN_CENTER, 0, 0);

// 	lv_task_handler();
// 	display_blanking_off(display_dev);

// 	// while (1) {
//     //     // lv_label_set_text(count_label, count_str);
// 	// 	lv_task_handler();
// 	// 	k_sleep(K_MSEC(10));
// 	// }
// }


// #include <zephyr/device.h>
// #include <zephyr/devicetree.h>
// #include <zephyr/drivers/display.h>
// #include <zephyr/drivers/gpio.h>
// #include <lvgl.h>
// #include <stdio.h>
// #include <string.h>
// #include <zephyr/kernel.h>
// #include <lvgl_input_device.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

static void event_handler(lv_event_t *e) {
    lv_obj_t *label = lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    
    uint32_t colors[] = {0xFF0000,0x00FF00,0x0000FF};
    static int color_index = 0;

    if (code == LV_EVENT_CLICKED) {
        color_index = (color_index + 1) % 3;  // Cycle through colors
        lv_obj_t *scr = lv_scr_act();
        lv_obj_set_style_bg_color(scr, lv_color_hex(colors[color_index]), LV_PART_MAIN);
        // lv_obj_add_style(label, &style, LV_PART_MAIN);
        // lv_style_reset(&style); // Reset style to avoid memory leaks
    }
}


int main(void) {
    const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    if (!device_is_ready(display_dev)) {
        LOG_ERR("Display device not ready, aborting test");
        return 0;
    }

    lv_init();

    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Change my color!");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *increase_btn = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(increase_btn, event_handler, LV_EVENT_CLICKED, label);
    lv_label_set_text(lv_label_create(increase_btn), "Change Color");
    lv_obj_align(increase_btn, LV_ALIGN_CENTER, 0, 50);

    display_blanking_off(display_dev);
    lv_task_handler();

    while (1) {
        lv_task_handler();
        k_sleep(K_MSEC(10));
    }
}