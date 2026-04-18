#include <BluetoothSerial.h>
#include <QTRSensors.h>
#include <ESP32Servo.h>

// ============= BLUETOOTH SETUP =============
BluetoothSerial SerialBT;
const char *BT_NAME = "N20_Beast_ESP32";

// ============= MOTOR PINS =============
const int IN1_L = 18;
const int IN2_L = 19;
const int EN_L  = 23;

const int IN1_R = 21;
const int IN2_R = 22;
const int EN_R  = 5;

// ============= SERVO CONTROL =============
Servo controlServo;
const int SERVO_PIN = 14;
int servoState = 0;  // 0 = 45°, 1 = 180°

// ============= SENSOR PINS (LINE FOLLOWER) =============
#define QTR_PIN_0  36
#define QTR_PIN_1  39
#define QTR_PIN_2  34
#define QTR_PIN_3  35
#define QTR_PIN_4  32

// ============= ULTRASONIC SENSOR PINS =============
const int TRIG_PIN = 13;  // Trigger pin
const int ECHO_PIN = 12;  // Echo pin

// ============= PWM SETTINGS =============
const uint32_t PWM_FREQ = 5000;
const uint8_t PWM_RESOLUTION = 8;

// ============= TELEGUIDE MODE VARIABLES =============
int currentSpeed = 200;

// ============= LINE FOLLOWER MODE VARIABLES =============
QTRSensors qtr;
uint16_t sensor_values[5];

#define BASE_SPEED      105
#define SPIN_SPEED      120
#define MIN_SPEED       30

#define Kp              0.35
#define Ki              0.00
#define Kd              5.7

#define BLACK_THRESHOLD 650
#define WHITE_THRESHOLD 900

int16_t error = 0;
int16_t prev_error = 0;
long integral = 0;
#define INTEGRAL_MAX 900

// ============= MODE SELECTOR =============
bool teleguideMode = false;  // false = Line Follower, true = Teleguide

// ============= SETUP =============
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  SerialBT.begin(BT_NAME);
  Serial.println("========================================");
  Serial.println("Bluetooth Ready - Connected Full Time!");
  Serial.println("========================================");
  Serial.println("COMMANDS:");
  Serial.println("  W     = TOGGLE Teleguide/Line Follower");
  Serial.println("  X     = TOGGLE Servo (45° ↔ 180°)");
  Serial.println("  F/B/L/R/S = Move (Teleguide mode)");
  Serial.println("  0-9   = Speed (Teleguide mode)");
  Serial.println("SENSORS:");
  Serial.println("  ✓ QTR Sensors (Line Following)");
  Serial.println("  ✓ Ultrasonic Sensor (Distance - Pin 13/12)");
  Serial.println("========================================");

  // Motor pins setup
  pinMode(IN1_L, OUTPUT);
  pinMode(IN2_L, OUTPUT);
  pinMode(IN1_R, OUTPUT);
  pinMode(IN2_R, OUTPUT);

  ledcAttach(EN_L, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(EN_R, PWM_FREQ, PWM_RESOLUTION);

  stopMotors();

  // Initialize Ultrasonic Sensor pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Initialize Servo on pin 14
  controlServo.attach(SERVO_PIN);
  controlServo.write(45);  // Start at 45 degrees
  delay(500);

  // Initialize QTR sensors for line follower
  uint8_t qtr_pins[5] = {QTR_PIN_0, QTR_PIN_1, QTR_PIN_2, QTR_PIN_3, QTR_PIN_4};
  qtr.setTypeAnalog();
  qtr.setSensorPins(qtr_pins, 5);

  for (int i = 0; i < 400; i++) {
    qtr.calibrate();
    delay(5);
  }
  delay(2000);
  
  Serial.println("Setup complete!");
}

// ============= MOTOR CONTROL FUNCTIONS =============
void stopMotors() {
  digitalWrite(IN1_L, LOW);
  digitalWrite(IN2_L, LOW);
  digitalWrite(IN1_R, LOW);
  digitalWrite(IN2_R, LOW);
  setMotorsSpeed(0);
}

void setMotorsSpeed(int speed) {
  ledcWrite(EN_L, speed);
  ledcWrite(EN_R, speed);
}

void moveForward() {
  digitalWrite(IN1_L, HIGH);
  digitalWrite(IN2_L, LOW);
  digitalWrite(IN1_R, HIGH);
  digitalWrite(IN2_R, LOW);
  setMotorsSpeed(currentSpeed);
}

void moveBackward() {
  digitalWrite(IN1_L, LOW);
  digitalWrite(IN2_L, HIGH);
  digitalWrite(IN1_R, LOW);
  digitalWrite(IN2_R, HIGH);
  setMotorsSpeed(currentSpeed);
}

void turnLeft() {
  digitalWrite(IN1_L, LOW);
  digitalWrite(IN2_L, HIGH);
  digitalWrite(IN1_R, HIGH);
  digitalWrite(IN2_R, LOW);
  setMotorsSpeed(currentSpeed);
}

void turnRight() {
  digitalWrite(IN1_L, HIGH);
  digitalWrite(IN2_L, LOW);
  digitalWrite(IN1_R, LOW);
  digitalWrite(IN2_R, HIGH);
  setMotorsSpeed(currentSpeed);
}

// ============= LINE FOLLOWER MOTOR CONTROL =============
void drive_motor(int enable_pin, int pin1, int pin2, int16_t speed) {
  bool forward = (speed >= 0);
  uint8_t pwm = abs(speed);

  if (pwm == 0) {
    ledcWrite(enable_pin, 0);
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
    return;
  }

  if (pwm < MIN_SPEED) pwm = MIN_SPEED;

  digitalWrite(pin1, forward);
  digitalWrite(pin2, !forward);
  ledcWrite(enable_pin, pwm);
}

