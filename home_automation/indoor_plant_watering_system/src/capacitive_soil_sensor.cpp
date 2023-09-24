#include "Arduino.h"
#include "capacitive_soil_sensor.hpp"

CapacitiveSoilSensor::CapacitiveSoilSensor(const uint8_t analog_read_pin,
                                           const uint8_t digital_pwr_pin,
                                           const uint8_t sensor_id) noexcept :
   m_analog_read_pin(analog_read_pin),
   m_dig_pwr_pin(digital_pwr_pin),
   m_sensor_id(sensor_id)
   {
      pinMode(m_dig_pwr_pin, OUTPUT);
      digitalWrite(m_dig_pwr_pin, LOW);
   }

void CapacitiveSoilSensor::setPwrOn() noexcept {
   Serial.print("Setting PWR ON for sensor with ID: ");
   Serial.println(m_sensor_id);
   digitalWrite(m_dig_pwr_pin, HIGH);
   m_pwr_on = true;
}

void CapacitiveSoilSensor::setPwrOff() noexcept {
   Serial.print("Setting PWR OFF for sensor with ID: ");
   Serial.println(m_sensor_id);
   digitalWrite(m_dig_pwr_pin, LOW);
   m_pwr_on = false;
}

int32_t CapacitiveSoilSensor::read() const noexcept {
   if (!m_pwr_on) {
      Serial.print("CapacitiveSoilSensor::read: Unable to read value from sensor with ID: ");
      Serial.print(m_sensor_id);
      Serial.println(" , PWR is not on.");
      return -1;
   }

   int32_t value {analogRead(m_analog_read_pin)};
   return value;
}

int16_t CapacitiveSoilSensor::currentMoisturePercentage() const noexcept {
   if (!m_pwr_on) {
      Serial.print("CapacitiveSoilSensor::currentMoisturePercentage: Unable to get current moisture for ID: ");
      Serial.print(m_sensor_id);
      Serial.println(" , PWR is not on.");
      return -1;
   }

   auto sensor_reading {read()};
   if (sensor_reading == -1) {
      Serial.println("CapacitiveSoilSensor::currentMoisturePercentage: Error value from sensor value");
      return -1;
   }

   int16_t percentage_value = map(sensor_reading, m_threshold_wet, m_threshold_dry, 100, 0);
   return percentage_value;
}

uint8_t CapacitiveSoilSensor::getSensorId() const noexcept {
   return m_sensor_id;
}

