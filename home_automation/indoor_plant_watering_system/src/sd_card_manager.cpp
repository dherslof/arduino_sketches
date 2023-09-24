#include "Arduino.h"
#include "sd_card_manager.hpp"

#include <SD.h>
#include <SPI.h>

SdCardManager::SdCardManager(const uint8_t cs_pin) :
   m_cs_pin(cs_pin)
   {}

bool SdCardManager::init() noexcept {
   if (!SD.begin(m_cs_pin)) {
    Serial.println("SdCardManager::Init - SD Card initialization failed");
    return false;
  }
  Serial.println("SdCardManager::Init - SD Card initialization done");
  return true;
}

bool SdCardManager::createDirectory(const String& dir_name) const noexcept {
   auto mkdir_result {SD.mkdir(dir_name)};
   if (mkdir_result == 0) {
      Serial.print("SdCardManager::createDirectory - Failed to create directory with name: ");
      Serial.println(dir_name);
    return false;
   }
   Serial.print("SdCardManager::createDirectory - Created directory with name: ");
   Serial.println(dir_name);
    return true;
}

bool SdCardManager::removeDirectory(const String& dir_name) const noexcept {
   auto rmdir_result {SD.rmdir(dir_name)};
   if (rmdir_result == 0) {
      Serial.print("SdCardManager::removeDirectory - Failed to remove directory with name: ");
      Serial.println(dir_name);
    return false;
   }
   Serial.print("SdCardManager::removeDirectory - Removed directory with name: ");
   Serial.println(dir_name);
    return true;
}

bool SdCardManager::createFile(const String& file_name) noexcept {
   m_file = SD.open(file_name, FILE_WRITE);
   if (!m_file) {
      Serial.print("SdCardManager::createFile - Failed to create file with name: ");
      Serial.println(file_name);
      return false;
   }
   Serial.print("SdCardManager::createFile - File created with name: ");
   Serial.println(file_name);
   m_file.close();
   return true;
}

bool SdCardManager::removeFile(const String& file_name) noexcept {
   auto rmfile_result {SD.remove(file_name)};
   if (rmfile_result == 0) {
      Serial.print("SdCardManager::removeFile - Failed to remove file with name: ");
      Serial.println(file_name);
    return false;
   }
   Serial.print("SdCardManager::removeFile - Removed file with name: ");
   Serial.println(file_name);
    return true;
}

bool SdCardManager::fileExists(const String& file_name) const noexcept {
   auto exists_result {SD.exists(file_name)};
   if (exists_result == 0) {
      Serial.print("SdCardManager::fileExists - file with name: ");
      Serial.print(file_name);
      Serial.println(" dont exists");
    return false;
   }
   Serial.print("SdCardManager::fileExists - file with name: ");
   Serial.print(file_name);
   Serial.println(" exists");
   return true;
}

bool SdCardManager::appendToFile(const String& file_name, const String& payload) noexcept {
   m_file = SD.open(file_name, FILE_WRITE);
   if (!m_file) {
      Serial.print("SdCardManager::appendToFile - Failed to open file with name: ");
      Serial.println(file_name);
      return false;
   }
   Serial.print("SdCardManager::createFile - Opened file with name: ");
   Serial.println(file_name);
   Serial.print("Writing payload to file");
   m_file.println("testing 1, 2, 3.");
   m_file.close();
   return true;
}





