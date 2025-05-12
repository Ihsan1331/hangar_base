#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <U8g2lib.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// RFID pins
#define SS_PIN 10
#define RST_PIN 9

String lastUID = "";
unsigned long lastScanTime = 0;
const unsigned long scanCooldown = 2000; // 2 seconds cooldown

// Create MFRC522 instance
MFRC522 rfid(SS_PIN, RST_PIN);

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
bool armorStatus   = true;
bool weaponsLoaded = true;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  // — Initialize hardware-I²C LCD once —  
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("HANGAR STATUS:");
  lcd.setCursor(0,1);
  lcd.print("INACTIVE");

  // — Initialize software-I²C OLED once —  
  u8g2.begin();
  u8g2.setFont(u8g2_font_5x8_tr);

  // Initialize DFPlayer Mini
  Serial.begin(115200);
  dfSerial.begin(9600);
  delay(200);  // Give the DFPlayer a moment to power up

  Serial.print("Initializing DFPlayer…");
  if (!dfplayer.begin(dfSerial)) {
    Serial.println(" FAIL (no ACK)");
    while (1);  // Stop here if it still won’t respond
  }
  Serial.println(" OK");

  dfplayer.volume(25);
  
  delay(100);

  do {
    // Title
    u8g2.drawStr(10, 12, "[HANGAR 01]");

    // Status message
    u8g2.drawStr(10, 28, "STATUS: EMPTY");
    u8g2.drawStr(10, 40, "NO UNIT PRESENT");
    u8g2.drawStr(10, 52, "STANDBY MODE...");

    // Optional box outline to imply a bay
    u8g2.drawFrame(90, 20, 30, 30);
    u8g2.drawStr(92, 38, "EMPTY");

  } while (u8g2.nextPage());
}

void loop() {
  // 1) Read RFID
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return;

  // 2) Construct UID string
  String uidStr;
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uidStr += "0";
    uidStr += String(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) uidStr += ":";
  }
  uidStr.toUpperCase();

  // 3) Debounce same card
  if (uidStr == lastUID && millis() - lastScanTime < scanCooldown) {
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    return;
  }
  lastUID = uidStr;
  lastScanTime = millis();

  // —— ONLY SERIAL ——  
  Serial.print("Card UID: ");
  Serial.println(uidStr);
  // ——————————————

  // 4) Now your existing hangar logic:
  if (uidStr == "C3:FF:37:17") {
    // — LCD: Gundam maintenance —  
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("GAT-X105+AQM/E-YM1");
    lcd.setCursor(0,1);
    lcd.print("MAINTENANCE");

    // — OLED: maintenance page —  
    u8g2.firstPage();
    do {
      u8g2.drawStr(10,11,"[HANGAR 01] STRIKE GUNDAM");
      u8g2.drawStr(10,22,"STATUS: MAINTENANCE");
      drawEnergyBar(energy);
      u8g2.drawStr(10,48, armorStatus   ? "ARMOR: LOADED"       : "ARMOR: DAMAGED");
      u8g2.drawStr(10,59, weaponsLoaded ? "WEAPONS: LOADED" : "WEAPONS: UNLOADED");
    } while (u8g2.nextPage());

    // Play audio
    Serial.println("Playing track 1…");
    dfplayer.play(1);  // Make sure you’ve named it 0001.mp3 in the root
  } 
  else if (uidStr == "8C:4B:F6:37") {
    // — LCD: back to inactive —  
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("HANGAR STATUS:");
    lcd.setCursor(0,1);
    lcd.print("INACTIVE");

    // OLED: empty hangar
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

  // 5) Cleanup
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
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
