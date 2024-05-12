/**
 * @file receiver.ino
 * @brief  IR receiver for M5 Core2, receive IR signal and print the data to get the protocol, address and command.
*/

#include <M5Core2.h>
#include <IRremote.h>

const int irPin = 32;  // IR pin connect for M5 Core2

void setup() {
  M5.begin();
  Serial.begin(115200);
  IrReceiver.begin(irPin, ENABLE_LED_FEEDBACK);  // initialize IR receiver
  Serial.println("IR Receiver is ready!");
}

void loop() {
    if (IrReceiver.decode()) {  // check if IR signal is received
        Serial.println("Received IR signal:");
        Serial.println(getProtocolName(IrReceiver.decodedIRData.protocol));
        unsigned long data = IrReceiver.decodedIRData.decodedRawData;
        Serial.print("Received data: 0x");
        Serial.println(data, HEX);  // use HEX to print the data
        Serial.print("Protocol: "); 
        Serial.println(IrReceiver.decodedIRData.protocol);  // print the protocol
        Serial.print("Address: 0x");
        Serial.println(IrReceiver.decodedIRData.address, HEX);  // print the address
        Serial.print("Command: 0x");
        Serial.println(IrReceiver.decodedIRData.command, HEX);  // print the command
        Serial.println();
        IrReceiver.resume();  // resume to receive IR signal
    }
  M5.update();
}

/**
 * @brief Get the protocol name
 * 
 * @param protocol The protocol type
 * @return const char* The protocol name
 
*/
const char* getProtocolName(decode_type_t protocol) {
  switch (protocol) {
    case NEC: return "NEC";
    case SONY: return "Sony";
    case RC5: return "RC5";
    case RC6: return "RC6";
    case UNKNOWN: return "Unknown";
    default: return getProtocolString(protocol);
  }
}