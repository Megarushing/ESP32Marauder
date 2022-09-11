#include "SDInterface.h"
#include "lang_var.h"

bool SDInterface::initSD() {
  String display_string = "";

  #ifdef KIT
    pinMode(SD_DET, INPUT);
    if (digitalRead(SD_DET) == LOW) {
      MegaSerial.println(F("SD Card Detect Pin Detected"));
    }
    else {
      MegaSerial.println(F("SD Card Detect Pin Not Detected"));
      this->supported = false;
      return false;
    }
  #endif

  pinMode(SD_CS, OUTPUT);

  delay(10);
  
  if (!SD.begin(SD_CS)) {
    MegaSerial.println(F("Failed to mount SD Card"));
    this->supported = false;
    return false;
  }
  else {
    this->supported = true;
    this->cardType = SD.cardType();
    //if (cardType == CARD_MMC)
    //  MegaSerial.println(F("SD: MMC Mounted"));
    //else if(cardType == CARD_SD)
    //    MegaSerial.println(F("SD: SDSC Mounted"));
    //else if(cardType == CARD_SDHC)
    //    MegaSerial.println(F("SD: SDHC Mounted"));
    //else
    //    MegaSerial.println(F("SD: UNKNOWN Card Mounted"));

    this->cardSizeMB = SD.cardSize() / (1024 * 1024);
    
    //MegaSerial.printf("SD Card Size: %lluMB\n", this->cardSizeMB);

    if (this->supported) {
      const int NUM_DIGITS = log10(this->cardSizeMB) + 1;
    
      char sz[NUM_DIGITS + 1];
     
      sz[NUM_DIGITS] =  0;
      for ( size_t i = NUM_DIGITS; i--; this->cardSizeMB /= 10)
      {
          sz[i] = '0' + (this->cardSizeMB % 10);
          display_string.concat((String)sz[i]);
      }
  
      this->card_sz = sz;
    }

    buffer_obj = Buffer();
    
    if (!SD.exists("/SCRIPTS")) {
      MegaSerial.println("/SCRIPTS does not exist. Creating...");

      SD.mkdir("/SCRIPTS");
      MegaSerial.println("/SCRIPTS created");
    }
    
    return true;
  }
}

void SDInterface::addPacket(uint8_t* buf, uint32_t len) {
  if ((this->supported) && (this->do_save)) {
    buffer_obj.addPacket(buf, len);
  }
}

void SDInterface::openCapture(String file_name) {
  if (this->supported)
    buffer_obj.open(&SD, file_name);
}

void SDInterface::runUpdate() {
  #ifdef HAS_SCREEN
    display_obj.tft.setTextWrap(false);
    display_obj.tft.setFreeFont(NULL);
    display_obj.tft.setCursor(0, 100);
    display_obj.tft.setTextSize(1);
    display_obj.tft.setTextColor(TFT_WHITE);
  
    display_obj.tft.println(F(text15));
  #endif
  File updateBin = SD.open("/update.bin");
  if (updateBin) {
    if(updateBin.isDirectory()){
      #ifdef HAS_SCREEN
        display_obj.tft.setTextColor(TFT_RED);
        display_obj.tft.println(F(text_table2[0]));
      #endif
      MegaSerial.println(F("Error, update.bin is not a file"));
      #ifdef HAS_SCREEN
        display_obj.tft.setTextColor(TFT_WHITE);
      #endif
      updateBin.close();
      return;
    }

    size_t updateSize = updateBin.size();

    if (updateSize > 0) {
      #ifdef HAS_SCREEN
        display_obj.tft.println(F(text_table2[1]));
      #endif
      MegaSerial.println(F("Try to start update"));
      this->performUpdate(updateBin, updateSize);
    }
    else {
      #ifdef HAS_SCREEN
        display_obj.tft.setTextColor(TFT_RED);
        display_obj.tft.println(F(text_table2[2]));
      #endif
      MegaSerial.println(F("Error, file is empty"));
      #ifdef HAS_SCREEN
        display_obj.tft.setTextColor(TFT_WHITE);
      #endif
      return;
    }

    updateBin.close();
    
      // whe finished remove the binary from sd card to indicate end of the process
    #ifdef HAS_SCREEN
      display_obj.tft.println(F(text_table2[3]));
    #endif
    MegaSerial.println(F("rebooting..."));
    //SD.remove("/update.bin");      
    delay(1000);
    ESP.restart();
  }
  else {
    #ifdef HAS_SCREEN
      display_obj.tft.setTextColor(TFT_RED);
      display_obj.tft.println(F(text_table2[4]));
    #endif
    MegaSerial.println(F("Could not load update.bin from sd root"));
    #ifdef HAS_SCREEN
      display_obj.tft.setTextColor(TFT_WHITE);
    #endif
  }
}

void SDInterface::performUpdate(Stream &updateSource, size_t updateSize) {
  if (Update.begin(updateSize)) {   
    #ifdef HAS_SCREEN
      display_obj.tft.println(text_table2[5] + String(updateSize));
      display_obj.tft.println(F(text_table2[6]));
    #endif
    size_t written = Update.writeStream(updateSource);
    if (written == updateSize) {
      #ifdef HAS_SCREEN
        display_obj.tft.println(text_table2[7] + String(written) + text_table2[10]);
      #endif
      MegaSerial.println("Written : " + String(written) + " successfully");
    }
    else {
      #ifdef HAS_SCREEN
        display_obj.tft.println(text_table2[8] + String(written) + "/" + String(updateSize) + text_table2[9]);
      #endif
      MegaSerial.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
    }
    if (Update.end()) {
      MegaSerial.println("OTA done!");
      if (Update.isFinished()) {
        #ifdef HAS_SCREEN
          display_obj.tft.println(F(text_table2[11]));
        #endif
        MegaSerial.println(F("Update successfully completed. Rebooting."));
      }
      else {
        #ifdef HAS_SCREEN
          display_obj.tft.setTextColor(TFT_RED);
          display_obj.tft.println(text_table2[12]);
        #endif
        MegaSerial.println("Update not finished? Something went wrong!");
        #ifdef HAS_SCREEN
          display_obj.tft.setTextColor(TFT_WHITE);
        #endif
      }
    }
    else {
      #ifdef HAS_SCREEN
        display_obj.tft.println(text_table2[13] + String(Update.getError()));
      #endif
      MegaSerial.println("Error Occurred. Error #: " + String(Update.getError()));
    }

  }
  else
  {
    #ifdef HAS_SCREEN
      display_obj.tft.println(text_table2[14]);
    #endif
    MegaSerial.println("Not enough space to begin OTA");
  }
}

bool SDInterface::checkDetectPin() {
  #ifdef KIT
    if (digitalRead(SD_DET) == LOW)
      return true;
    else
      return false;
  #endif

  return false;
}

void SDInterface::main() {
  if ((this->supported) && (this->do_save)) {
    //MegaSerial.println("Saving packet...");
    buffer_obj.forceSave(&SD);
  }
  else if (!this->supported) {
    if (checkDetectPin()) {
      delay(100);
      this->initSD();
    }
  }
}
