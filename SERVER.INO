#include <WiFi.h>
#include <WebServer.h>

// --- Server Configuration ---
const char* ssid = "MRNC_FACTORY";      // SSID defined in the Cahier de Charge
const char* password = "password123";   
const String token = "ABC123XYZ";       // Your secret team token

// The server listens on Port 5000
WebServer server(5000);

void setup() {
  Serial.begin(115200);

  // 1. Initialize the Access Point
  WiFi.softAP(ssid, password);
  
  Serial.println("\n==========================================");
  Serial.println("   MRNC 2026 CENTRAL FACTORY SERVER       ");
  Serial.println("==========================================");
  Serial.print("Server IP: "); 
  Serial.println(WiFi.softAPIP()); // Usually 192.168.4.1
  Serial.println("Status: Listening for Robot Commands...");

  // --- MISSION: CHECK-IN (The Start Handshake) ---
  // The robot stays still until this returns "SUCCESS"
  server.on("/CHECK/" + token, []() {
    Serial.println("[SYSTEM] Robot Handshake: AUTHORIZED. (Mission Start)");
    server.send(200, "text/plain", "SUCCESS");
  });

  // --- MISSION: TABLE ROTATIVE (The Circle) ---
  server.on("/ROTATIVE/" + token, []() {
    Serial.println("[ACTION] Rotating Circle... Servo Turned!"); 
    server.send(200, "text/plain", "SUCCESS");
  });

  // --- MISSION: GATE CONTROL ---
  server.on("/GATE/OPEN/" + token, []() {
    Serial.println("[ACTION] Factory Gate: Opening Barrier...");
    server.send(200, "text/plain", "SUCCESS");
  });

  // --- MISSION: ELEVATOR / LIFT ---
  server.on("/ELEVATOR/" + token, []() {
    Serial.println("[ACTION] Elevator: Moving to Level 2...");
    server.send(200, "text/plain", "ACTIVATED");
  });

  // --- CUSTOM ALERT: Wall Warnings ---
  server.on("/DIST/NEAR/" + token, []() {
    Serial.println("ROBOT ALERT: 9rib! (Object Detected)");
    server.send(200, "text/plain", "NEAR_ACK");
  });

  server.on("/DIST/FAR/" + token, []() {
    Serial.println("ROBOT STATUS: b3iiiiid. (Path Clear)");
    server.send(200, "text/plain", "FAR_ACK");
  });

  // --- ERROR HANDLING (Diagnostic) ---
  server.onNotFound([]() {
    String errorMsg = "INVALID REQUEST: " + server.uri();
    Serial.println("!!! " + errorMsg + " !!!");
    server.send(404, "text/plain", "FAILURE");
  });

  server.begin();
}

void loop() {
  // Constantly check for incoming robot requests
  server.handleClient();
}