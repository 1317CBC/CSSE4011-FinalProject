#include <M5Core2.h>
#include <SimpleKalmanFilter.h>

float e_mea = 0.1; // 测量噪声
float e_est = 0.1; // 估计误差
float q = 0.01;     // 过程噪声

// Create a Kalman filter object for each axis
SimpleKalmanFilter kalmanX(e_mea, e_est, q);
SimpleKalmanFilter kalmanY(e_mea, e_est, q);
SimpleKalmanFilter kalmanZ(e_mea, e_est, q);
// 实例化Kalman滤波器，用于Pitch和Roll角度
SimpleKalmanFilter kalmanPitch(e_mea, e_est, q);
SimpleKalmanFilter kalmanRoll(e_mea, e_est, q);
SimpleKalmanFilter kalmanGx(e_mea, e_est, q);
SimpleKalmanFilter kalmanGy(e_mea, e_est, q);
SimpleKalmanFilter kalmanGz(e_mea, e_est, q);

float axSum = 0, aySum = 0, azSum = 0;  // 加速度计校准和
float gxSum = 0, gySum = 0, gzSum = 0;  // 陀螺仪校准和

const float ACC_THRESHOLD = 0.2;  // 加速度阈值，用于判断运动状态
const float GYRO_THRESHOLD = 60.0;  // 角速度阈值，用于判断旋转状态

float axOffset = 0, ayOffset = 0, azOffset = 0;  // 加速度计偏移量
float gxOffset = 0, gyOffset = 0, gzOffset = 0;  // 陀螺仪偏移量

const int CALIBRATION_SAMPLES = 1000;  // 校准样本数
const int CALIBRATION_DELAY = 1;      // 校准间隔时间(ms)

float ax, ay, az, gx, gy, gz;
// Kalman filter variables for X, Y, Z axes
float alpha = 0.1; // 平滑因子，介于0和1之间
float smoothedValue = 0;

float prev_vx = 0, prev_vy = 0, prev_vz = 0;
float prev_ax = 0, prev_ay = 0, prev_az = 0;
float temp = 0.0F;
float x = 0, y = 0, z = 0;
float yaw = 0, pitch = 0, roll = 0;
unsigned long prevTime = 0;
float deltaTime = 0.0;
char motionState[10];


void setup()
{
    M5.begin();
    Serial.begin(115200); // 启动串口通信，波特率设置为115200
    M5.IMU.Init();
    calibrateIMU();
    prevTime = millis();
    M5.Lcd.setTextSize(2);
    M5.Lcd.fillScreen(0x00f0ff);
    M5.Lcd.setTextColor(
        GREEN,
        0x00f0ff); 
}

void calibrateIMU() {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 40);
  M5.Lcd.println("Calibrating IMU...");
  M5.Lcd.println("Keep the device still.");

  // 重置校准和
  delay(50);
  axSum = 0; aySum = 0; azSum = 0;
  gxSum = 0; gySum = 0; gzSum = 0;

  float accx = 0.0, accy = 0.0, accz = 0.0, gyx = 0.0, gyy = 0.0, gyz = 0.0;
  // 收集校准样本
  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
    M5.IMU.getAccelData(&accx, &accy, &accz);
    M5.IMU.getGyroData(&gx, &gy, &gz);


    float calibrateax = kalmanGx.updateEstimate(accx * 9.8);
    float calibrateay = kalmanGy.updateEstimate(accy * 9.8);
    float calibrateaz = kalmanGz.updateEstimate(accz * 9.8);

    calibrateax = round(calibrateax * 100) / 100.0;
    calibrateay = round(calibrateay * 100) / 100.0;
    calibrateaz = round(calibrateaz * 100) / 100.0;

    axSum += calibrateax; aySum += calibrateay; azSum += calibrateaz;
    gxSum += gx; gySum += gy; gzSum += gz;

    delay(CALIBRATION_DELAY);
  }

  // 计算偏移量
  axOffset = (axSum / CALIBRATION_SAMPLES) - 0;
  ayOffset = (aySum / CALIBRATION_SAMPLES) - 0;
  azOffset = (azSum / CALIBRATION_SAMPLES) - 9.8;  // 假设设备静止时 z 轴加速度为 1g
  gxOffset = (gxSum / CALIBRATION_SAMPLES) - 0;
  gyOffset = (gySum / CALIBRATION_SAMPLES) - 0;
  gzOffset = (gzSum / CALIBRATION_SAMPLES) - 0;

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 40);
  M5.Lcd.println("Calibration done!");
  delay(100);
}


void loop() {

    M5.update();
    // M5.Touch.update();

    // float ax, ay, az, gx, gy, gz;
    M5.IMU.getAccelData(&ax, &ay, &az); //单位为g
    M5.IMU.getGyroData(&gx, &gy, &gz);
    M5.IMU.getAhrsData(&pitch, &roll, &yaw);

    unsigned long currentTime = micros();
    deltaTime = (currentTime - prevTime) / 1000000.0;
    prevTime = currentTime;
    ax = ax * 9.8;
    ay = ay * 9.8;
    az = az * 9.8;

    float filteredX = kalmanX.updateEstimate(ax);
    float filteredY = kalmanY.updateEstimate(ay);
    float filteredZ = kalmanZ.updateEstimate(az);


    filteredX = round(filteredX * 100) / 100.0;
    filteredY = round(filteredY * 100) / 100.0;
    filteredZ = round(filteredZ * 100) / 100.0;

    filteredX = filteredX - axOffset;
    filteredY = filteredY - ayOffset;
    filteredZ = filteredZ - azOffset;

    float pitchAcc = atan2(ax, sqrt(ay * ay + az * az)) * 180 / PI;
    float rollAcc = atan2(ay, sqrt(ax * ax + az * az)) * 180 / PI;

    float pitch = kalmanPitch.updateEstimate(pitchAcc);
    float roll = kalmanRoll.updateEstimate(rollAcc);
    
    float gxf = kalmanGx.updateEstimate(gx);
    float gyf = kalmanGy.updateEstimate(gy);
    float gzf = kalmanGz.updateEstimate(gz);
    M5.Lcd.setCursor(
        0, 80);  // Move the cursor position to (x,y).  移动光标位置到(x,y)处
    if (abs(pitch) > 3 || abs(roll) > 3 || abs(filteredZ) > 10 || abs(gxf) > GYRO_THRESHOLD) {
        if (abs(pitch) > 10) {
            sprintf(motionState, "%s", (pitch > 0) ? "left" : "right");
        } else if (abs(roll) > 10) {
            sprintf(motionState, "%s", (roll > 0) ? "back" : "front");
        } else if (abs(filteredZ) > 10) {
            if (filteredZ > 11)
            {
                sprintf(motionState, "%s", "uper");
            } else if (filteredZ < 5) {
                 sprintf(motionState, "%s", "Down");
            }
        } else if (abs(gzf) > GYRO_THRESHOLD) {
            sprintf(motionState, "%s", (gzf > 0) ? "anti" : "count");
        }
    }
    M5.Lcd.printf("Motion State: %s", motionState);
    Serial.printf("Motion State: %s\n", motionState);

    delay(10);
}