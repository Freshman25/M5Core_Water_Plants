#include <M5Core2.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#define PUMP_PIN 32 // This was written on the back of M5Stack lol
#define SENSOR_PIN 33   // written here: http://docs.m5stack.com/en/unit/watering

// 26: 700, 36: 0, 34: 0, 33: 2545, 32: 2600, 

static int32_t w;
static int32_t h;

Button button1(60, 120, 90, 90);
Button button2(180, 120, 90, 90);

bool flag = false;
int rawADC;

void setup()
{
    M5.begin();
    M5.Lcd.setTextColor(GREEN);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setTextDatum(TC_DATUM);
    
    M5.Lcd.setCursor(70, 30);
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
    digitalWrite(PUMP_PIN, 0);

    pinMode(SENSOR_PIN, INPUT);    
    pinMode(33, OUTPUT);
}

char info[30];

void loop()
{
    M5.update();
    rawADC = analogRead(SENSOR_PIN);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setCursor(70, 50);
    M5.Lcd.fillRect(70, 50, 200, 30, BLACK); // Adjust width and height as needed
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

