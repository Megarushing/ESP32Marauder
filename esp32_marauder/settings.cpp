#include "settings.h"

String Settings::getSettingsString() {
  return this->json_settings_string;
}

bool Settings::begin() {
  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
    MegaSerial.println("Settings SPIFFS Mount Failed");
    return false;
  }

  File settingsFile;

  //SPIFFS.remove("/settings.json"); // NEED TO REMOVE THIS LINE

  if (SPIFFS.exists("/settings.json")) {
    settingsFile = SPIFFS.open("/settings.json", FILE_READ);
    
    if (!settingsFile) {
      settingsFile.close();
      MegaSerial.println(F("Could not find settings file"));
      if (this->createDefaultSettings(SPIFFS))
        return true;
      else
        return false;    
    }
  }
  else {
    MegaSerial.println("Settings file does not exist");
    if (this->createDefaultSettings(SPIFFS))
      return true;
    else
      return false;
  }

  String json_string;
  DynamicJsonDocument jsonBuffer(1024);
  DeserializationError error = deserializeJson(jsonBuffer, settingsFile);
  serializeJson(jsonBuffer, json_string);
  //MegaSerial.println("Settings: " + (String)json_string + "\n");
  //this->printJsonSettings(json_string);

  this->json_settings_string = json_string;
  
  return true;
}

template <typename T>
T Settings::loadSetting(String key) {}

// Get type int settings
template<>
int Settings::loadSetting<int>(String key) {
  DynamicJsonDocument json(1024); // ArduinoJson v6

  if (deserializeJson(json, this->json_settings_string)) {
    MegaSerial.println("\nCould not parse json");
  }

  for (int i = 0; i < json["Settings"].size(); i++) {
    if (json["Settings"][i]["name"].as<String>() == key)
      return json["Settings"][i]["value"];
  }

  return 0;
}

// Get type string settings
template<>
String Settings::loadSetting<String>(String key) {
  //return this->json_settings_string;
  
  DynamicJsonDocument json(1024); // ArduinoJson v6

  if (deserializeJson(json, this->json_settings_string)) {
    MegaSerial.println("\nCould not parse json");
  }

  for (int i = 0; i < json["Settings"].size(); i++) {
    if (json["Settings"][i]["name"].as<String>() == key)
      return json["Settings"][i]["value"];
  }

  return "";
}

// Get type bool settings
template<>
bool Settings::loadSetting<bool>(String key) {
  DynamicJsonDocument json(1024); // ArduinoJson v6

  if (deserializeJson(json, this->json_settings_string)) {
    MegaSerial.println("\nCould not parse json");
  }

  for (int i = 0; i < json["Settings"].size(); i++) {
    if (json["Settings"][i]["name"].as<String>() == key)
      return json["Settings"][i]["value"];
  }

  return false;
}

//Get type uint8_t settings
template<>
uint8_t Settings::loadSetting<uint8_t>(String key) {
  DynamicJsonDocument json(1024); // ArduinoJson v6

  if (deserializeJson(json, this->json_settings_string)) {
    MegaSerial.println("\nCould not parse json");
  }

  for (int i = 0; i < json["Settings"].size(); i++) {
    if (json["Settings"][i]["name"].as<String>() == key)
      return json["Settings"][i]["value"];
  }

  return 0;
}

template <typename T>
T Settings::saveSetting(String key, bool value) {}

template<>
bool Settings::saveSetting<bool>(String key, bool value) {
  DynamicJsonDocument json(1024); // ArduinoJson v6

  if (deserializeJson(json, this->json_settings_string)) {
    MegaSerial.println("\nCould not parse json");
  }

  String settings_string;

  for (int i = 0; i < json["Settings"].size(); i++) {
    if (json["Settings"][i]["name"].as<String>() == key) {
      json["Settings"][i]["value"] = value;

      MegaSerial.println("Saving setting...");

      File settingsFile = SPIFFS.open("/settings.json", FILE_WRITE);

      if (!settingsFile) {
        MegaSerial.println(F("Failed to create settings file"));
        return false;
      }

      if (serializeJson(json, settingsFile) == 0) {
        MegaSerial.println(F("Failed to write to file"));
      }
      if (serializeJson(json, settings_string) == 0) {
        MegaSerial.println(F("Failed to write to string"));
      }
    
      // Close the file
      settingsFile.close();
    
      this->json_settings_string = settings_string;
    
      this->printJsonSettings(settings_string);
      
      return true;
    }
  }
  return false;
}

