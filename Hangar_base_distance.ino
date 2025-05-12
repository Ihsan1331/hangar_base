#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <U8g2lib.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <NewPing.h>

// HC-SR04 Pins
#define TRIG_PIN 8
#define ECHO_PIN 7
#define MAX_DISTANCE 100 // Maximum distance to measure in cm

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

const unsigned int presenceThreshold = 7; // cm, adjust as needed

// LCD on the *hardware* I²C bus (A4=A, A5=SCL on Uno)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// OLED on *software* I²C: clock=D3, data=D2
U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(
  U8G2_R0,
  /* clock=*/ 3,
  /* data=*/ 2,
  /* reset=*/ U8X8_PIN_NONE
);

// DFPlayer Mini pins (using software serial)
#define DF_RX_PIN 5  // ← DFPlayer TX
#define DF_TX_PIN 6  // → DFPlayer RX

SoftwareSerial dfSerial(DF_RX_PIN, DF_TX_PIN);
DFRobotDFPlayerMini dfplayer;
DFRobotDFPlayerMini myDFPlayer;

// System status
int energy = 78;
bool armorStatus = true;
bool weaponsLoaded = true;

// Timing variables
unsigned long previousMillis = 0;  // Timer for 45 seconds
unsigned long previousMillisShort = 0;  // Timer for 1 second
const long intervalLong = 75000;  // 45 seconds in milliseconds
const long intervalShort = 1000;  // 1 second in milliseconds

bool songPlaying = false;  // Flag to track if song is playing

void setup() {
  Serial.begin(9600);

  // Initialize hardware-I²C LCD
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("HANGAR STATUS:");
  lcd.setCursor(0, 1);
  lcd.print("INACTIVE");

  // Initialize software-I²C OLED
  u8g2.begin();
  u8g2.setFont(u8g2_font_5x8_tr);

  // Initialize DFPlayer Mini
  dfSerial.begin(9600);
  delay(200);  // Give the DFPlayer a moment to power up

  Serial.print("Initializing DFPlayer…");
  if (!dfplayer.begin(dfSerial)) {
    Serial.println(" FAIL (no ACK)");
    while (1);  // Stop here if it still won’t respond
  }
  Serial.println(" OK");

  dfplayer.volume(15);
  delay(100);

  // Initial OLED splash
  do {
    u8g2.drawStr(10, 12, "[HANGAR 01]");
    u8g2.drawStr(10, 28, "STATUS: EMPTY");
    u8g2.drawStr(10, 40, "NO UNIT PRESENT");
    u8g2.drawStr(10, 52, "STANDBY MODE...");
    u8g2.drawFrame(90, 20, 30, 30);
    u8g2.drawStr(92, 38, "EMPTY");
  } while (u8g2.nextPage());
}

void loop() {
  delay(200); // Wait for sensor to stabilize

  unsigned int distance = sonar.ping_cm(); // Get distance from the sensor

  // Debug print to see the measured distance
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  unsigned long currentMillis = millis();  // Current time

  // Check if Gunpla is detected
  if (distance > 0 && distance < presenceThreshold) {
    // Gunpla detected: Play the song for 45 seconds
    if (!songPlaying) {
      songPlaying = true;
      dfplayer.play(3);  // Play track 1 (ensure 0001.mp3 is in root folder)
      previousMillis = currentMillis;  // Start the 45-second timer
    }

    // Update display continuously while song is playing
    updateDisplays();
  }

  // If song is playing (for Gunpla detection), wait for 45 seconds before updating
  if (songPlaying) {
    if (currentMillis - previousMillis >= intervalLong) {
      // After 45 seconds, stop playing the song
      songPlaying = false;
      dfplayer.stop();  // Stop song
      previousMillisShort = currentMillis;  // Start fast updates again
    }
  } else {
    // No Gunpla detected: Update every second
    if (currentMillis - previousMillisShort >= intervalShort) {
      previousMillisShort = currentMillis;

      // Update LCD for no Gunpla detected
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("HANGAR STATUS:");
      lcd.setCursor(0, 1);
      lcd.print("INACTIVE");

      // Update OLED for no Gunpla detected
      u8g2.firstPage();
      do {
        u8g2.drawStr(10, 12, "[HANGAR 01]");
        u8g2.drawStr(10, 28, "STATUS: EMPTY");
        u8g2.drawStr(10, 40, "NO UNIT PRESENT");
        u8g2.drawStr(10, 52, "STANDBY MODE...");
        u8g2.drawFrame(90, 20, 30, 30);
        u8g2.drawStr(92, 38, "EMPTY");
      } while (u8g2.nextPage());
    }
  }
}

// Function to update both LCD and OLED displays
void updateDisplays() {
  // Update LCD for Gunpla detected
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("GAT-X105+AQM/E-YM1");
  lcd.setCursor(3, 1);
  lcd.print("MAINTENANCE");

  // Update OLED for Gunpla detected
  u8g2.firstPage();
  do {
    u8g2.drawStr(10, 11, "[HANGAR 01] STRIKE ");
    u8g2.drawStr(10, 22, "STATUS: MAINTENANCE");
    drawEnergyBar(energy);
    u8g2.drawStr(10, 48, armorStatus ? "ARMOR: LOADED" : "ARMOR: DAMAGED");
    u8g2.drawStr(10, 59, weaponsLoaded ? "WEAPONS: LOADED" : "WEAPONS: UNLOADED");
  } while (u8g2.nextPage());
}

void drawEnergyBar(int level) {
  // Frame
  u8g2.drawFrame(10, 34, 100, 5);
  // Filled
  int w = map(level, 0, 100, 0, 100);
  u8g2.drawBox(10, 34, w, 5);
  // Label
  char buf[12];
  sprintf(buf, "ENERGY: %d%%", level);
  u8g2.drawStr(10, 31, buf);
}
