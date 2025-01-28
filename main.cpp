#include <M5Core2.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const int LINE_HEIGHT = 16;  // Line height for display

const char* ssid="";
const char* password=""; // WiFi password
WebServer server(80);


#define PUMP_PIN 32 // This was written on the back of M5Stack lol
#define SENSOR_PIN 33

static int32_t w;
static int32_t h;

Button button1(60, 120, 90, 90);
Button button2(180, 120, 90, 90);

bool flag = false;
int rawADC;


void handleRoot() {
    String message;

    message = "<!DOCTYPE html>";
    message += "<html>";
    message += "<head>";
    message += "<meta charset=\"utf-8\">";
    message += "<meta http-equiv=\"refresh\" content=\"1\">";
    message += "<title>Test</title>";
    message += "</head>";
    message += "<body style=\"background-color: #f0f0f0; width: 500px; height: 300px;\">";
    message += "<h1>Output from ENV IV UNIT</h1>";

    // Buttons with fixed dimensions
    message += "<form action=\"/control\" method=\"POST\">";
    message += "<button type=\"submit\" class=\"btn-info\" name=\"pump\" value=\"ON\" style=\"width: 150px; height: 50px; background-color:rgb(106, 133, 253); color: white;\">ON</button>";
    message += "<button type=\"submit\" class=\"btn-info\" name=\"pump\" value=\"OFF\" style=\"width: 150px; height: 50px; background-color:rgb(252, 237, 62); color: white; margin-left: 10px;\">OFF</button>";
    message += "</form>";

    message += "</body>";
    message += "</html>";

    server.send(200, "text/html", message);
}



void handleControl() {
    String action = server.arg("pump");
    if (action == "ON") {
        flag = true;
        digitalWrite(PUMP_PIN, flag);
    } else if (action == "OFF") {
        flag = false;
        digitalWrite(PUMP_PIN, flag);
    }

    server.sendHeader("Location", "/");
    server.send(303);  // HTTP for redirect
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

    if (!MDNS.begin("m5stack")) {   // Set hostname to "m5stack"
        M5.Lcd.println("Error setting up mDNS responder!");
        while (1) {
            delay(1000);  // Stay here if mDNS setup fails
        }
    }

    // -- server routing --
    server.on("/", handleRoot);
    server.on("/control", HTTP_POST, handleControl);

    server.begin();



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

    rawADC = analogRead(SENSOR_PIN);

    M5.Lcd.setCursor(70, 60);
    M5.Lcd.fillRect(70, 60, 200, 30, BLACK); // Adjust width and height as needed
    M5.Lcd.print(String("ADC: ") + String(rawADC));
    Serial.print("Watering ADC value: ");
    Serial.println(rawADC);

    // manually control
    if (M5.BtnB.wasPressed()) {
        digitalWrite(PUMP_PIN, flag);
        flag = !flag;
    }

    if (button1.wasPressed()){
      flag = true;
      digitalWrite(PUMP_PIN, flag);
    }
    if (button2.wasPressed()){
      flag = false;
      digitalWrite(PUMP_PIN, flag);
    }

    M5.update();
    delay(100);
}
