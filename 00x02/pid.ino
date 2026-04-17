#include <QTRSensors.h>

#define IN1_LEFT   18
#define IN2_LEFT   19
#define EN_LEFT    23

#define IN1_RIGHT  21
#define IN2_RIGHT  22
#define EN_RIGHT   5

#define QTR_PIN_0  36
#define QTR_PIN_1  39
#define QTR_PIN_2  34
#define QTR_PIN_3  35
#define QTR_PIN_4  32

#define PWM_FREQUENCY   5000
#define PWM_RESOLUTION  8

#define BASE_SPEED      105
#define SPIN_SPEED      120
#define MIN_SPEED       30

#define Kp              0.35
#define Ki              0.00
#define Kd              5.7

#define BLACK_THRESHOLD 650
#define WHITE_THRESHOLD 900

QTRSensors qtr;
uint16_t sensor_values[5];

int16_t error = 0;
int16_t prev_error = 0;
long integral = 0;
#define INTEGRAL_MAX 900

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
  drive_motor(EN_LEFT, IN1_LEFT, IN2_LEFT, left_speed);
  drive_motor(EN_RIGHT, IN1_RIGHT, IN2_RIGHT, right_speed);
}

void stop_motors() {
  drive(0, 0);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(IN1_LEFT, OUTPUT);
  pinMode(IN2_LEFT, OUTPUT);
  pinMode(IN1_RIGHT, OUTPUT);
  pinMode(IN2_RIGHT, OUTPUT);

  ledcAttach(EN_LEFT, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttach(EN_RIGHT, PWM_FREQUENCY, PWM_RESOLUTION);

  stop_motors();

  uint8_t qtr_pins[5] = {QTR_PIN_0, QTR_PIN_1, QTR_PIN_2, QTR_PIN_3, QTR_PIN_4};
  qtr.setTypeAnalog();
  qtr.setSensorPins(qtr_pins, 5);

  for (int i = 0; i < 400; i++) {
    qtr.calibrate();
    delay(5);
  }
  delay(2000);
}

void loop() {
  uint16_t line_pos = qtr.readLineBlack(sensor_values);

  bool all_white = true;
  for (int i = 0; i < 5; i++) {
    if (sensor_values[i] < WHITE_THRESHOLD) {
      all_white = false;
      break;
    }
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