void drive(int16_t left_speed, int16_t right_speed) {
  drive_motor(EN_L, IN1_L, IN2_L, left_speed);
  drive_motor(EN_R, IN1_R, IN2_R, right_speed);
}

// ============= TELEGUIDE MODE: PROCESS BLUETOOTH COMMANDS =============
void processCommand(char command) {
  command = toupper(command);

  // Teleguide movement commands only
  if (command == 'F') moveForward();
  else if (command == 'B') moveBackward();
  else if (command == 'L') turnLeft();
  else if (command == 'R') turnRight();
  else if (command == 'S') stopMotors();
  else if (command >= '0' && command <= '9') {
    currentSpeed = (command - '0') * 25;
    if (currentSpeed > 255) currentSpeed = 255;
  }
}

// ============= ULTRASONIC SENSOR: READ DISTANCE =============
long getDistance() {
  // Send trigger pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Measure echo time
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // Timeout: 30ms
  
  // Calculate distance (duration / 2) / 29.1 cm/microsecond
  long distance = duration / 58;  // Simplified: cm = duration / 58
  
  return distance;
}

// ============= LINE FOLLOWER MODE: MAIN LOGIC =============
void lineFollowerLoop() {
  uint16_t line_pos = qtr.readLineBlack(sensor_values);

  bool all_white = true;
  for (int i = 0; i < 5; i++) {
    if (sensor_values[i] < WHITE_THRESHOLD) {
      all_white = false;
      break;
    }
  }

  // Read distance every 10 loops
  static int loopCount = 0;
  loopCount++;
  if (loopCount >= 10) {
    long distance = getDistance();
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    SerialBT.print("DIST:");
    SerialBT.println(distance);
    loopCount = 0;
  }

  if (all_white) {
    drive(BASE_SPEED, -BASE_SPEED);
    integral = prev_error = 0;
    return;
  }

  bool left_black = (sensor_values[0] < BLACK_THRESHOLD) && (sensor_values[1] < BLACK_THRESHOLD);
  bool right_black = (sensor_values[3] < BLACK_THRESHOLD) && (sensor_values[4] < BLACK_THRESHOLD);
  bool center_black = (sensor_values[2] < BLACK_THRESHOLD);

  if (left_black && !right_black && !center_black) {
    drive(-SPIN_SPEED, SPIN_SPEED);
    integral = prev_error = 0;
    return;
  }

  if (right_black && !left_black && !center_black) {
    drive(SPIN_SPEED, -SPIN_SPEED);
    integral = prev_error = 0;
    return;
  }

  error = (int16_t)line_pos - 2000;
  integral = constrain(integral + error, -INTEGRAL_MAX, INTEGRAL_MAX);
  int16_t derivative = error - prev_error;
  prev_error = error;

  float correction = (Kp * error) + (Ki * integral) + (Kd * derivative);
  correction = constrain(correction, -80, 80);

  int16_t left_speed = constrain(BASE_SPEED + correction, 0, 255);
  int16_t right_speed = constrain(BASE_SPEED - correction, 0, 255);

  drive(left_speed, right_speed);
}

// ============= MAIN LOOP =============
void loop() {
  // ALWAYS listen to Bluetooth for commands
  if (SerialBT.available()) {
    char receivedChar = SerialBT.read();
    
    // ============ SERVO TOGGLE (X = Toggle 45° ↔ 180°) ============
    if (receivedChar == 'X' || receivedChar == 'x') {
      if (servoState == 0) {
        // Switch to 180°
        controlServo.write(180);
        servoState = 1;
        Serial.println(">>> SERVO → 180° <<<");
        SerialBT.println("SERVO 180");
      } else {
        // Switch to 45°
        controlServo.write(45);
        servoState = 0;
        Serial.println(">>> SERVO → 45° <<<");
        SerialBT.println("SERVO 45");
      }
      delay(300);
      return;
    }
    
    // ============ MODE TOGGLE (W = Toggle Teleguide ↔ Line Follower) ============
    if (receivedChar == 'W' || receivedChar == 'w') {
      teleguideMode = !teleguideMode;
      stopMotors();
      if (teleguideMode) {
        Serial.println(">>> SWITCHED TO TELEGUIDE MODE <<<");
      } else {
        Serial.println(">>> SWITCHED TO LINE FOLLOWER MODE <<<");
      }
      return;
    }
    
    // If in Teleguide mode, process movement commands
    if (teleguideMode) {
      processCommand(receivedChar);
    }
  }

  // Check for commands from Serial as well
  if (Serial.available()) {
    char receivedChar = Serial.read();
    if (receivedChar == 'X' || receivedChar == 'x') {
      if (servoState == 0) {
        controlServo.write(180);
        servoState = 1;
        Serial.println(">>> SERVO → 180° <<<");
      } else {
        controlServo.write(45);
        servoState = 0;
        Serial.println(">>> SERVO → 45° <<<");
      }
      delay(300);
      return;
    }
    if (receivedChar == 'W' || receivedChar == 'w') {
      teleguideMode = !teleguideMode;
      stopMotors();
      if (teleguideMode) {
        Serial.println(">>> SWITCHED TO TELEGUIDE MODE <<<");
      } else {
        Serial.println(">>> SWITCHED TO LINE FOLLOWER MODE <<<");
      }
      return;
    }
  }

  // Run the active mode
  if (teleguideMode) {
    // Teleguide mode: Wait for Bluetooth commands (handled above)
    delay(10);  // Small delay to prevent high CPU usage
  } else {
    // Line follower mode: Run continuously
    lineFollowerLoop();
  }
}
