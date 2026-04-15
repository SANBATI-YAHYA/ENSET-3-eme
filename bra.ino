#include <BluetoothSerial.h>
#include <ESP32Servo.h>

BluetoothSerial SerialBT;
const char *BT_NAME = "ESP32_Robot";

// Servos
Servo stillServo;      // Servo 1 - GPIO27
Servo movingServo;     // Servo 2 - GPIO14

int stillPos = 0;      // Still servo position (0-180) - starts at 0
int movingPos = 0;     // Moving servo position (0-180) - starts at 0

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Attach servos
  stillServo.attach(27);
  movingServo.attach(14);

  // Initial positions (0 degrees)
  stillServo.write(0);
  movingServo.write(0);

  // Start Bluetooth
  SerialBT.begin(BT_NAME);
  Serial.println("✓ Bluetooth started");
  Serial.println("Commands: F=90°, B=0°, L=S2→0°, R=S2→90°");
}

void loop() {
  if (SerialBT.available()) {
    char cmd = SerialBT.read();
    processCommand(cmd);
  }
}

void processCommand(char cmd) {
  cmd = toupper(cmd);
  
  switch(cmd) {
    case 'F':
      stillServo.write(90);
      movingServo.write(90);
      stillPos = 90;
      movingPos = 90;
      Serial.println("F: Both → 90°");
      SerialBT.println("Both → 90°");
      break;
      
    case 'B':
      stillServo.write(0);
      movingServo.write(0);
      stillPos = 0;
      movingPos = 0;
      Serial.println("B: Both → 0°");
      SerialBT.println("Both → 0°");
      break;
      
    case 'L':
      movingServo.write(0);
      movingPos = 0;
      Serial.println("L: S2 → 0°");
      SerialBT.println("S2 → 0°");
      break;
      
    case 'R':
      movingServo.write(90);
      movingPos = 90;
      Serial.println("R: S2 → 90°");
      SerialBT.println("S2 → 90°");
      break;
  }
}
