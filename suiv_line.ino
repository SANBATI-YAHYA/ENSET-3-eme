/*********************************************************************
 * ESP32 · L298N · QTR (6 Sensors: 0=Left … 5=Right)
 * LINE FOLLOWER — PID + Spin Turn
 *********************************************************************/
#include <QTRSensors.h>

/* ---------- QTR (6 sensors) ---------- */
/* Use ADC1 pins for ESP32 (safe with WiFi) */
QTRSensors qtr;
const uint8_t qtrPins[6] = {36, 39, 34, 35, 32, 33};  // 0=Left ... 5=Right
uint16_t qVal[6];

/* ---------- Motors (YOUR NEW PINS) ---------- */
// Left motor
const int IN1_L = 18;
const int IN2_L = 19;
const int EN_L  = 23;   // PWM (ENA)

// Right motor
const int IN1_R = 21;   // IN3
const int IN2_R = 22;   // IN4
const int EN_R  = 5;    // PWM (ENB)

/* ---------- PID gains ---------- */
float   Kp = 0.28 , Ki = 0.000 , Kd = 5.5;
int16_t basePWM     = 120;
int16_t maxCorrPWM  = 120;

/* ---------- PWM thresholds ---------- */
const int16_t MIN_RUN_PWM = 35;
const int16_t SPIN_PWM    = 200;
const float   BACK_RATIO  = 1.00;

/* ---------- Turn detection ---------- */
const uint16_t BLACK_TH = 650;

/* ---------- PID state ---------- */
int16_t err = 0 , prevErr = 0;
long integ = 0;
const long I_CLAMP = 900;

/* ---------- PWM config ESP32 ---------- */
const int PWM_FREQ = 20000;
const int PWM_RES  = 8;   // 0..255

/* ---------- Motor helpers ---------- */
inline void side(int enPin, int in1, int in2, int16_t p){
  bool fwd = p >= 0;
  uint8_t pwm = abs(p);

  if (pwm == 0){
    ledcWrite(enPin, 0);   // ESP32 core 3.x style
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
  } else {
    if (pwm < MIN_RUN_PWM) pwm = MIN_RUN_PWM;
    digitalWrite(in1, fwd);
    digitalWrite(in2, !fwd);
    ledcWrite(enPin, pwm);
  }
}

inline void setMotor(int16_t L, int16_t R){
  side(EN_L, IN1_L, IN2_L, L);
  side(EN_R, IN1_R, IN2_R, R);
}

void setup() {
  Serial.begin(115200);

  pinMode(IN1_L, OUTPUT); pinMode(IN2_L, OUTPUT);
  pinMode(IN1_R, OUTPUT); pinMode(IN2_R, OUTPUT);

  // PWM attach
  ledcAttach(EN_L, PWM_FREQ, PWM_RES);
  ledcAttach(EN_R, PWM_FREQ, PWM_RES);

  // QTR init (6 sensors)
  qtr.setTypeAnalog();
  qtr.setSensorPins(qtrPins, 6);

  Serial.println("Calibrating QTR...");
  for (uint16_t i = 0; i < 400; i++) {
    qtr.calibrate();
    delay(5);
  }
  Serial.println("Calibration done");
}

void loop() {
  uint16_t pos = qtr.readLineBlack(qVal);  // 0..5000 (for 6 sensors)

  bool allWhite = true;
  for (uint8_t i = 0; i < 6; i++) {
    if (qVal[i] < 900) {
      allWhite = false;
      break;
    }
  }

  if (allWhite) {
    setMotor(basePWM, -basePWM * BACK_RATIO);
    integ = prevErr = 0;
    return;
  }

  bool leftEdge   = (qVal[0] < BLACK_TH) && (qVal[1] < BLACK_TH);
  bool rightEdge  = (qVal[5] < BLACK_TH) && (qVal[4] < BLACK_TH);
  bool centerSeen = (qVal[2] < BLACK_TH) || (qVal[3] < BLACK_TH);

  if (leftEdge && !rightEdge && !centerSeen) {
    setMotor(-SPIN_PWM * BACK_RATIO, SPIN_PWM);
    integ = prevErr = 0;
    return;
  }
  if (rightEdge && !leftEdge && !centerSeen) {
    setMotor(SPIN_PWM, -SPIN_PWM * BACK_RATIO);
    integ = prevErr = 0;
    return;
  }

  // PID (same logic)
  err   = (int16_t)pos - 2500;   // center for 6 sensors
  integ = constrain(integ + err, -I_CLAMP, I_CLAMP);
  int16_t deriv = err - prevErr;
  prevErr = err;

  float corr = Kp * err + Ki * integ + Kd * deriv;
  corr = constrain(corr, -maxCorrPWM, maxCorrPWM);

  int16_t L = constrain(basePWM + corr, 0, 255);
  int16_t R = constrain(basePWM - corr, 0, 255);
  setMotor(L, R);
}