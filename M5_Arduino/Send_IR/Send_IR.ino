// #include <IRremote.h>

// const int irPin = 4; // IR LED connected to pin 4
// IRsend irsend(irPin); // Create an instance of the IRsend class

// void setup() {
//   Serial.begin(115200); // Initialize serial communication
// }

// void loop() {
//     // unsigned long data = 0x3F01FE;  //  7F80FC00 invert coode
//     unsigned long data = 0x003FE11E;  //  7887FC00 invert code
//   // Send the NEC command
//     irsend.sendNEC(data, 32); // sendNEC(data, number of bits)
//     Serial.print("Sent NEC command: 0x");
//     Serial.println(data, HEX);
    
//     delay(3000); // Wait for 3 seconds before sending the command again
// }

const int irPin = 4;

void setup() {
  pinMode(irPin, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  unsigned long data = 0x3FE11E;  // NEC data for 7887FC00 
  sendNEC(data, 32);
  Serial.print("Sent NEC command: 0x");
  Serial.println(data, HEX);
  delay(3000); // send IR data each 3 seconds
}

void sendNEC(unsigned long data, int nbits) {
  // 发送引导码
  pulseIR(9000); // 9ms High
  space(4500); // 4.5ms low 

  // send data
  for (int i = 0; i < nbits; i++) {
    if (data & 0x80000000) {
      pulseIR(560); // High for 1
      space(1690); // low for 1
    } else {
      pulseIR(560); // High for 0
      space(560);  // Low for 0
    }
    data <<= 1;
  }

  // send stop bit
  pulseIR(560);
  space(560); // last signal low
}

void pulseIR(long microsecs) {
  // use 38KHz frequency send data
  long cycles = microsecs * 38 / 1000;
  for (long i = 0; i < cycles; i++) {
    digitalWrite(irPin, HIGH);
    delayMicroseconds(13); // 38KHz is 26us for 1 cyrcle, half high half low
    digitalWrite(irPin, LOW);
    delayMicroseconds(13); // half low
  }
}

void space(long microsecs) {
  digitalWrite(irPin, LOW);
  delayMicroseconds(microsecs);
}