#include <SPI.h>
#include <Wire.h>
#include <RFID.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define sda 10
#define rst 9

RFID rfid(sda, rst);

void showLocked() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LOCKED");
  lcd.setCursor(0, 1);
  lcd.print("Scan a card");
}

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.setBacklight(255);

  pinMode(sda, OUTPUT);
  digitalWrite(sda, HIGH);
  pinMode(rst, OUTPUT);
  digitalWrite(rst, LOW);
  delay(50);
  digitalWrite(rst, HIGH);
  delay(50);

  SPI.begin();
  rfid.init();
  delay(100);
  rfid.antennaOn();

  byte readerVersion = rfid.readMFRC522(VersionReg);
  Serial.print(F("MFRC522 version: 0x"));
  Serial.println(readerVersion, HEX);
  if (readerVersion == 0x00 || readerVersion == 0xFF) {
    Serial.println(F("Reader SPI check failed; inspect reader wiring and power"));
  } else {
    Serial.println(F("Reader SPI communication OK"));
  }
  Serial.println(F("Ready - scan a card"));
  showLocked();
}

void loop() {
  if (!rfid.isCard()) {
    static unsigned long lastWaitingMessage = 0;
    if (millis() - lastWaitingMessage >= 2000) {
      Serial.println(F("Waiting - no card detected"));
      lastWaitingMessage = millis();
    }
    return;
  }

  Serial.println(F("Card detected"));
  if (!rfid.readCardSerial()) {
    Serial.println(F("UID read failed"));
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("READ FAILED");
    lcd.setCursor(0, 1);
    lcd.print("Try again");
    rfid.halt();
    delay(1500);
    showLocked();
    return;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ACCESS GRANTED");

  Serial.print(F("Card UID: "));
  for (byte i = 0; i < 4; i++) {
    if (i > 0) {
      Serial.print(" ");
    }
    if (rfid.serNum[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(rfid.serNum[i], HEX);
  }
  Serial.println();

  rfid.halt();
  delay(3000);
  showLocked();
}