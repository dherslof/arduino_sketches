/*
Description: Trivial representation class for the (HW) capacitive soil sensor used.

Author: dherslof

*/
#ifndef CAPACITIVE_SOIL_SENSOR_H
#define CAPACITIVE_SOIL_SENSOR_H

#include "Arduino.h"

class CapacitiveSoilSensor {
public:
   CapacitiveSoilSensor(const uint8_t analog_read_pin, const uint8_t digital_pwr_pin, const uint8_t sensor_id) noexcept;

   // Start sensor
   void setPwrOn() noexcept;
   // Stop sensor
   void setPwrOff() noexcept;

   // Read sensor value
   int32_t read() const noexcept;

   // Get current soil moisture in percentage
   int16_t currentMoisturePercentage() const noexcept;

   uint8_t getSensorId() const noexcept;

   private:
   // Associated pins needed by HW sensor
   const uint8_t m_analog_read_pin;
   const uint8_t m_dig_pwr_pin;

   uint16_t m_threshold_wet {0};
   uint16_t m_threshold_dry {0};

   bool m_pwr_on = false;
   const uint8_t m_sensor_id;

   bool inline isPwrOn() const noexcept { return m_pwr_on; }

};

# endif //CAPACITIVE_SOIL_SENSOR