#include "TemperatureInterface.h"

#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();

TemperatureInterface::TemperatureInterface() {
  
}

void TemperatureInterface::RunSetup() {
  this->initTime = millis();
  this->current_temp = this->getCurrentTemp();
}

uint8_t TemperatureInterface::getCurrentTemp() {
  #ifndef MARAUDER_FLIPPER
    return ((temprature_sens_read() - 32) / 1.8);
  #endif
    return 0;
}

void TemperatureInterface::main(uint32_t currentTime) {
  if (currentTime != 0) {
    if (currentTime - initTime >= 100) {
      //MegaSerial.println("Checking Battery Level");
      this->initTime = millis();
      this->current_temp = this->getCurrentTemp();
    }
  }
}
