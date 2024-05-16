#include <M5Core2.h>
#include <SimpleKalmanFilter.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEBeacon.h>
#include <BLEAdvertising.h>

#define BEACON_MAJOR 0 // Major value
#define BEACON_MINOR 0 // Minor value

BLEAdvertising *pAdvertising;

// define the button properties
const int buttonWidth = 150;         // button width
const int buttonHeight = 150;        // button height
const int buttonY = 40;              // button y position
const int cornerRadius = 15;         // radius of the button corners
const uint16_t shadowColor = 0x39E7; // shadow color

uint8_t custom1 = 0x00; // first byte of iBeacon command UUID
uint8_t custom2 = 0x00; // second byte of iBeacon command UUID
uint8_t custom3 = 0x00; // third byte of iBeacon command UUID
uint8_t custom4 = 0x00; // fourth byte of iBeacon command UUID

float e_mea = 0.1; // noise of the measurement
float e_est = 0.1; // error of the estimate
float q = 0.01;     // noise of the process

// Create a Kalman filter object for each axis
SimpleKalmanFilter kalmanX(e_mea, e_est, q);
SimpleKalmanFilter kalmanY(e_mea, e_est, q);
SimpleKalmanFilter kalmanZ(e_mea, e_est, q);
// create Kalman filter objects for Pitch and Roll angles
SimpleKalmanFilter kalmanPitch(e_mea, e_est, q);
SimpleKalmanFilter kalmanRoll(e_mea, e_est, q);
SimpleKalmanFilter kalmanGx(e_mea, e_est, q);
SimpleKalmanFilter kalmanGy(e_mea, e_est, q);
SimpleKalmanFilter kalmanGz(e_mea, e_est, q);

float axSum = 0, aySum = 0, azSum = 0;  // accelerometer calibration sum
float gxSum = 0, gySum = 0, gzSum = 0;  // IMU calibration sum

const float ACC_THRESHOLD = 0.2;  // acceleration threshold for motion detection
const float GYRO_THRESHOLD = 60.0;  // IMU threshold for rotation detection

float axOffset = 0, ayOffset = 0, azOffset = 0;  // accelerometer offset
float gxOffset = 0, gyOffset = 0, gzOffset = 0;  // IMU offset

const int CALIBRATION_SAMPLES = 1000;  // number of calibration samples
const int CALIBRATION_DELAY = 1;      // time interval for calibration (ms)

float ax, ay, az, gx, gy, gz;
// Kalman filter variables for X, Y, Z axes
float alpha = 0.1; // smoothing factor between 0 and 1
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
    Serial.begin(115200);
    M5.IMU.Init();
    calibrateIMU();
    prevTime = millis();
    M5.Lcd.setFont(&FreeMonoOblique24pt7b); // set the font

    // initialize the display
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setTextColor(0x00f0ff, WHITE);

    // draw buttons
    drawButton("FAN", 0, buttonY, WHITE);
    drawButton("LED", 160, buttonY, WHITE);
}

/**
 * function to start the iBeacon
 * @param custom1 the first byte of the iBeacon command UUID
 * @param custom2 the second byte of the iBeacon command UUID
 * @param custom3 the third byte of the iBeacon command UUID
 * @param custom4 the fourth byte of the iBeacon command UUID
*/
void startBeacon(uint8_t custom1, uint8_t custom2, uint8_t custom3, uint8_t custom4) {
  BLEDevice::init("M5Core2 iBeacon"); // initialize BLE
  
  BLEServer *pServer = BLEDevice::createServer(); // create BLE server
  pAdvertising = pServer->getAdvertising(); // get advertising object
  
  BLEBeacon oBeacon = BLEBeacon();
  oBeacon.setManufacturerId(0x4C00); // set manufacturer ID
  
  // 生成 UUID 字符串
  char uuid[37];
  sprintf(uuid, "C33E4011-9AC2-%02X%02X-%02X%02X-999999999999", custom1, custom2, custom3, custom4);
  oBeacon.setProximityUUID(BLEUUID(uuid));
  
  oBeacon.setMajor(BEACON_MAJOR); // set Major value
  oBeacon.setMinor(BEACON_MINOR); // set Minor value
  oBeacon.setSignalPower(0xC5); // set signal power
  
  // set iBeacon data
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  oAdvertisementData.setFlags(0x04); // BR_EDR_NOT_SUPPORTED 0x04

  // set service data
  std::string strServiceData = "";
  strServiceData += (char)26;     // Len
  strServiceData += (char)0xFF;   // Type
  strServiceData += oBeacon.getData();
  oAdvertisementData.addData(strServiceData);

  // set scan response data
  BLEAdvertisementData oScanResponseData = BLEAdvertisementData();
  oScanResponseData.setName("M5Core2 iBeacon");

  pAdvertising->setAdvertisementData(oAdvertisementData);
  pAdvertising->setScanResponseData(oScanResponseData);
  
  pAdvertising->start(); // start advertising
}

