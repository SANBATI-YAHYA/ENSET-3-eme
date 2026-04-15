#include <BluetoothSerial.h>
#include <ESP32Servo.h>

BluetoothSerial SerialBT;
const char *BT_NAME = "ESP32_Robot";

// Servos
Servo stillServo;      // Servo 1 - GPIO27
Servo movingServo;     // Servo 2 - GPIO14

int stillPos = 0;      // Still servo position (0-180) - starts at 0
int movingPos = 0;     // Moving servo position (0-180) - starts at 0


const int IN1_L = 18;
const int IN2_L = 19;
const int EN_L = 23;

const int IN1_R = 21;
const int IN2_R = 22;
const int EN_R = 5;

const uint32_t PWM_FREQ = 5000;
const uint8_t PWM_RESOLUTION = 8;

int speedValue = 200;
char receivedChar = 'S';
char mode = 'w';

void setup() {
  Serial.begin(115200);
  delay(1000);
 

  // Attach servos
  stillServo.attach(27);
  movingServo.attach(14);

  // Initial positions (0 degrees)
  stillServo.write(0);
  movingServo.write(0);


  SerialBT.begin(BT_NAME);

  pinMode(IN1_L, OUTPUT);
  pinMode(IN2_L, OUTPUT);
  pinMode(IN1_R, OUTPUT);
  pinMode(IN2_R, OUTPUT);

  ledcAttach(EN_L, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(EN_R, PWM_FREQ, PWM_RESOLUTION);

  stopMotors();
}

void moveForward() {
  digitalWrite(IN1_L, HIGH);
  digitalWrite(IN2_L, LOW);
  ledcWrite(EN_L, speedValue);

  digitalWrite(IN1_R, HIGH);
  digitalWrite(IN2_R, LOW);
  ledcWrite(EN_R, speedValue);
}

void moveBackward() {
  digitalWrite(IN1_L, LOW);
  digitalWrite(IN2_L, HIGH);
  ledcWrite(EN_L, speedValue);

  digitalWrite(IN1_R, LOW);
  digitalWrite(IN2_R, HIGH);
  ledcWrite(EN_R, speedValue);
}

void turnLeft() {
  digitalWrite(IN1_L, LOW);
  digitalWrite(IN2_L, HIGH);
  ledcWrite(EN_L, speedValue);

  digitalWrite(IN1_R, HIGH);
  digitalWrite(IN2_R, LOW);
  ledcWrite(EN_R, speedValue);
}

void turnRight() {
  digitalWrite(IN1_L, HIGH);
  digitalWrite(IN2_L, LOW);
  ledcWrite(EN_L, speedValue);

  digitalWrite(IN1_R, LOW);
  digitalWrite(IN2_R, HIGH);
  ledcWrite(EN_R, speedValue);
}

void diagonalForwardLeft() {
  digitalWrite(IN1_L, HIGH);
  digitalWrite(IN2_L, LOW);
  ledcWrite(EN_L, 100);

  digitalWrite(IN1_R, HIGH);
  digitalWrite(IN2_R, LOW);
  ledcWrite(EN_R, 200);
}

void diagonalForwardRight() {
  digitalWrite(IN1_L, HIGH);
  digitalWrite(IN2_L, LOW);
  ledcWrite(EN_L, 200);

  digitalWrite(IN1_R, HIGH);
  digitalWrite(IN2_R, LOW);
  ledcWrite(EN_R, 100);
}

void diagonalBackwardLeft() {
  digitalWrite(IN1_L, LOW);
  digitalWrite(IN2_L, HIGH);
  ledcWrite(EN_L, 100);

  digitalWrite(IN1_R, LOW);
  digitalWrite(IN2_R, HIGH);
  ledcWrite(EN_R, 200);
}

void diagonalBackwardRight() {
  digitalWrite(IN1_L, LOW);
  digitalWrite(IN2_L, HIGH);
  ledcWrite(EN_L, 200);

  digitalWrite(IN1_R, LOW);
  digitalWrite(IN2_R, HIGH);
  ledcWrite(EN_R, 100);
}

void stopMotors() {
  digitalWrite(IN1_L, LOW);
  digitalWrite(IN2_L, LOW);
  digitalWrite(IN1_R, LOW);
  digitalWrite(IN2_R, LOW);

  ledcWrite(EN_L, 0);
  ledcWrite(EN_R, 0);
}

void setSpeed(int speed) {
  speedValue = constrain(speed, 0, 255);
}

void processCommand(char command) {
  command = toupper(command);

  switch (command) {
    case 'F':
      moveForward();
      break;
    case 'B':
      moveBackward();
      break;
    case 'L':
      turnLeft();
      break;
    case 'R':
      turnRight();
      break;
    case 'I':
      stillServo.write(90);
      stillPos = 90;
      break;
    case 'G':
      movingServo.write(90);
      movingPos = 90;
      break;
    case 'J':
      stillServo.write(0);
      stillPos = 0;
      break;
    case 'H':
      movingServo.write(0);
      movingPos = 0;  
      break;
    case 'S':
      stopMotors();
      break;
    case 'W':
      mode = 'W';
      break;
  }
}

void loop() {
  if (SerialBT.available()) {
    receivedChar = SerialBT.read();
    processCommand(receivedChar);
  }

  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');

    if (input.length() > 0 && isDigit(input[0])) {
      int newSpeed = input.toInt();
      setSpeed(newSpeed);
    } else if (input.length() > 0) {
      processCommand(input[0]);
    }
  }
}