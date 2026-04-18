#include <ESP32Servo.h>
#include <BluetoothSerial.h>

BluetoothSerial SerialBT;
Servo testServo;

const int SERVO_PIN = 14;
int currentPos = 45;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Attach servo to pin 14
  testServo.attach(SERVO_PIN);
  
  // Start at 45 degrees
  testServo.write(45);
  currentPos = 45;
  delay(1000);
  
  // Start Bluetooth
  SerialBT.begin("ESP32_SERVO_TEST");
  
  Serial.println("========================================");
  Serial.println("SERVO TEST - PIN 14");
  Serial.println("========================================");
  Serial.println("COMMANDS:");
  Serial.println("  F = Move to 45°");
  Serial.println("  B = Move to 180°");
  Serial.println("  0-9 = Custom angles (0°=0, 9=180°)");
  Serial.println("========================================");
  Serial.print("Current Position: ");
  Serial.println(currentPos);
}

void loop() {
  // Listen from Bluetooth
  if (SerialBT.available()) {
    char cmd = SerialBT.read();
    processServoCommand(cmd);
  }
  
  // Listen from Serial
  if (Serial.available()) {
    char cmd = Serial.read();
    processServoCommand(cmd);
  }
}

void processServoCommand(char cmd) {
  cmd = toupper(cmd);
  
  if (cmd == 'F') {
    // Move to 45°
    testServo.write(45);
    currentPos = 45;
    Serial.println("SERVO → 45°");
    SerialBT.println("SERVO 45");
    delay(500);
  }
  else if (cmd == 'B') {
    // Move to 180°
    testServo.write(180);
    currentPos = 180;
    Serial.println("SERVO → 180°");
    SerialBT.println("SERVO 180");
    delay(500);
  }
  else if (cmd >= '0' && cmd <= '9') {
    // Custom angle: 0-9 maps to 0-180 degrees
    int angle = (cmd - '0') * 20;  // 0*20=0°, 9*20=180°
    testServo.write(angle);
    currentPos = angle;
    Serial.print("SERVO → ");
    Serial.print(angle);
    Serial.println("°");
    delay(500);
  }
}
