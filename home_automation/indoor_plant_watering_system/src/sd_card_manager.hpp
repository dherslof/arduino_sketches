/*
Description: Simple manager class for handling a sd-card module usage .

Author: dherslof

*/
#ifndef SD_CARD_MANAGER_H
#define SD_CARD_MANAGER_H

#include "Arduino.h"
#include <SD.h>

class SdCardManager {
public:

   SdCardManager(const uint8_t cs_pin);

   // Init the sd card
   bool init() noexcept;

   // Create a new directory
   bool createDirectory(const String& dir_name) const noexcept;

   // Remove existing directory
   bool removeDirectory(const String& dir_name) const noexcept;

   // Create a new file
   bool createFile(const String& file_name) noexcept;

   // Remove a file
   bool removeFile(const String& file_name) noexcept;

   // Check if a file exists
   bool fileExists(const String& file_name) const noexcept;

   // Append to file
   bool appendToFile(const String& file_name, const String& payload) noexcept;

private:
   const uint8_t m_cs_pin;
   File m_file;
};

#endif // SD_CARD_MANAGER_H