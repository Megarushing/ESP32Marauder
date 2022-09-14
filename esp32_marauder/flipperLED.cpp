#include "flipperLED.h"

void flipperLED::flashLED() {
  digitalWrite(R_PIN, LOW);
  delay(500);
  digitalWrite(R_PIN, HIGH);
  delay(500);
  digitalWrite(R_PIN, LOW);
}

void flipperLED::RunSetup() {
  pinMode(R_PIN, OUTPUT);

  delay(50);
  flashLED();
}

void flipperLED::attackLED() {
  flashLED();
}

void flipperLED::sniffLED() {
  flashLED();
}

void flipperLED::offLED() {
  digitalWrite(R_PIN, HIGH);
  delay(10);
  digitalWrite(R_PIN, LOW);
}
