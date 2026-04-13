#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// WiFi
const char *SSID     = "EAGLES";
const char *PASSWORD = "EAGLES06";

// Servos
Servo stillServo;      // Catch - GPIO27
Servo movingServo;     // Movement - GPIO14

WebServer server(80);

int stillPos = 90;     // Still servo position (0-180)
int movingPos = 90;    // Moving servo position (0-180)

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Attach servos
  stillServo.attach(27);
  movingServo.attach(14);

  // Initial positions
  stillServo.write(90);
  movingServo.write(90);

  // Connect WiFi
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✓ WiFi Connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // Web routes
  server.on("/", handleDashboard);
  server.on("/still", handleStill);
  server.on("/moving", handleMoving);
  server.begin();
}

void loop() {
  server.handleClient();
}

// ===== Dashboard =====
void handleDashboard() {
  String html = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial; text-align: center; background: #222; color: #fff; padding: 20px; }
    .container { max-width: 500px; margin: 0 auto; }
    h1 { color: #0f0; }
    .servo { background: #333; padding: 20px; margin: 15px 0; border-radius: 10px; border: 2px solid #0f0; }
    button { padding: 12px 20px; margin: 8px; font-size: 16px; font-weight: bold; border: none; border-radius: 5px; cursor: pointer; }
    .btn-up { background: #0f0; color: #000; }
    .btn-down { background: #f00; color: #fff; }
    .btn-middle { background: #0099ff; color: #fff; }
    .status { font-size: 20px; margin: 10px 0; }
  </style>
</head>
<body>
  <div class="container">
    <h1>🎮 ESP32 Servo Control</h1>

    <!-- Still Servo (Catch) -->
    <div class="servo">
      <h2>Servo 1: Catch (GPIO27)</h2>
      <div class="status">Position: <span id="stillStatus">90</span>°</div>
      <button class="btn-down" onclick="setSero('still', 0)">0° (Drop)</button>
      <button class="btn-middle" onclick="setSero('still', 90)">90° (Middle)</button>
      <button class="btn-up" onclick="setSero('still', 180)">180° (Catch)</button>
    </div>

    <!-- Moving Servo -->
    <div class="servo">
      <h2>Servo 2: Movement (GPIO14)</h2>
      <div class="status">Position: <span id="movingStatus">90</span>°</div>
      <button class="btn-down" onclick="setSero('moving', 0)">⬇️ Down (0°)</button>
      <button class="btn-middle" onclick="setSero('moving', 90)">⏸️ Middle (90°)</button>
      <button class="btn-up" onclick="setSero('moving', 180)">⬆️ Up (180°)</button>
    </div>

    <p style="margin-top: 30px; color: #666;">WiFi: EAGLES | Connected ✓</p>
  </div>

  <script>
    function setSero(type, pos) {
      fetch('/' + type + '?pos=' + pos)
        .then(r => r.text())
        .then(data => {
          if (type === 'still') {
            document.getElementById('stillStatus').textContent = pos;
          } else {
            document.getElementById('movingStatus').textContent = pos;
          }
        });
    }

    // Update every 1 second
    setInterval(() => {
      fetch('/')
        .then(r => r.text())
        .then(html => {
          // Auto refresh status
        });
    }, 1000);
  </script>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

// Still Servo Control
void handleStill() {
  if (server.hasArg("pos")) {
    stillPos = server.arg("pos").toInt();
    if (stillPos >= 0 && stillPos <= 180) {
      stillServo.write(stillPos);
      Serial.print("Still Servo: ");
      Serial.println(stillPos);
    }
  }
  server.send(200, "text/plain", String(stillPos));
}

// Moving Servo Control
void handleMoving() {
  if (server.hasArg("pos")) {
    movingPos = server.arg("pos").toInt();
    if (movingPos >= 0 && movingPos <= 180) {
      movingServo.write(movingPos);
      Serial.print("Moving Servo: ");
      Serial.println(movingPos);
    }
  }
  server.send(200, "text/plain", String(movingPos));
}