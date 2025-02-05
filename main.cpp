#include <M5Core2.h>
#include <WiFi.h>
#include <WebServer.h>
#include "time.h"
#include <ctime>
#include <ESPmDNS.h>

const int LINE_HEIGHT = 16;  // Line height for display

const char* ssid="";
const char* password=""; // WiFi password
WebServer server(80);

const char* ntpServer = "pool.ntp.org"; // ntp server for time
const long  gmtOffset_sec = 9 * 3600;
const int   daylightOffset_sec = 0;

#define PUMP_PIN 32 // This was written on the back of M5Stack lol
#define SENSOR_PIN 35 // INPUT

static int32_t w;
static int32_t h;
time_t timestamp;
bool manualOverride;

Button button1(60, 120, 90, 90);
Button button2(180, 120, 90, 90);

bool flag = false;
int rawADC;

void handleRoot() {
    String message;
    int percentage;
    int adcValue = rawADC;

    // Calculate percentage (dry: 4095, in water: 2000)
    percentage = 100 - (int)((adcValue - 2000) * 100 / (4095 - 2000));

    // Ensure percentage is between 0 and 100
    if (percentage < 0) {
        percentage = 0;
    } else if (percentage > 100) {
        percentage = 100;
    }

    // Convert timestamp from NTP to readable format
    char timeStr[26];
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {  // Fetch current time
        strftime(timeStr, 26, "%Y-%m-%d %H:%M:%S", &timeinfo);  // Format timestamp
    } else {
        strcpy(timeStr, "Failed to get time");
    }

    // Use the timestamp updated only when the pump is turned on
    char lastPumpTime[26];
    if (timestamp > 0) {
        struct tm lastTime;
        localtime_r(&timestamp, &lastTime);
        strftime(lastPumpTime, sizeof(lastPumpTime), "%Y-%m-%d %H:%M:%S", &lastTime);
    } else {
        strcpy(lastPumpTime, "N/A");
    }

    // Determine color based on percentage
    String barColor;
    if (percentage <= 20) barColor = "red";
    else if (percentage <= 50) barColor = "yellow";
    else if (percentage <= 80) barColor = "green";
    else barColor = "blue";

    // Generate HTML content
    message = "<!DOCTYPE html>";
    message += "<html>";
    message += "<head>";
    message += "<meta charset=\"utf-8\">";
    message += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
    message += "<title>Barometer</title>";
    message += "<style>";
    message += "body { background-color: #f0f0f0; text-align: center; padding: 20px; font-family: Arial, sans-serif; }";
    message += "h1 { font-size: 24px; }";
    message += ".bar-container { width: 100%; height: 30px; background-color: #ddd; margin: 20px 0; }";
    message += ".bar { height: 100%; text-align: center; color: white; line-height: 30px; font-size: 16px; }";
    message += ".btn-info { width: 100%; height: 50px; color: white; border: none; font-size: 18px; margin-bottom: 10px; box-sizing: border-box; }";
    message += ".btn-on { background-color: rgb(106, 133, 253); }";
    message += ".btn-off { background-color: rgb(252, 237, 62); }";
    message += "@media (min-width: 600px) {";
    message += ".btn-info { width: 150px; display: inline-block; margin: 0 10px; }";
    message += "}";
    message += "</style>";
    message += "<script>";
    message += "function updateSensorData() {";
    message += "  fetch('/getSensorData')";
    message += "    .then(response => response.json())";
    message += "    .then(data => {";
    message += "      document.getElementById('adc-value').innerText = 'ADC Value: ' + data.adcValue;";
    message += "      document.getElementById('moisture-percentage').innerText = 'Moisture Percentage: ' + data.percentage + '%';";
    message += "      var bar = document.getElementById('moisture-bar');";
    message += "      bar.style.width = data.percentage + '%';";
    message += "      bar.style.backgroundColor = data.barColor;";
    message += "    });";
    message += "}";
    message += "setInterval(updateSensorData, 2000);";
    message += "window.onload = updateSensorData;";
    message += "</script>";
    message += "</head>";
    message += "<body>";
    message += "<h1>Moisture Level Barometer</h1>";
    message += "<h3>Parsley</h3>";
    message += "<h5>Petroselinum cripsum</h5>";
    message += "<img width=\"256\" alt=\"ParsleyStem\" src=\"https://upload.wikimedia.org/wikipedia/commons/thumb/6/6a/Parsley_Curled.jpg/512px-Parsley_Curled.jpg?20050427131012\">";
    message += "<p id='adc-value'>ADC Value: " + String(adcValue) + "</p>";
    message += "<p id='moisture-percentage'>Moisture Percentage: " + String(percentage) + "%</p>";
    message += "<p>Last Pump Activation: " + String(lastPumpTime) + "</p>";  // Display timestamp only when updated

    // Barometer visualization (colored bar)
    message += "<div class=\"bar-container\">";
    message += "<div id='moisture-bar' class=\"bar\" style=\"width: " + String(percentage) + "%; background-color: " + barColor + ";\">";
    message += "</div>";
    message += "</div>";

    // Control buttons
    message += "<form action=\"/control\" method=\"POST\">";
    message += "<button type=\"submit\" class=\"btn-info btn-on\" name=\"pump\" value=\"ON\">ON</button>";
    message += "<button type=\"submit\" class=\"btn-info btn-off\" name=\"pump\" value=\"OFF\">OFF</button>";
    message += "</form>";
    message += "</body>";
    message += "</html>";

    server.send(200, "text/html", message);
}



