/*
Description: Trivial control class for the pump relay module. The HW module has 4 relays on it, so it has the capacity to control 4 pumps.

Author: dherslof

*/
#ifndef PUMP_RELAY_CTRL_H
#define PUMP_RELAY_CTRL_H

#include "Arduino.h"

class PumpRelay {
   public:
   // Set pin number for relays
   void setPinOutRelay1(const uint8_t pin_nr) noexcept;
   void setPinOutRelay2(const uint8_t pin_nr) noexcept;
   void setPinOutRelay3(const uint8_t pin_nr) noexcept;
   void setPinOutRelay4(const uint8_t pin_nr) noexcept;

   // Set relay ON
   bool setRelayOn(uint8_t relay) noexcept;
   // Set relay OFF
   bool setRelayOff(uint8_t relay) noexcept;
   // Set relay ON for X seconds
   bool setRelayOnForSpecifiedTime(uint8_t relay, uint32_t seconds) noexcept;

   private:
   bool isPinOutSet(const uint8_t relay) const noexcept;
   uint32_t getRelayNumberFromPin(uint8_t pin_num) const  noexcept;

   // Relay pinOut mapping
   uint8_t m_pin_num_relay_1 = 0;
   uint8_t m_pin_num_relay_2 = 0;
   uint8_t m_pin_num_relay_3 = 0;
   uint8_t m_pin_num_relay_4 = 0;

   // Max number of relays
   const uint8_t m_number_of_relays = 4;
};


#endif //PUMP_RELAY_CTRL_H