#include <M5Core2.h>

#define PUMP_PIN 32 // Pump control pin
#define SENSOR_PIN 35 // Soil moisture sensor pin

Button button1(60, 120, 90, 90); // ON button
Button button2(180, 120, 90, 90); // OFF button

bool pumpState = false;
int rawADC;

void setup() {
    M5.begin();
    pinMode(PUMP_PIN, OUTPUT);
    digitalWrite(PUMP_PIN, LOW); // Ensure pump is off at startup
    pinMode(SENSOR_PIN, INPUT);

    // Draw ON/OFF Button
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
}

void loop() {
    M5.update();
    rawADC = analogRead(SENSOR_PIN);

    // Display sensor reading
    M5.Lcd.setCursor(0, 20);
    M5.Lcd.setTextColor(WHITE);    
    M5.Lcd.print("Pump: Press ON/OFF"); // test pump by pressing buttons

    M5.Lcd.setCursor(0, 40);
    M5.Lcd.print("Earth sensor: Touch sensor"); // touch the sensor (if you cannot see anything with whole hand on the sensor your sensor does not work as expected)

    M5.Lcd.setCursor(0, 60);
    M5.Lcd.fillRect(0, 60, 200, 30, BLACK);
    M5.Lcd.print(String("ADC: ") + String(rawADC));

    if (button1.wasPressed()) {
        pumpState = true;
        digitalWrite(PUMP_PIN, HIGH);
    }

    if (button2.wasPressed()) {
        pumpState = false;
        digitalWrite(PUMP_PIN, LOW);
    }
    
    delay(100);
}
