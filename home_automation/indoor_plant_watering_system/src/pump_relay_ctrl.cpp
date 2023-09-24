#include "Arduino.h"
#include "pump_relay_ctrl.hpp"

void PumpRelay::setPinOutRelay1(const uint8_t pin_nr) noexcept {
   m_pin_num_relay_1 = pin_nr;
   pinMode(m_pin_num_relay_1, OUTPUT);
}

void PumpRelay::setPinOutRelay2(const uint8_t pin_nr) noexcept {
   m_pin_num_relay_2 = pin_nr;
   pinMode(m_pin_num_relay_2, OUTPUT);
}

void PumpRelay::setPinOutRelay3(const uint8_t pin_nr) noexcept {
   m_pin_num_relay_3 = pin_nr;
   pinMode(m_pin_num_relay_3, OUTPUT);
}

void PumpRelay::setPinOutRelay4(const uint8_t pin_nr) noexcept {
   m_pin_num_relay_4 = pin_nr;
   pinMode(m_pin_num_relay_4, OUTPUT);
}

bool PumpRelay::setRelayOn(uint8_t relay) noexcept {
   if (relay > m_number_of_relays) {
      Serial.print("PumpRelay::setRelayOn: Relay number ");
      Serial.print(relay);
      Serial.println(" is not available, unable to turn it ON");
      return false;
   }

   if (!isPinOutSet(relay)) {
      Serial.print("PinOut is not set for relay # ");
      Serial.println(relay);

      return false;
   }

   Serial.print("Turning relay # ");
   Serial.print(relay);
   Serial.println(" ON");

   switch (relay)
   {
   case 1:
      digitalWrite(m_pin_num_relay_1, HIGH);
      break;

   case 2:
      digitalWrite(m_pin_num_relay_2, HIGH);
      break;

   case 3:
      digitalWrite(m_pin_num_relay_3, HIGH);
      break;

   case 4:
      digitalWrite(m_pin_num_relay_4, HIGH);
      break;

   default:
      // Should never happen, do nothing
      Serial.println("PumpRelay::setRelayOn: Doing nothing do to default unforeseen case statement");
      break;
   }
}

bool PumpRelay::setRelayOff(uint8_t relay) noexcept {
   if (relay > m_number_of_relays) {
      Serial.print("PumpRelay::setRelayOff: Relay number ");
      Serial.print(relay);
      Serial.println(" is not available, unable to turn it Off");
      return false;
   }

   if (!isPinOutSet(relay)) {
      Serial.print("PinOut is not set for relay # ");
      Serial.println(relay);

      return false;
   }

   Serial.print("Turning relay # ");
   Serial.print(relay);
   Serial.println(" OFF");

   switch (relay)
   {
   case 1:
      digitalWrite(m_pin_num_relay_1, LOW);
      break;

   case 2:
      digitalWrite(m_pin_num_relay_2, LOW);
      break;

   case 3:
      digitalWrite(m_pin_num_relay_3, LOW);
      break;

   case 4:
      digitalWrite(m_pin_num_relay_4, LOW);
      break;

   default:
      // Should never happen, do nothing
      Serial.println("PumpRelay::setRelayOff: Doing nothing do to default unforeseen case statement");
      break;
   }
}

bool PumpRelay::setRelayOnForSpecifiedTime(uint8_t relay, uint32_t seconds) noexcept {
   if (relay > m_number_of_relays) {
      Serial.print("PumpRelay::setRelayOn: Relay number ");
      Serial.print(relay);
      Serial.println(" is not available, unable to turn it ON");
      return false;
   }

   if (!isPinOutSet(relay)) {
      Serial.print("PinOut is not set for relay # ");
      Serial.println(relay);

      return false;
   }

   uint32_t delay_time = (seconds * 1000);

   Serial.print("Turning relay # ");
   Serial.print(relay);
   Serial.print(" ON for ");
   Serial.print(seconds);
   Serial.println(" s");

   switch (relay)
   {
   case 1:
      digitalWrite(m_pin_num_relay_1, HIGH);
      delay(delay_time);
      digitalWrite(m_pin_num_relay_1, LOW);
      break;

   case 2:
      digitalWrite(m_pin_num_relay_2, HIGH);
      delay(delay_time);
      digitalWrite(m_pin_num_relay_2, LOW);
      break;

   case 3:
      digitalWrite(m_pin_num_relay_3, HIGH);
      delay(delay_time);
      digitalWrite(m_pin_num_relay_3, LOW);
      break;

   case 4:
      digitalWrite(m_pin_num_relay_4, HIGH);
      delay(delay_time);
      digitalWrite(m_pin_num_relay_4, LOW);
      break;

   default:
      // Should never happen, do nothing
      Serial.println("PumpRelay::setRelayOn: Doing nothing do to default unforeseen case statement");
      break;
   }
}

bool PumpRelay::isPinOutSet(const uint8_t relay) const noexcept {
   bool pin_set = false;
   switch (relay)
   {
   case 1:
      if (m_pin_num_relay_1 != 0) {
         pin_set = true;
      }
      break;

   case 2:
      if (m_pin_num_relay_2 != 0) {
         pin_set = true;
      }
      break;

   case 3:
      if (m_pin_num_relay_3 != 0) {
         pin_set = true;
      }
      break;

   case 4:
      if (m_pin_num_relay_4 != 0) {
         pin_set = true;
      }
      break;

   default:
      // Should never happen, do nothing
      Serial.println("PumpRelay::isPinOutSet: Relay not available");
      break;
   }

   return pin_set;
}

