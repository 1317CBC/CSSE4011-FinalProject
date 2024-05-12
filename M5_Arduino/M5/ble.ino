// M5Core2 (Arduino IDE) - BLE central
#include <M5Core2.h>
#include <BLEDevice.h>

static BLEUUID serviceUUID("FFF0");
static BLEUUID    charUUID("FFF1");

static BLEAdvertisedDevice* pServer;
static BLERemoteCharacteristic* pRemoteChar;

static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify)
{
    Serial.print("Received data: ");
    for (int i = 0; i < length; i++) {
        Serial.print(pData[i], HEX);
    }
    Serial.println();
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks 
{
    void onResult(BLEAdvertisedDevice advertisedDevice) 
    {
        Serial.print("Found device: ");
        Serial.println(advertisedDevice.toString().c_str());

        if (advertisedDevice.haveServiceUUID() && advertisedDevice.getServiceUUID().equals(serviceUUID)) {
            pServer = new BLEAdvertisedDevice(advertisedDevice);
        }
    }
};

bool connectToServer() 
{
    BLEClient*  pClient  = BLEDevice::createClient();

    pClient->connect(pServer);
    Serial.println("Connected to server");

    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
        Serial.print("Failed to find service UUID: ");
        Serial.println(serviceUUID.toString().c_str());
        return false;
    }

    pRemoteChar = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteChar == nullptr) {
        Serial.print("Failed to find characteristic UUID: ");
        Serial.println(charUUID.toString().c_str());
        return false;
    }

    pRemoteChar->registerForNotify(notifyCallback);
    return true;
}

void sendStringData(const String& data)
{
    if (pRemoteChar != nullptr) {
        pRemoteChar->writeValue(data.c_str(), data.length());
        Serial.print("Sent data: ");
        Serial.println(data);
    } else {
        Serial.println("Not connected to server, unable to send data.");
    }
}

void setup() 
{
    M5.begin();
    Serial.begin(115200);

    Serial.println("Starting Arduino BLE Client application...");
    BLEDevice::init("");

    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setInterval(1349);
    pBLEScan->setWindow(449);
    pBLEScan->setActiveScan(true);
    pBLEScan->start(5, false);
    
    if (connectToServer()) {
        Serial.println("Connected and ready to send data.");
    } else {
        Serial.println("Failed to connect.");
    }
}

void loop() 
{
    String message = "Hello from M5Stack!";
    sendStringData(message);
    delay(2000);
}