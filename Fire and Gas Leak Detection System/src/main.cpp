#include <Arduino.h>

#define BLYNK_TEMPLATE_ID "TMPL6bvUvs3i8"
#define BLYNK_TEMPLATE_NAME "Fire and Gas Leak Detection System"
#define BLYNK_AUTH_TOKEN "E8VuovDSmuQev1bK9VHXXCdjPOm1BDxH"

#include <WiFi.h>

#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>

// WiFi credentials
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// Define pins
#define FLAME_SENSOR 34
#define GAS_SENSOR   35
#define BUZZER       12
#define LED          23

// Create LCD object (0x27 is the I2C address, 16x2 LCD)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(115200);

  // Start LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Starting...");

  // Connect to WiFi and Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Set pin modes
  pinMode(FLAME_SENSOR, INPUT);
  pinMode(GAS_SENSOR, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);

  delay(2000);
  lcd.clear();
}

void loop() {
  Blynk.run();

  int flameValue = analogRead(FLAME_SENSOR);
  int gasValue = analogRead(GAS_SENSOR);

  // Display on LCD
  lcd.setCursor(0, 0);
  lcd.print("Flame:");
  lcd.print(flameValue);
  lcd.setCursor(0, 1);
  lcd.print("Gas:  ");
  lcd.print(gasValue);
  Serial.print("Gas: ");
  Serial.println(gasValue);

  // Send to Blynk
  Blynk.virtualWrite(V2, flameValue);
  Blynk.virtualWrite(V3, gasValue);

  bool fireDetected = flameValue < 1000;
  bool gasDetected = gasValue > 500;

  if (fireDetected || gasDetected) {
    digitalWrite(BUZZER, HIGH);
    digitalWrite(LED, HIGH);
    Blynk.virtualWrite(V0, fireDetected);
    Blynk.virtualWrite(V1, gasDetected);

    Blynk.logEvent("alert", fireDetected ? "🔥 Fire Detected!" : "🛢️ Gas Leak Detected!");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("!! DANGER ALERT !!");
    delay(2000);
  } else {
    digitalWrite(BUZZER, LOW);
    digitalWrite(LED, LOW);
    Blynk.virtualWrite(V0, 0);
    Blynk.virtualWrite(V1, 0);
  }

  delay(1000);
}
