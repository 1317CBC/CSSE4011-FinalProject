#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/gpio.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <math.h>
#include <stdio.h>

typedef struct {
    float ax, ay, az; // calibration data for accelerometer
    float gx, gy, gz; // calibration data for gyroscope
} SensorData;

// calculate pitch and roll angles
void calculateAngles(const SensorData* data, float* pitch, float* roll) {
    if (data == NULL || pitch == NULL || roll == NULL) {
        return;
    }

    // calculate pitch angle Pitch
    *pitch = atan2f(data->ay, sqrtf(data->ax * data->ax + data->az * data->az));
    // calculate roll angle Roll
    *roll = atan2f(-data->ax, data->az);

    //  convert radians to degrees
    *pitch *= 180.0f / 3.1415926f;
    *roll *= 180.0f / 3.1415926f;
}

int main(void) {
    const struct device *dev = device_get_binding("MPU6050");
    if (!dev) {
        printf("Failed to find sensor device\n");
        return 1;
    }

    while (1) {
        SensorData data;
        struct sensor_value accel[3], gyro[3];
        float pitch = 0.0f, roll = 0.0f;

        sensor_sample_fetch(dev);
        sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, accel);
        sensor_channel_get(dev, SENSOR_CHAN_GYRO_XYZ, gyro);

        data.ax = sensor_value_to_double(&accel[0]);
        data.ay = sensor_value_to_double(&accel[1]);
        data.az = sensor_value_to_double(&accel[2]);
        data.gx = sensor_value_to_double(&gyro[0]);
        data.gy = sensor_value_to_double(&gyro[1]);
        data.gz = sensor_value_to_double(&gyro[2]);

        calculateAngles(&data, &pitch, &roll);

        printf("Pitch: %lf degrees\n", (double)pitch);
        printf("Roll: %lf degrees\n", (double)roll);

        k_sleep(K_MSEC(100));
    }

    return 0;
}