void handleControl() {
    String action = server.arg("pump");
    if (action == "ON") {
        if (!flag) {  // Only update timestamp if the pump was previously OFF
            flag = true;
            time(&timestamp);  // Update the timestamp when the pump is activated
        }
        digitalWrite(PUMP_PIN, flag);
    } else if (action == "OFF") {
        flag = false;
        digitalWrite(PUMP_PIN, flag);
    }

    server.sendHeader("Location", "/");
    server.send(303);  // HTTP for redirect
}



void handleGetSensorData() {
    // Read ADC value each time this route is called
    rawADC = analogRead(SENSOR_PIN);  // Read the sensor value

    String barColor;
    int percentage = 100 - (int)((rawADC - 2000) * 100 / (4095 - 2000));

    if (percentage < 0) percentage = 0;
    if (percentage > 100) percentage = 100;

    if (percentage <= 20) barColor = "red";
    else if (percentage <= 50) barColor = "yellow";
    else if (percentage <= 80) barColor = "green";
    else barColor = "blue";

    String response = "{\"adcValue\": " + String(rawADC) + ", \"percentage\": " + String(percentage) + ", \"barColor\": \"" + barColor + "\"}";
    server.send(200, "application/json", response);  // Return updated data as JSON
}


void setup() {
    M5.begin();
    delay(10);

    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE);

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.print("Connecting to WiFi...");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        M5.Lcd.print(".");
    }
    
    M5.Lcd.fillRect(0, 0, 500, 500, BLACK);
    M5.Lcd.setCursor(70, 20);
    M5.Lcd.print("IP: ");
    M5.Lcd.printf("%s\n\n", WiFi.localIP().toString().c_str());

    // NTP initialization
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    
    // Wait for time to sync
    int retries = 0;
    struct tm timeinfo;
    while (!getLocalTime(&timeinfo) && retries < 10) {
        M5.Lcd.print("Waiting for NTP time sync...");
        delay(1000);
        retries++;
    }

    if (!MDNS.begin("m5stack")) {   // Set hostname to "m5stack"
        M5.Lcd.println("Error setting up mDNS responder!");
        while (1) {
            delay(1000);  // Stay here if mDNS setup fails
        }
    }

    // -- server routing --
    server.on("/", handleRoot);
    server.on("/control", HTTP_POST, handleControl);
    server.on("/getSensorData", HTTP_GET, handleGetSensorData);

    server.begin();
    server.handleClient();

    // M5 stack input
    M5.Lcd.setCursor(70, 40);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.print("Watering v1.1");

    M5.Lcd.fillRect(60, 120, 90, 90, 0x84FE);
    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(2);    
    M5.Lcd.setCursor(95, 155);
    M5.Lcd.print("ON");
    
    M5.Lcd.fillRect(180, 120, 90, 90, 0xFD80);
    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(210, 155);
    M5.Lcd.print("OFF");

    pinMode(PUMP_PIN, OUTPUT);
    digitalWrite(PUMP_PIN, 0);    // let voltage 0

    pinMode(SENSOR_PIN, INPUT);    
    pinMode(33, OUTPUT);
}

void loop() {
    M5.update();
    
    M5.Lcd.setTextColor(WHITE);
    server.handleClient();

    // Read the ADC value to calculate moisture percentage
    rawADC = analogRead(SENSOR_PIN);
    int percentage = 100 - (int)((rawADC - 2000) * 100 / (4095 - 2000));

    // Display the ADC and percentage on the screen
    M5.Lcd.setCursor(70, 60);
    M5.Lcd.fillRect(70, 60, 200, 30, BLACK);
    M5.Lcd.print(String("ADC: ") + String(rawADC));

    Serial.print("Watering ADC value: ");
    Serial.println(rawADC);

    // Manually control the pump using button presses
    if (M5.BtnB.wasPressed()) {
        digitalWrite(PUMP_PIN, flag);
        flag = !flag;
    }

    if (button1.wasPressed()) {
        flag = true;
        digitalWrite(PUMP_PIN, flag);
    }

    if (button2.wasPressed()) {
        flag = false;
        digitalWrite(PUMP_PIN, flag);
        manualOverride = true; // Set flag to indicate manual override
    }

    // Automatic turning on, but respecting the manual override
    if (percentage < 30) {
        if (!flag && !manualOverride) {  // Only turn on automatically if no manual override
            flag = true;
            digitalWrite(PUMP_PIN, HIGH);
            time(&timestamp);
        }
    } else {
        if (flag) {
            flag = false;
            digitalWrite(PUMP_PIN, LOW);
        }
    }

    // Check if the soil moisture percentage is high enough to reset the manual override
    if (percentage >= 30) {
        manualOverride = false; // Reset manual override once soil moisture is sufficient
    }

    M5.update();
    delay(100);  // Delay to prevent overwhelming the system with updates
}
