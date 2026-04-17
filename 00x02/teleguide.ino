#include <BluetoothSerial.h>

BluetoothSerial SerialBT;
const char *BT_NAME = "N20_Beast_ESP32";

const int IN1_L = 18;
const int IN2_L = 19;
const int EN_L  = 23;

const int IN1_R = 21;
const int IN2_R = 22;
const int EN_R  = 5;

const uint32_t PWM_FREQ = 5000;
const uint8_t PWM_RESOLUTION = 8;
int currentSpeed = 200;

void setup() {
  Serial.begin(115200);
  
  SerialBT.begin(BT_NAME);
  Serial.println("Bluetooth is ready! You can pair now.");

  pinMode(IN1_L, OUTPUT);
  pinMode(IN2_L, OUTPUT);
  pinMode(IN1_R, OUTPUT);
  pinMode(IN2_R, OUTPUT);

  ledcAttach(EN_L, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(EN_R, PWM_FREQ, PWM_RESOLUTION);

  stopMotors();
}

void moveForward() {
  digitalWrite(IN1_L, HIGH); digitalWrite(IN2_L, LOW);
  digitalWrite(IN1_R, HIGH); digitalWrite(IN2_R, LOW);
  setMotorsSpeed(currentSpeed);
}

void moveBackward() {
  digitalWrite(IN1_L, LOW); digitalWrite(IN2_L, HIGH);
  digitalWrite(IN1_R, LOW); digitalWrite(IN2_R, HIGH);
  setMotorsSpeed(currentSpeed);
}

void turnLeft() {
  digitalWrite(IN1_L, LOW);  digitalWrite(IN2_L, HIGH);
  digitalWrite(IN1_R, HIGH); digitalWrite(IN2_R, LOW);
  setMotorsSpeed(currentSpeed);
}

void turnRight() {
  digitalWrite(IN1_L, HIGH); digitalWrite(IN2_L, LOW);
  digitalWrite(IN1_R, LOW);  digitalWrite(IN2_R, HIGH);
  setMotorsSpeed(currentSpeed);
}

void stopMotors() {
  digitalWrite(IN1_L, LOW); digitalWrite(IN2_L, LOW);
  digitalWrite(IN1_R, LOW); digitalWrite(IN2_R, LOW);
  setMotorsSpeed(0);
}

void setMotorsSpeed(int speed) {
  ledcWrite(EN_L, speed);
  ledcWrite(EN_R, speed);
}

void processCommand(char command) {
  command = toupper(command); 

  switch (command) {
    case 'F': moveForward();  break;
    case 'B': moveBackward(); break;
    case 'L': turnLeft();     break;
    case 'R': turnRight();    break;
    case 'S': stopMotors();   break;
    
    case '0': currentSpeed = 0;   break;
    case '1': currentSpeed = 25;  break;
    case '2': currentSpeed = 50;  break;
    case '3': currentSpeed = 75;  break;
    case '4': currentSpeed = 100; break;
    case '5': currentSpeed = 125; break;
    case '6': currentSpeed = 150; break;
    case '7': currentSpeed = 175; break;
    case '8': currentSpeed = 200; break;
    case '9': currentSpeed = 255; break;
  }
}

void loop() {
  if (SerialBT.available()) {
    char receivedChar = SerialBT.read();
    processCommand(receivedChar);
  }

  if (Serial.available()) {
    char receivedChar = Serial.read();
    processCommand(receivedChar);
  }
}