#include "esp_interface.h"

HardwareSerial MyMegaSerial(1);

void EspInterface::begin() {
  pinMode(ESP_RST, OUTPUT);
  pinMode(ESP_ZERO, OUTPUT);

  delay(100);

  digitalWrite(ESP_ZERO, HIGH);

  MegaSerial.println("Checking for ESP8266...");

  MyMegaSerial.begin(BAUD, SERIAL_8N1, 39, 37);

  delay(100);

  #ifdef HAS_SCREEN
    display_obj.tft.println("Checking for ESP8266...");
  #endif

  this->bootRunMode();

  delay(500);

  while (MyMegaSerial.available())
    MyMegaSerial.read();

  MyMegaSerial.write("PING");

  delay(2000);

  String display_string = "";

  while (MyMegaSerial.available()) {
    display_string.concat((char)MyMegaSerial.read());
  }

  display_string.trim();

  MegaSerial.println("\nDisplay string: " + (String)display_string);

  if (display_string == "ESP8266 Pong") {
    #ifdef HAS_SCREEN
      display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
      display_obj.tft.println("ESP8266 Found!");
      display_obj.tft.setTextColor(TFT_CYAN, TFT_BLACK);
    #endif
    MegaSerial.println("ESP8266 Found!");
    this->supported = true;
  }
  else {
    #ifdef HAS_SCREEN
      display_obj.tft.setTextColor(TFT_RED, TFT_BLACK);
      display_obj.tft.println("ESP8266 Not Found");
      display_obj.tft.setTextColor(TFT_CYAN, TFT_BLACK);
    #endif
  }

  this->initTime = millis();
}

void EspInterface::RunUpdate() {
  this->bootProgramMode();
  #ifdef HAS_SCREEN
    display_obj.tft.setTextWrap(true);
    display_obj.tft.setFreeFont(NULL);
    display_obj.tft.setCursor(0, 100);
    display_obj.tft.setTextSize(1);
    display_obj.tft.setTextColor(TFT_GREEN);

    display_obj.tft.println("Waiting for serial data...");

    display_obj.tft.setTextColor(TFT_WHITE);
  #endif
}

void EspInterface::bootProgramMode() {
  Serial.println("[!] Setting ESP12 in program mode...");
  digitalWrite(ESP_ZERO, LOW);
  delay(100);
  digitalWrite(ESP_RST, LOW);
  delay(100);
  digitalWrite(ESP_RST, HIGH);
  delay(100);
  digitalWrite(ESP_ZERO, HIGH);
  Serial.println("[!] Complete");
  Serial.end();
  Serial.begin(57600);
}

void EspInterface::bootRunMode() {
  Serial.end();
  Serial.begin(115200);
  Serial.println("[!] Setting ESP12 in run mode...");
  digitalWrite(ESP_ZERO, HIGH);
  delay(100);
  digitalWrite(ESP_RST, LOW);
  delay(100);
  digitalWrite(ESP_RST, HIGH);
  delay(100);
  digitalWrite(ESP_ZERO, HIGH);
  Serial.println("[!] Complete");
}

void EspInterface::program() {
  if (MyMegaSerial.available()) {
    MegaSerial.write((uint8_t)MyMegaSerial.read());
  }

  if (MegaSerial.available()) {
    #ifdef HAS_SCREEN
      display_obj.tft.print(".");
    #endif
    while (MegaSerial.available()) {
      MyMegaSerial.write((uint8_t)MegaSerial.read());
    }
  }
}

void EspInterface::main(uint32_t current_time) {
  if (current_time - this->initTime >= 1000) {
    this->initTime = millis();
    //MyMegaSerial.write("PING");
  }

  while (MyMegaSerial.available()) {
    MegaSerial.print((char)MyMegaSerial.read());
  }

  if (MegaSerial.available()) {
    MyMegaSerial.write((uint8_t)MegaSerial.read());
  }
}