bool Settings::toggleSetting(String key) {
  DynamicJsonDocument json(1024); // ArduinoJson v6

  if (deserializeJson(json, this->json_settings_string)) {
    MegaSerial.println("\nCould not parse json");
  }

  for (int i = 0; i < json["Settings"].size(); i++) {
    if (json["Settings"][i]["name"].as<String>() == key) {
      if (json["Settings"][i]["value"]) {
        saveSetting<bool>(key, false);
        MegaSerial.println("Setting value to false");
        return false;
      }
      else {
        saveSetting<bool>(key, true);
        MegaSerial.println("Setting value to true");
        return true;
      }

      return false;
    }
  }
}

String Settings::setting_index_to_name(int i) {
  DynamicJsonDocument json(1024); // ArduinoJson v6

  if (deserializeJson(json, this->json_settings_string)) {
    MegaSerial.println("\nCould not parse json");
  }

  return json["Settings"][i]["name"];
}

int Settings::getNumberSettings() {
  DynamicJsonDocument json(1024); // ArduinoJson v6

  if (deserializeJson(json, this->json_settings_string)) {
    MegaSerial.println("\nCould not parse json");
  }

  return json["Settings"].size();
}

String Settings::getSettingType(String key) {
  DynamicJsonDocument json(1024); // ArduinoJson v6

  if (deserializeJson(json, this->json_settings_string)) {
    MegaSerial.println("\nCould not parse json");
  }
  
  for (int i = 0; i < json["Settings"].size(); i++) {
    if (json["Settings"][i]["name"].as<String>() == key)
      return json["Settings"][i]["type"];
  }
}

void Settings::printJsonSettings(String json_string) {
  DynamicJsonDocument json(1024); // ArduinoJson v6

  if (deserializeJson(json, json_string)) {
    MegaSerial.println("\nCould not parse json");
  }
  
  MegaSerial.println("Settings\n----------------------------------------------");
  for (int i = 0; i < json["Settings"].size(); i++) {
    MegaSerial.println("Name: " + json["Settings"][i]["name"].as<String>());
    MegaSerial.println("Type: " + json["Settings"][i]["type"].as<String>());
    MegaSerial.println("Value: " + json["Settings"][i]["value"].as<String>());
    MegaSerial.println("----------------------------------------------");
  }
}

bool Settings::createDefaultSettings(fs::FS &fs) {
  MegaSerial.println(F("Creating default settings file: settings.json"));
  
  File settingsFile = fs.open("/settings.json", FILE_WRITE);

  if (!settingsFile) {
    MegaSerial.println(F("Failed to create settings file"));
    return false;
  }

  DynamicJsonDocument jsonBuffer(1024);
  String settings_string;

  //jsonBuffer["Settings"][0]["name"] = "Channel";
  //jsonBuffer["Settings"][0]["type"] = "uint8_t";
  //jsonBuffer["Settings"][0]["value"] = 11;
  //jsonBuffer["Settings"][0]["range"]["min"] = 1;
  //jsonBuffer["Settings"][0]["range"]["max"] = 14;

  //jsonBuffer["Settings"][1]["name"] = "Channel Hop Delay";
  //jsonBuffer["Settings"][1]["type"] = "int";
  //jsonBuffer["Settings"][1]["value"] = 1;
  //jsonBuffer["Settings"][1]["range"]["min"] = 1;
  //jsonBuffer["Settings"][1]["range"]["max"] = 10;

  jsonBuffer["Settings"][0]["name"] = "ForcePMKID";
  jsonBuffer["Settings"][0]["type"] = "bool";
  jsonBuffer["Settings"][0]["value"] = true;
  jsonBuffer["Settings"][0]["range"]["min"] = false;
  jsonBuffer["Settings"][0]["range"]["max"] = true;

  jsonBuffer["Settings"][1]["name"] = "ForceProbe";
  jsonBuffer["Settings"][1]["type"] = "bool";
  jsonBuffer["Settings"][1]["value"] = true;
  jsonBuffer["Settings"][1]["range"]["min"] = false;
  jsonBuffer["Settings"][1]["range"]["max"] = true;

  jsonBuffer["Settings"][2]["name"] = "SavePCAP";
  jsonBuffer["Settings"][2]["type"] = "bool";
  jsonBuffer["Settings"][2]["value"] = true;
  jsonBuffer["Settings"][2]["range"]["min"] = false;
  jsonBuffer["Settings"][2]["range"]["max"] = true;

  //jsonBuffer.printTo(settingsFile);
  if (serializeJson(jsonBuffer, settingsFile) == 0) {
    MegaSerial.println(F("Failed to write to file"));
  }
  if (serializeJson(jsonBuffer, settings_string) == 0) {
    MegaSerial.println(F("Failed to write to string"));
  }

  // Close the file
  settingsFile.close();

  this->json_settings_string = settings_string;

  this->printJsonSettings(settings_string);

  return true;
}

void Settings::main(uint32_t currentTime) {
  
}
