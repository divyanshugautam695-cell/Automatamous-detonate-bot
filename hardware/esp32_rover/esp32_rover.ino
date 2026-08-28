/*
 * TACTIS-VR - Safe Real Rover Controller
 *
 * Physical behavior is NON-DESTRUCTIVE:
 * SAFE_EVENT => stop motors + buzzer + status LED.
 * There is no detonation/ignition output in this firmware.
 *
 * Arduino IDE target: ESP32 DevKit V1
 *
 * Initial hardware:
 *   GPIO25 -> Motor driver IN1
 *   GPIO26 -> Motor driver IN2
 *   GPIO27 -> Motor driver IN3
 *   GPIO14 -> Motor driver IN4
 *   GPIO13 -> active buzzer (or transistor driver for larger buzzer)
 *   GPIO2  -> status LED
 *
 * IMPORTANT:
 * - Use a proper motor driver; never drive motors directly from ESP32 GPIOs.
 * - Use a hardware emergency-stop that cuts motor power.
 * - Verify supply voltages before connecting the ESP32.
 */

#include <WiFi.h>
#include <WebServer.h>

const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

WebServer server(80);

constexpr uint8_t IN1 = 25;
constexpr uint8_t IN2 = 26;
constexpr uint8_t IN3 = 27;
constexpr uint8_t IN4 = 14;
constexpr uint8_t BUZZER = 13;
constexpr uint8_t STATUS_LED = 2;

bool softwareStop = false;
unsigned long lastCommandMs = 0;
constexpr unsigned long COMMAND_TIMEOUT_MS = 1500;

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void backward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void left() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void right() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void safeEvent() {
  softwareStop = true;
  stopMotors();
  digitalWrite(STATUS_LED, HIGH);

  // Safe physical indication only: short buzzer pattern.
  for (int i = 0; i < 3; ++i) {
    digitalWrite(BUZZER, HIGH);
    delay(180);
    digitalWrite(BUZZER, LOW);
    delay(120);
  }
}

void handleCommand() {
  if (!server.hasArg("cmd")) {
    server.send(400, "text/plain", "Missing cmd");
    return;
  }

  String cmd = server.arg("cmd");
  cmd.toUpperCase();
  lastCommandMs = millis();

  if (cmd == "SAFE_EVENT") {
    safeEvent();
    server.send(200, "text/plain", "SAFE_EVENT: STOP+BEEP+LED");
    return;
  }

  if (cmd == "STOP" || cmd == "ESTOP") {
    softwareStop = true;
    stopMotors();
    server.send(200, "text/plain", "STOPPED");
    return;
  }

  if (cmd == "RESUME") {
    softwareStop = false;
    digitalWrite(STATUS_LED, LOW);
    server.send(200, "text/plain", "READY");
    return;
  }

  if (softwareStop) {
    server.send(423, "text/plain", "LOCKED: send RESUME");
    return;
  }

  if (cmd == "FORWARD") forward();
  else if (cmd == "BACK") backward();
  else if (cmd == "LEFT") left();
  else if (cmd == "RIGHT") right();
  else {
    stopMotors();
    server.send(400, "text/plain", "Unknown command");
    return;
  }

  server.send(200, "text/plain", "OK");
}

void handleStatus() {
  String json = "{\"wifi\":" + String(WiFi.status() == WL_CONNECTED ? "true" : "false") +
                ",\"ip\":\"" + WiFi.localIP().toString() +
                "\",\"stopped\":" + String(softwareStop ? "true" : "false") + "}";
  server.send(200, "application/json", json);
}

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);

  stopMotors();
  digitalWrite(BUZZER, LOW);
  digitalWrite(STATUS_LED, LOW);

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
  }

  Serial.println();
  Serial.print("ROVER IP: ");
  Serial.println(WiFi.localIP());

  server.on("/command", HTTP_GET, handleCommand);
  server.on("/status", HTTP_GET, handleStatus);
  server.begin();

  lastCommandMs = millis();
}

void loop() {
  server.handleClient();

  // Communication failsafe: stop if commands stop arriving.
  if (!softwareStop && millis() - lastCommandMs > COMMAND_TIMEOUT_MS) {
    stopMotors();
  }
}
