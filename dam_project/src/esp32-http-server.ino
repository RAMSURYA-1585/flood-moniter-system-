/*
  ESP32 HTTP Flood Monitoring System for Wokwi

  When running in Wokwi for VSCode, open http://localhost:8180
  in your browser to view the status page.
*/

#include <WiFi.h>
#include <WebServer.h>
#include <uri/UriBraces.h>

// WiFi credentials for Wokwi simulation
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#define WIFI_CHANNEL 6

// Pin assignments (adjust as per your Wokwi diagram)
#define TRIG_PIN    5    // HC-SR04 TRIG
#define ECHO_PIN    18   // HC-SR04 ECHO
#define GREEN_LED   26   // Safe status LED
#define RED_LED     27   // Alert status LED

#define FLOOD_THRESHOLD_CM  20  // Flood risk threshold (cm)

WebServer server(80);

float currentWaterLevel = 0;
bool floodAlert = false;

void sendHtml() {
  String statusText = (currentWaterLevel < 0) ? "Sensor Error" :
                      (currentWaterLevel < FLOOD_THRESHOLD_CM) ? "ALERT: Flood risk!" : "Normal";
  String alertColor = (currentWaterLevel < 0) ? "#888" :
                      (currentWaterLevel < FLOOD_THRESHOLD_CM) ? "#c00" : "#090";

  String response = R"(
    <!DOCTYPE html><html>
      <head>
        <title>ESP32 Flood Monitoring</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <style>
          html { font-family: sans-serif; text-align: center; }
          body { margin: 2em; }
          h1 { margin-bottom: 1em; }
          .status { font-size: 2em; font-weight: bold; color: ALERT_COLOR; }
          .level { font-size: 1.5em; margin: 1em 0; }
          .led { display: inline-block; width: 1em; height: 1em; border-radius: 50%; margin: 0 0.5em; }
          .green { background: #0f0; border: 1px solid #090; }
          .red { background: #f00; border: 1px solid #900; }
        </style>
      </head>
      <body>
        <h1>Flood Monitoring System</h1>
        <div class="level">Water Level: <b>LEVEL_CM</b> cm</div>
        <div class="status" style="color: ALERT_COLOR;">STATUS_TEXT</div>
        <div>
          <span class="led green" style="opacity:GREEN_OPACITY;"></span> Green (Safe)
          <span class="led red" style="opacity:RED_OPACITY;"></span> Red (Alert)
        </div>
        <div style="margin-top:2em; font-size:0.9em; color:#888;">
          Last updated: TIME ms since boot
        </div>
      </body>
    </html>
  )";

  response.replace("LEVEL_CM", (currentWaterLevel < 0) ? "--" : String(currentWaterLevel, 2));
  response.replace("STATUS_TEXT", statusText);
  response.replace("ALERT_COLOR", alertColor);
  response.replace("GREEN_OPACITY", (currentWaterLevel >= FLOOD_THRESHOLD_CM) ? "1" : "0.2");
  response.replace("RED_OPACITY", (currentWaterLevel < FLOOD_THRESHOLD_CM && currentWaterLevel >= 0) ? "1" : "0.2");
  response.replace("TIME", String(millis()));

  server.send(200, "text/html", response);
}

float readWaterLevelCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout
  if (duration == 0) return -1; // Sensor error
  float distance = duration * 0.034 / 2.0;
  return distance;
}

void updateIndicators(float level) {
  if (level < 0) {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, LOW);
    floodAlert = false;
    return;
  }

  if (level < FLOOD_THRESHOLD_CM) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    floodAlert = true;
  } else {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    floodAlert = false;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  Serial.print("Connecting to WiFi ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", sendHtml);
  server.begin();
  Serial.println("HTTP server started (http://localhost:8180)");
}

unsigned long lastRead = 0;
const unsigned long readInterval = 3000; // 3 seconds

void loop() {
  server.handleClient();

  unsigned long now = millis();
  if (now - lastRead > readInterval) {
    currentWaterLevel = readWaterLevelCM();
    updateIndicators(currentWaterLevel);

    if (currentWaterLevel < 0) {
      Serial.println("Sensor error: No echo received.");
    } else {
      Serial.print("Water Level (cm): ");
      Serial.println(currentWaterLevel);
      if (floodAlert) {
        Serial.println("ALERT: Flood risk!");
      } else {
        Serial.println("Water level normal.");
      }
    }
    lastRead = now;
  }
}
