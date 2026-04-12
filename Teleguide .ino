/*********************************************************************
 * ESP32 · L298N
 * WIFI TELEGUIDE ONLY — Forward/Backward/Left/Right/Stop + Gripper
 *********************************************************************/
#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);

/* ---------- WiFi ---------- */
const char *SSID     = "EAGLES";
const char *PASSWORD = "EAGLES06";

/* ---------- Gripper Output ---------- */
const int CATCH_PIN = 27;
bool isCaught = false;

/* ---------- Motors ---------- */
const int IN1_L = 18;
const int IN2_L = 19;
const int EN_L  = 23;

const int IN1_R = 21;
const int IN2_R = 22;
const int EN_R  = 5;

/* ---------- Motor speed ---------- */
int16_t motorSpeed = 200;  // 0..255

/* ---------- Motor control ---------- */
void setMotorRaw(int enPin, int in1, int in2, int16_t pwm) {
  if (pwm == 0) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    analogWrite(enPin, 0);
  } else {
    bool fwd = pwm >= 0;
    uint8_t duty = abs(pwm);
    if (duty > 255) duty = 255;
    digitalWrite(in1, fwd);
    digitalWrite(in2, !fwd);
    analogWrite(enPin, duty);
  }
}

void setMotors(int16_t L, int16_t R) {
  setMotorRaw(EN_L, IN1_L, IN2_L, L);
  setMotorRaw(EN_R, IN1_R, IN2_R, R);
}

/* ---------- Movement commands ---------- */
void moveForward() {
  setMotors(motorSpeed, motorSpeed);
  Serial.println("→ FORWARD");
}

void moveBackward() {
  setMotors(-motorSpeed, -motorSpeed);
  Serial.println("← BACKWARD");
}

void turnLeft() {
  setMotors(-motorSpeed, motorSpeed);
  Serial.println("↺ LEFT");
}

void turnRight() {
  setMotors(motorSpeed, -motorSpeed);
  Serial.println("↻ RIGHT");
}

void stopRobot() {
  setMotors(0, 0);
  Serial.println("⏹ STOP");
}

