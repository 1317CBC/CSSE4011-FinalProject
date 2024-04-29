#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <math.h>
#include <zephyr/types.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

#define RADIUS 50
#define ANGLE_STEP 5

#define NUM_SAMPLES 10
#define SAMPLE_PERIOD_MS 100
#define RAD_TO_DEG 57.2957795131

const double ACCEL_THRESHOLD = 1;  // accelerometer threshold
const double GYRO_THRESHOLD = 3.0;  // gyroscope threshold

static lv_obj_t *label;
static lv_obj_t *roll_lable;
static lv_style_t label_style;

typedef struct {
    double ax, ay, az; // calibration data for accelerometer
    double gx, gy, gz; // calibration data for gyroscope
} SensorData;


const struct device *dev;

/**
 * @brief Calculate pitch and roll angles
 * 
*/
void calculateAngles(const SensorData* data, float* pitch, float* roll) {
    if (data == NULL || pitch == NULL || roll == NULL) {
        return;
    }

    // calculate pitch angle Pitch
    *pitch = atan2f(data->ay, sqrtf(data->ax * data->ax + data->az * data->az));
    // calculate roll angle Roll
    *roll = atan2f(-data->ax, data->az);

    // convert radians to degrees
    *pitch *= 180.0f / 3.1415926f;
    *roll *= 180.0f / 3.1415926f;
}


static void accinfo(void){
    SensorData data = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    struct sensor_value accel[3], gyro[3];
    float pitch = 0.0f, roll = 0.0f;
    char pitch_str[40];
    char roll_str[40];

    // sensor_sample_fetch(dev);
    // sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, accel);
    // sensor_channel_get(dev, SENSOR_CHAN_GYRO_XYZ, gyro);

    // data.ax = sensor_value_to_double(&accel[0]);
    // data.ay = sensor_value_to_double(&accel[1]);
    // data.az = sensor_value_to_double(&accel[2]);
    // data.gx = sensor_value_to_double(&gyro[0]);
    // data.gy = sensor_value_to_double(&gyro[1]);
    // data.gz = sensor_value_to_double(&gyro[2]);

    for (int i = 0; i < NUM_SAMPLES; i++) {
        sensor_sample_fetch(dev);
        sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, accel);
        sensor_channel_get(dev, SENSOR_CHAN_GYRO_XYZ, gyro);

        data.ax += (sensor_value_to_double(&accel[0]) + 0.05);
        data.ay += (sensor_value_to_double(&accel[1]) - 0.18);
        data.az += (sensor_value_to_double(&accel[2]) - 10.45);
        data.gx += sensor_value_to_double(&gyro[0]);
        data.gy += sensor_value_to_double(&gyro[1]);
        data.gz += sensor_value_to_double(&gyro[2]);

        k_sleep(K_MSEC(10)); // Wait a bit between samples
    }

    data.ax = round((data.ax / NUM_SAMPLES) * 100) / 100;
    data.ay = round((data.ay / NUM_SAMPLES) * 100) / 100;
    data.az = round((data.az / NUM_SAMPLES) * 100) / 100;
    data.gx = round((data.gx / NUM_SAMPLES) * 100) / 100;
    data.gy = round((data.gy / NUM_SAMPLES) * 100) / 100;
    data.gz = round((data.gz / NUM_SAMPLES) * 100) / 100;

    double angle = atan2(data.ay, data.ax) * RAD_TO_DEG;


    if (data.ax > ACCEL_THRESHOLD || data.ay > ACCEL_THRESHOLD ||
        data.ax < -ACCEL_THRESHOLD || data.ay < -ACCEL_THRESHOLD) {
        // double angle = atan2(data.ay, data.ax) * RAD_TO_DEG;
        if ((angle > 67.5 && angle <= 112.5)) {
            sprintf(pitch_str, "back   %lf\n", (double)angle);
        } else if ((angle > 157.5 && angle <= 180) || (angle > -180 && angle <= -157.5)) {
            sprintf(pitch_str, "right   %lf\n", (double)angle);
        } else if ((angle < -67.5 && angle >= -112.5)) {
            sprintf(pitch_str, "front    %lf\n", (double)angle);
        } else if ((angle > -22.5 && angle <= 22.5) || (angle > 157.5 || angle < -157.5)) {
            sprintf(pitch_str, "Left    %lf\n", (double)angle);
        }
    }

    if (data.gz > GYRO_THRESHOLD) {
        sprintf(pitch_str, "anti   %lf\n", (double)data.gz);
    } else if (data.gz < -GYRO_THRESHOLD) {
        sprintf(pitch_str, "count   %lf\n", (double)data.gz);
    }

    // calculateAngles(&data, &pitch, &roll);
    // sprintf(pitch_str, "Pitch: %lf \n", (double)pitch);
    // sprintf(roll_str, "Roll: %lf \n", (double)roll);
    lv_label_set_text(label,pitch_str);
    // lv_label_set_text(roll_lable,roll_str);

    lv_obj_align(label, LV_ALIGN_CENTER, -10, -50);
    // lv_obj_align(roll_lable, LV_ALIGN_CENTER, -10, 0);
    

}

static void create_objects(void)
{
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), LV_PART_MAIN);

    lv_style_init(&label_style);
    lv_style_set_text_font(&label_style, &lv_font_montserrat_28);
    lv_style_set_text_color(&label_style, lv_color_hex(0xFFFFFF));

    label = lv_label_create(scr);
    roll_lable = lv_label_create(scr);
    lv_obj_add_style(label, &label_style, LV_PART_MAIN);
    lv_obj_add_style(roll_lable, &label_style, LV_PART_MAIN);

    lv_label_set_text(label, "0.0\n");
    lv_label_set_text(roll_lable, "0.0\n");

    lv_obj_align(label, LV_ALIGN_CENTER, -30, -50);
    lv_obj_align(roll_lable, LV_ALIGN_CENTER, -30, 0);

    lv_obj_invalidate(scr);
    lv_refr_now(NULL);
}



int main(void)
{
    const struct device *display_dev;
    dev = device_get_binding("MPU6050");
    display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    if (!device_is_ready(display_dev)) {
        LOG_ERR("Device not ready, aborting test");
        return 0;
    }

    if (!dev) {
        printf("Failed to find sensor device\n");
        return 1;
    }

    lv_init();

    create_objects();

    display_blanking_off(display_dev);

    while (1) {
        accinfo();
        lv_timer_handler();
        // k_sleep(K_MSEC(100));
    }
}