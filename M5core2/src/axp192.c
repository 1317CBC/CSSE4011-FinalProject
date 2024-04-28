#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/regulator.h>
#include <zephyr/lvgl.h>

struct display_buffer_descriptor desc;


static lv_obj_t * create_label(lv_obj_t *parent, const char *text) {
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    return label;
}


static void display_setup(void) {
    const struct device *display_dev = device_get_binding("ILI9341");
    if (display_dev == NULL) {
        printk("Cannot find display device!\n");
        return;
    }

    lv_init();
    lvgl_driver_init();

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = lvgl_display_flush;
    disp_drv.hor_res = 320;
    disp_drv.ver_res = 240;
    disp_drv.user_data = display_dev;
    lv_disp_drv_register(&disp_drv);

    lv_obj_t *scr = lv_disp_get_scr_act(NULL);
    create_label(scr, "Hello, M5Core2!");
}


void setup_axp192(void) {
    const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));
    if (!device_is_ready(i2c_dev)) {
        printk("Failed to bind to I2C0\n");
        return;
    }

    const struct device *axp192_dev = DEVICE_DT_GET(DT_NODELABEL(axp192_pmic));

    if (!device_is_ready(axp192_dev)) {
        printk("AXP192 device not ready\n");
        return;
    }

    // 启用LDO3供电给显示器
    const struct device *dcdc3 = DEVICE_DT_GET(DT_NODELABEL(lcd_bg));
    if (!device_is_ready(dcdc3)) {
        printk("LDO3 regulator not ready\n");
        return;
    }

    if (regulator_enable(dcdc3) != 0) {
        printk("Failed to enable LDO3\n");
        return;
    }

    printk("AXP192 setup complete, LDO3 enabled.\n");
}

int main(void) {

    // lv_init();
    // printk("start setup axp192\n");
    // setup_axp192();
    // printk("axp192 setup complete\n");


    // static lv_disp_draw_buf_t disp_buf;
    // static lv_color_t buf_1[MY_DISP_HOR_RES * 10];
    // lv_disp_draw_buf_init(&disp_buf, buf_1, NULL, MY_DISP_HOR_RES * 10);

    // printk("start setup display\n");
    // const struct device *display = DEVICE_DT_GET(DT_NODELABEL(ili9342c));
    // if (!device_is_ready(display)) {
    //     printk("Cannot find display device!\n");
    //     return 0;
    // }
    // printk("display setup complete\n");

    // struct display_buffer_descriptor desc = {
    //     .width = 320,
    //     .height = 240,
    //     .pitch = 320,
    //     .buf_size = 320 * 240 * 2,
    // };

    // desc.width = 320;
    // desc.height = 240;
    // desc.pitch = 320;
    // desc.buf_size = 320 * 240 * 2;

    // uint16_t buf[320 * 240];
    // for (int i = 0; i < 320 * 240; i++) {
    //     buf[i] = 0xFF00; // White color
    // }

    // char buffer[100] = "Hello World";

    // // display_write(display, 0, 0, &desc, buf);
    // display_write(display, 0, 0, &desc, buffer);
    // display_blanking_off(display);


    // printk("Hello World displayed!\n");
    // return 0;
    setup_axp192();
    display_setup();
    while (1) {
        lv_task_handler();
        k_sleep(K_MSEC(10));
    }
    return 0;
}