/* ---------- HTTP handlers ---------- */
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 Robot Teleguide</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body {
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      min-height: 100vh;
      display: flex;
      justify-content: center;
      align-items: center;
      padding: 20px;
    }
    .container {
      background: white;
      border-radius: 20px;
      box-shadow: 0 20px 60px rgba(0, 0, 0, 0.3);
      padding: 40px;
      max-width: 500px;
      width: 100%;
    }
    h1 {
      text-align: center;
      color: #333;
      margin-bottom: 10px;
      font-size: 32px;
    }
    .subtitle {
      text-align: center;
      color: #666;
      margin-bottom: 30px;
      font-size: 14px;
    }
    
    .control-pad {
      display: grid;
      grid-template-columns: repeat(3, 1fr);
      gap: 15px;
      margin-bottom: 30px;
    }
    .btn {
      padding: 25px;
      font-size: 18px;
      font-weight: bold;
      border: none;
      border-radius: 15px;
      cursor: pointer;
      transition: all 0.3s ease;
      color: white;
      text-transform: uppercase;
      letter-spacing: 1px;
      box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2);
    }
    .btn:hover {
      transform: scale(1.08);
      box-shadow: 0 8px 25px rgba(0, 0, 0, 0.3);
    }
    .btn:active {
      transform: scale(0.95);
    }
    .btn-forward { grid-column: 2; background: #4CAF50; }
    .btn-left { grid-column: 1; background: #2196F3; }
    .btn-stop { grid-column: 2; background: #f44336; grid-row: 3; }
    .btn-right { grid-column: 3; background: #2196F3; }
    .btn-backward { grid-column: 2; background: #FF9800; }
    
    .gripper-section {
      border-top: 3px solid #ddd;
      padding-top: 25px;
      margin-top: 25px;
    }
    .gripper-title {
      text-align: center;
      color: #333;
      margin-bottom: 15px;
      font-size: 18px;
      font-weight: bold;
    }
    .gripper-status {
      text-align: center;
      padding: 12px;
      background: #f0f0f0;
      border-radius: 10px;
      margin-bottom: 15px;
      font-weight: bold;
      color: #333;
      font-size: 16px;
    }
    .gripper-buttons {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 15px;
    }
    .btn-catch { background: #00c853; }
    .btn-release { background: #d50000; }

    .speed-control {
      margin-top: 25px;
      border-top: 3px solid #ddd;
      padding-top: 25px;
    }
    .speed-label {
      text-align: center;
      color: #333;
      margin-bottom: 15px;
      font-weight: bold;
      font-size: 16px;
    }
    .speed-slider {
      width: 100%;
      height: 8px;
      border-radius: 5px;
      background: #ddd;
      outline: none;
      -webkit-appearance: none;
    }
    .speed-slider::-webkit-slider-thumb {
      -webkit-appearance: none;
      appearance: none;
      width: 30px;
      height: 30px;
      border-radius: 50%;
      background: #667eea;
      cursor: pointer;
      box-shadow: 0 2px 8px rgba(0, 0, 0, 0.2);
    }
    .speed-slider::-moz-range-thumb {
      width: 30px;
      height: 30px;
      border-radius: 50%;
      background: #667eea;
      cursor: pointer;
      border: none;
      box-shadow: 0 2px 8px rgba(0, 0, 0, 0.2);
    }
    .speed-value {
      text-align: center;
      margin-top: 12px;
      color: #667eea;
      font-weight: bold;
      font-size: 20px;
    }

    .info-box {
      margin-top: 25px;
      padding: 15px;
      background: #e3f2fd;
      border-left: 4px solid #2196F3;
      border-radius: 5px;
      color: #333;
      font-size: 13px;
      line-height: 1.6;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>🤖 Robot Control</h1>
    <div class="subtitle">WiFi Teleguide</div>
    
    <div class="control-pad">
      <button class="btn btn-forward" onmousedown="fetch('/fwd')" onmouseup="fetch('/stop')" ontouchstart="fetch('/fwd')" ontouchend="fetch('/stop')">⬆ FWD</button>
      
      <button class="btn btn-left" onmousedown="fetch('/left')" onmouseup="fetch('/stop')" ontouchstart="fetch('/left')" ontouchend="fetch('/stop')">⬅ LEFT</button>
      
      <button class="btn btn-stop" onclick="fetch('/stop')">⏹ STOP</button>
      
      <button class="btn btn-right" onmousedown="fetch('/right')" onmouseup="fetch('/stop')" ontouchstart="fetch('/right')" ontouchend="fetch('/stop')">RIGHT ➡</button>
      
      <button class="btn btn-backward" onmousedown="fetch('/bwd')" onmouseup="fetch('/stop')" ontouchstart="fetch('/bwd')" ontouchend="fetch('/stop')">⬇ BWD</button>
    </div>

    <div class="speed-control">
      <div class="speed-label">⚡ Motor Speed</div>
      <input type="range" class="speed-slider" id="speedSlider" min="50" max="255" value="200" 
             onchange="setSpeed(this.value)" oninput="updateSpeedDisplay(this.value)">
      <div class="speed-value" id="speedValue">200</div>
    </div>

    <div class="gripper-section">
      <div class="gripper-title">🔲 Gripper Control</div>
      <div class="gripper-status" id="gripperStatus">RELEASED</div>
      <div class="gripper-buttons">
        <button class="btn btn-catch" onclick="fetch('/catch')">CATCH</button>
        <button class="btn btn-release" onclick="fetch('/release')">RELEASE</button>
      </div>
    </div>

    <div class="info-box">
      📱 <b>Controls:</b><br>
      • Press & hold arrow buttons to move<br>
      • Release to stop<br>
      • Adjust speed slider for power<br>
      • Use gripper buttons anytime
    </div>
  </div>

  <script>
    function setSpeed(value) {
      fetch('/setspeed?value=' + value);
      updateSpeedDisplay(value);
    }

    function updateSpeedDisplay(value) {
      document.getElementById('speedValue').innerText = value;
    }

    // Update gripper status every 500ms
    setInterval(() => {
      fetch('/status').then(r => r.text()).then(status => {
        document.getElementById('gripperStatus').innerText = status;
      });
    }, 500);
  </script>
</body>
</html>)rawliteral";

  server.send(200, "text/html", html);
}

void handleFwd() {
  moveForward();
  server.send(200, "text/plain", "FWD");
}

void handleBwd() {
  moveBackward();
  server.send(200, "text/plain", "BWD");
}

void handleLeft() {
  turnLeft();
  server.send(200, "text/plain", "LEFT");
}

void handleRight() {
  turnRight();
  server.send(200, "text/plain", "RIGHT");
}

void handleStop() {
  stopRobot();
  server.send(200, "text/plain", "STOP");
}

void handleCatch() {
  isCaught = true;
  digitalWrite(CATCH_PIN, HIGH);
  server.send(200, "text/plain", "CATCHED");
  Serial.println("🔲 CATCH");
}

void handleRelease() {
  isCaught = false;
  digitalWrite(CATCH_PIN, LOW);
  server.send(200, "text/plain", "RELEASED");
  Serial.println("🔲 RELEASE");
}

void handleSetSpeed() {
  if (server.hasArg("value")) {
    motorSpeed = server.arg("value").toInt();
    motorSpeed = constrain(motorSpeed, 50, 255);
    Serial.println("Speed: " + String(motorSpeed));
    server.send(200, "text/plain", "Speed: " + String(motorSpeed));
  }
}

void handleStatus() {
  String status = isCaught ? "CATCHED" : "RELEASED";
  server.send(200, "text/plain", status);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n========== ESP32 TELEGUIDE ==========");

  // Gripper
  pinMode(CATCH_PIN, OUTPUT);
  digitalWrite(CATCH_PIN, LOW);

  // Motors
  pinMode(IN1_L, OUTPUT); 
  pinMode(IN2_L, OUTPUT);
  pinMode(IN1_R, OUTPUT); 
  pinMode(IN2_R, OUTPUT);
  pinMode(EN_L, OUTPUT);
  pinMode(EN_R, OUTPUT);

  stopRobot();
  Serial.println("Motors initialized");

  // WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  Serial.print("Connecting to WiFi");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WiFi connected!");
    Serial.print("📡 IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n✗ WiFi connection failed!");
  }

  // HTTP routes
  server.on("/", handleRoot);
  server.on("/fwd", handleFwd);
  server.on("/bwd", handleBwd);
  server.on("/left", handleLeft);
  server.on("/right", handleRight);
  server.on("/stop", handleStop);
  server.on("/catch", handleCatch);
  server.on("/release", handleRelease);
  server.on("/setspeed", handleSetSpeed);
  server.on("/status", handleStatus);

  server.begin();
  Serial.println("✓ HTTP server started on port 80");
  Serial.println("=====================================\n");
}

void loop() {
  server.handleClient();
  delay(10);
}