void calibrateIMU() {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 40);
  M5.Lcd.println("Calibrating IMU...");
  M5.Lcd.println("Keep the device still.");

  // reset calibration sum
  delay(50);
  axSum = 0; aySum = 0; azSum = 0;
  gxSum = 0; gySum = 0; gzSum = 0;

  float accx = 0.0, accy = 0.0, accz = 0.0, gyx = 0.0, gyy = 0.0, gyz = 0.0;
  // collect calibration samples
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

  // calculate offset
  axOffset = (axSum / CALIBRATION_SAMPLES) - 0;
  ayOffset = (aySum / CALIBRATION_SAMPLES) - 0;
  azOffset = (azSum / CALIBRATION_SAMPLES) - 9.8;
  gxOffset = (gxSum / CALIBRATION_SAMPLES) - 0;
  gyOffset = (gySum / CALIBRATION_SAMPLES) - 0;
  gzOffset = (gzSum / CALIBRATION_SAMPLES) - 0;

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 40);
  M5.Lcd.println("Calibration done!");
  delay(100);
}

/**
 * function to draw a button
 * @param label the text to display on the button
 * @param x the x position of the button
 * @param y the y position of the button
 * @param color the color of the button
 */
void drawButton(String label, int x, int y, uint16_t color)
{

    // draw shadow
    M5.Lcd.fillRoundRect(x + 3, y + 3, buttonWidth, buttonHeight, cornerRadius, shadowColor);
    // draw button
    M5.Lcd.fillRoundRect(x, y, buttonWidth, buttonHeight, cornerRadius, color);
    // draw border
    M5.Lcd.drawRoundRect(x, y, buttonWidth, buttonHeight, cornerRadius, WHITE);
    // set text color
    M5.Lcd.setCursor(x + 30, y + buttonHeight / 2); // set the cursor position
    M5.Lcd.print(label);                            // print the label
}


void loop() {

    M5.update();

    if (M5.Touch.ispressed())
    {
        TouchPoint_t pos = M5.Touch.getPressPoint(); // get the touch point

        if (pos.y >= buttonY && pos.y < buttonY + buttonHeight)
        {
            if (pos.x >= 0 && pos.x < buttonWidth)
            {                        // area of the FAN button
                custom1 = 0x01;
                M5.Lcd.setTextColor(0x00f0ff, BLUE);
                drawButton("FAN", 0, buttonY, BLUE); // set button FAN to blue
                M5.Lcd.setTextColor(0x00f0ff, WHITE);
                drawButton("LED", 160, buttonY, WHITE); // reset button LED to white
                delay(50);
                startBeacon(custom1, custom2, custom3, custom4);
            }
            else if (pos.x >= 160 && pos.x < 320)
            {                        // area of the LED button
                custom1 = 0x02;
                M5.Lcd.setTextColor(0x00f0ff, BLUE);
                drawButton("LED", 160, buttonY, BLUE); // set button LED to blue
                M5.Lcd.setTextColor(0x00f0ff, WHITE);
                drawButton("FAN", 0, buttonY, WHITE); // reset button FAN to white
                delay(50);
                startBeacon(custom1, custom2, custom3, custom4);
            }
        }
    }

    M5.IMU.getAccelData(&ax, &ay, &az); // unit g
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

    // float pitchAcc = atan2(ax, sqrt(ay * ay + az * az)) * 180 / PI;
    // float rollAcc = atan2(ay, sqrt(ax * ax + az * az)) * 180 / PI;
    float pitch = kalmanPitch.updateEstimate(pitchAcc);
    float roll = kalmanRoll.updateEstimate(rollAcc);
    
    float gxf = kalmanGx.updateEstimate(gx);
    float gyf = kalmanGy.updateEstimate(gy);
    float gzf = kalmanGz.updateEstimate(gz);
    if (abs(pitch) > 3 || abs(roll) > 3 || abs(filteredZ) > 10 || abs(gxf) > GYRO_THRESHOLD) {
        if (abs(pitch) > 10) {
            if (pitch > 0) {
                custom2 = 0x01; // left
            } else {
                custom2 = 0x02; // right
            }
            startBeacon(custom1, custom2, custom3, custom4);
        } else if (abs(roll) > 10) {
            if (roll > 0) {
                custom2 = 0x03; // back
            } else {
                custom2 = 0x04; // front
            }
            startBeacon(custom1, custom2, custom3, custom4);
        } else if (abs(filteredZ) > 10) {
            if (filteredZ > 11)
            {
                custom2 = 0x05; // uper
                startBeacon(custom1, custom2, custom3, custom4);
            } else if (filteredZ < 5) {
                custom2 = 0x06; // down
                startBeacon(custom1, custom2, custom3, custom4);
            }
        } else if (abs(gzf) > GYRO_THRESHOLD) {
            if (gzf > 0) {
                custom2 = 0x07; // anti
            } else {
                custom2 = 0x08; // count
            }
            startBeacon(custom1, custom2, custom3, custom4);
        }
    }
    delay(10);
}