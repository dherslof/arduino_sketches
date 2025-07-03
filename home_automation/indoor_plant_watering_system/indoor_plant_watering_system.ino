// Description: to-be-written
// 
// Author: dherslof

#include <Wire.h>
#include <DS3231.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <SD.h>
#include <SPI.h>

#define PUMP_1_PIN_NUM 5  // DIG OUT PIN for pump 1
#define PUMP_2_PIN_NUM 6  // DIG OUT PIN for pump 2
#define PUMP_3_PIN_NUM 7  // DIG OUT PIN for pump 3
#define PUMP_4_PIN_NUM 8  // DIG OUT PIN for pump 4

#define PUMP_ON_TIME_S 5  // Time to keep the pump ON in seconds

#define PUMP_ON 1  // Convenient type def for pump ON
#define PUMP_OFF 2 // Convenient type def for pump OFF

#define SOIL_SENSOR_1_IN_PIN_NUM 0 // ANALOG IN PIN for soil sensor 1
#define SOIL_SENSOR_2_IN_PIN_NUM 1 // ANALOG IN PIN for soil sensor 2
#define SOIL_SENSOR_3_IN_PIN_NUM 2 // ANALOG IN PIN for soil sensor 3
#define SOIL_SENSOR_4_IN_PIN_NUM 3 // ANALOG IN PIN for soil sensor 4

#define SOIL_SENSOR_CALIBRATION_WATER 221 // Calibration value. Put sensor in water and set reading
#define SOIL_SENSOR_CALIBRATION_AIR 527   // Calibartion value. Put sensor in dry air and set reading

#define SOIL_MOISTURE_DRY_LIMIT_PERCENTAGE 15   // Tuning value. Dry limit in procentage. Soil value below this indicates that the soil needs water  
#define VENUS_FLYTRAP_WATER_INTERVAL 16         // Tuning valueInterval for when this specific plant needs water. 16 => every 4th day (4 wakeups per day - every 6h)
#define VENUS_FLYTRAP_WATER_EMPTY_CYCLES 4      // Tuning value. Indicates how many times pump can be activated before empty water container.

#define WATER_LEVEL_DISTANCE_EMPTY_CM 25        // Tuning value. Distance (cm) to empty water level. Set for "normal bucket".
#define NUM_OF_WATER_LEVEL_SENSOR_READINGS 10   // Tuning value Amount of times to read the sensor in order to calculate average distance to water
#define HCSR04_TRIG_PIN_NUM 9                   // Ultrasonic sensor trigger pin
#define HCSR04_ECHO_PIN_NUM 4                   // Ultrasonic sensor echo pin (10 before)

// SPI pins for SD card writing
#define SD_CS_PIN 10
#define SD_MOSI_PIN 11
#define SD_MISO_PIN 12
#define SD_CLK_PIN 13

#define SYSTEM_ERROR_LED_PIN 3                  // LED for indicating SYSTEM FAULT

//TODO: If possible, add manual triggering otherwise remove (interrupt 2 can be used, pin3)
//#define EXECUTION_DELAY_MS 60000                 // Time to wait until next execution cycle
//#define PUMP_SELECT_PB_PIN 8                     // Push button to select pump to activate manually 
//#define PUMP_MANUAL_ACTIVE_PB_PIN 9              // Push button to manually activate pump

#define RTC_INTERRUPT_PIN_NUM 2        // Digital pin 2 is the interrupt pin on the UNO board
#define SLEEP_INTERVAL_MINUTES 360     // Tuning value. Time between wakeups, 360min = 6h
#define DS3231_I2C_ADDRESS 0x68        // RTC I2C address, used for communication
#define DS3231_CTRL_REG_ADDRESS 0x0E   // Control register address for rtc
#define DS3231_ENABLE_ALARM1_VAL 0x05  // Set A1IE (1) and INTCN (1) => enabling alarm1 interrupt mode    
#define DS3231_STATUS_REG_ADDRESS 0x0F // Status register for the rtc
#define DS3231_CLEAR_ALARM1_VAL 0x00   // Bit 0 is the Alarm1 flag but value is for clearing full register


// Global variables for soil sensor reading. Only 2 variables for memory constraints
int soil_sensor_raw_value;
uint8_t soil_sensor_percentage;

// Global variables for sleep & wakeup
volatile bool wakeup_flag = false;         // True indicates interrupt wakeup
volatile uint8_t wakeup_counter = 0;       // Count amount of wakeups. 
static DS3231 rtc;                         // Real time clock object
bool first_boot = true;                    // Indicate if it's first boot or if system has been running
uint8_t venus_flytrap_pump_activation = 0; // Count how many times the specific pump has been activated

// Global variables for water level
float current_average_water_level_distane_cm; // Avarge distance to water surface
long duration;                                // Duration between echo and response

// Log file name
const char* log_file_name = "ws_log.txt"; // WaterSystem Log file name
char sd_log_entry[100];                   // Log entry buffer

void CapacitiveSoilSensorRead(const uint8_t sensor_input_pin, int& raw, uint8_t& percentage) {
  // Serial prints only for debug
  raw = analogRead(sensor_input_pin);
  percentage = map(raw, SOIL_SENSOR_CALIBRATION_AIR, SOIL_SENSOR_CALIBRATION_WATER, 0, 100);
  percentage = constrain(percentage, 0, 100);
  Serial.print(F("[CSS-"));
  Serial.print(sensor_input_pin);
  Serial.print(F("]: raw value = "));
  Serial.print(raw);
  Serial.print(F(" -> "));
  Serial.println(percentage);
}

void PumpInit(const uint8_t pump_out_pin) {
  // Serial prints only for debug
  Serial.print(F("Init pump pin: "));
  Serial.println(pump_out_pin);
  pinMode(pump_out_pin, OUTPUT);
  digitalWrite(pump_out_pin, LOW);
}

void TogglePump(const uint8_t pump_out_pin, const uint8_t state) {
  if (state == PUMP_ON) {
    Serial.print(F("Starting pump pin #"));
    Serial.println(pump_out_pin);
    digitalWrite(pump_out_pin, LOW);
  } else if (state == PUMP_OFF) {
    Serial.print(F("Stopping pump pin #"));
    Serial.println(pump_out_pin);
    digitalWrite(pump_out_pin, HIGH);
  } else {
    // UNKNOWN STATE
  }
}

void SetAlarmForNextInterval(const uint16_t sleep_interval) {
  // Get current time
  bool h12, PM;
  uint8_t current_hour = rtc.getHour(h12, PM);
  uint8_t current_minute = rtc.getMinute();
  
  // Calculate wakeup time, current time + sleep interval
  uint8_t alarm_minute = current_minute + sleep_interval;
  uint8_t alarm_hour = current_hour;

  alarm_hour += alarm_minute / 60;    // Add the extra full H
  alarm_minute = alarm_minute % 60;   // Keep remaining minutes
  alarm_hour = alarm_hour % 24;       // Ensure 24h
  
  // Configure SQW pin for alarm interrupt
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(DS3231_CTRL_REG_ADDRESS);
  Wire.write(DS3231_ENABLE_ALARM1_VAL);
  Wire.endTransmission();
  
  // Set Alarm1 for the calculated time => day, h,min,s,alarmbits (defines which parts of time must match for trigger), am-pm, mode, day-of-the-week matching
  rtc.setA1Time(0, alarm_hour, alarm_minute, 0, 0b00001100, false, false, false);
  
  // Enable Alarm 1
  rtc.turnOnAlarm(1);
  
  Serial.print(F("Next alarm set for: "));
  Serial.print(alarm_minute);
  Serial.print(F(":"));
  if (alarm_minute < 10){
    Serial.print(F("0"));
  }
  Serial.println(alarm_minute);
}

void clearAlarm() {
  // Direct register access to clear alarm flag
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(DS3231_STATUS_REG_ADDRESS);
  Wire.write(DS3231_CLEAR_ALARM1_VAL);
  Wire.endTransmission();
}

//ISR function
void HandleInterrupt() {
  wakeup_flag = true;
  wakeup_counter += 1;
}

void GoToSleep() {
  Serial.println(F("Going to sleep..."));
  Serial.flush();
  
  // Disable ADC
  ADCSRA &= ~(1 << ADEN);
  
  // Configure sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  
  // Disable unused peripherals
  power_adc_disable();
  power_spi_disable();
  power_timer1_disable();
  power_timer2_disable();
  
  if (Serial.available()) {
    power_usart0_disable();
  }
  
  // Disable brown-out detection
  noInterrupts();
  sleep_bod_disable();
  
  // Enter sleep mode
  sleep_cpu();
  interrupts();
  
  // Code resumes here after wake-up
  sleep_disable();
  
  // Re-enable peripherals
  sleep_disable();
  power_adc_enable();
  power_spi_enable();
  power_timer1_enable();
  power_timer2_enable();
  power_usart0_enable();
  
  // Re-enable ADC
  ADCSRA |= (1 << ADEN);
}

void InitDistanceSensor() {
  Serial.print(F("Distance sensor init. TRIG pin: "));
  Serial.print(HCSR04_TRIG_PIN_NUM);
  Serial.print(F(" and ECHO pin: "));
  Serial.println(HCSR04_ECHO_PIN_NUM);
  
  pinMode(HCSR04_TRIG_PIN_NUM, OUTPUT);
  pinMode(HCSR04_ECHO_PIN_NUM, INPUT);

  // Initial delay to let sensor stabilize, might not be needed but since no time constraints - leave it
  delay(1000);
}

void ReadAndCalculateAverageWaterDistance(float& average_distance) {
  Serial.println(F("Reading distance sensor"));
  float sum = 0.0;
  // Read sensor x defined times and get average
  for (uint8_t n = 0; n < NUM_OF_WATER_LEVEL_SENSOR_READINGS; n++) {
    digitalWrite(HCSR04_TRIG_PIN_NUM, LOW);
    delayMicroseconds(2);
  
    // Send 10µs pulse to trigger
    digitalWrite(HCSR04_TRIG_PIN_NUM, HIGH);
    delayMicroseconds(10);
    digitalWrite(HCSR04_TRIG_PIN_NUM, LOW);

    // Measure the response from echo pin
    duration = pulseIn(HCSR04_ECHO_PIN_NUM, HIGH);

    // Calculate distance in centimeters
    // Sound speed = 343m/s = 34300cm/s
    // Distance = (Time × Speed) ÷ 2
    // The factor 0.0343 comes from 34300/1000000
    sum += (duration * 0.0343 / 2); // Distance in cm
  }

  average_distance = (sum / NUM_OF_WATER_LEVEL_SENSOR_READINGS);
  Serial.print(F("Average distance to water surface from "));
  Serial.print(NUM_OF_WATER_LEVEL_SENSOR_READINGS);
  Serial.print(F(" sensor readings are: "));
  Serial.print(average_distance);
  Serial.print(F("cm"));
}

//void WriteToSdLog(const String& data) {
void WriteToSdLog(const char* data) {
  File logfile = SD.open(log_file_name, FILE_WRITE);
  if (logfile) {
    logfile.println(data);
    logfile.close();
  } else {
    Serial.println(F("Unable to open system logfile for writing"));
    
  }
}

void SetSystemError() {
  digitalWrite(SYSTEM_ERROR_LED_PIN, HIGH);
  Serial.println(F("Setting system error LED HIGH"));
}

void ClearSstemError() {
  digitalWrite(SYSTEM_ERROR_LED_PIN, LOW);
  Serial.println(F("System error LED cleared"));
}

void setup() {
  // Init serial for debug logging
  Serial.begin(9600); // For logging
  Wire.begin();       // For I2C

  // Boot message
  Serial.println(F("dherslof - Indoor Plant Watering system boot"));

  // Init pumps
  PumpInit(PUMP_1_PIN_NUM);
  PumpInit(PUMP_2_PIN_NUM);
  PumpInit(PUMP_3_PIN_NUM);
  PumpInit(PUMP_4_PIN_NUM);

  // Init rtc
  // Current time has to be set during compilation
  rtc.setClockMode(false);  // Set to 24h mode
  rtc.setYear(25);
  rtc.setMonth(3);
  rtc.setDate(8);
  rtc.setDoW(6);  // 1=Sunday, 7=Saturday
  rtc.setHour(18);
  rtc.setMinute(13);
  rtc.setSecond(0);

  // Attach interrupt to ISR function
  pinMode(RTC_INTERRUPT_PIN_NUM, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RTC_INTERRUPT_PIN_NUM), HandleInterrupt, FALLING);

  // Init distance sensor to water
  InitDistanceSensor();

  // Init SD card
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println(F("Failed to init SD-card"));
    SetSystemError();
  }

  
  sprintf(sd_log_entry, PSTR("System boot and setup done - Entering main loop"));
  WriteToSdLog(sd_log_entry);
}

void loop() {
  if (wakeup_flag || first_boot) {
    first_boot = false;
    
    // Print wakeup
    bool h12, PM;
    Serial.print(F("Woke up at: "));
    Serial.print(rtc.getHour(h12, PM)); 
    Serial.print(F(":"));
    if (rtc.getMinute() < 10) Serial.print(F("0"));
    Serial.print(rtc.getMinute());
    Serial.print(F(":"));
    if (rtc.getSecond() < 10) Serial.print(F("0"));
    Serial.println(rtc.getSecond());
    
    clearAlarm();
    wakeup_flag = false;
    
    sprintf(sd_log_entry, PSTR("Woke up at hour %d"), rtc.getHour(h12, PM));
    WriteToSdLog(sd_log_entry);
    
    ReadAndCalculateAverageWaterDistance(current_average_water_level_distane_cm);
    
    sprintf(sd_log_entry, PSTR("Current average water level distance %d"), current_average_water_level_distane_cm);
    WriteToSdLog(sd_log_entry);
    
    CapacitiveSoilSensorRead(SOIL_SENSOR_1_IN_PIN_NUM, soil_sensor_raw_value, soil_sensor_percentage);
    if (soil_sensor_percentage <= SOIL_MOISTURE_DRY_LIMIT_PERCENTAGE) {
      Serial.print(F("Soil sensor percentage for sensor #1 (pin: "));
      Serial.print(SOIL_SENSOR_1_IN_PIN_NUM);
      Serial.println(F(") is below dry limit. Water needed."));
      
      sprintf(sd_log_entry, PSTR("SS1 indicates dry soil %d, water needed"),soil_sensor_percentage);
      WriteToSdLog(sd_log_entry);
      
      if (current_average_water_level_distane_cm < WATER_LEVEL_DISTANCE_EMPTY_CM) {
        Serial.println(F("Water level ok. Allowed to start pump"));
        TogglePump(PUMP_1_PIN_NUM, PUMP_ON);
        delay(PUMP_ON_TIME_S);
        TogglePump(PUMP_1_PIN_NUM, PUMP_OFF);
        
        sprintf(sd_log_entry, PSTR("Pump1 used"));
        WriteToSdLog(sd_log_entry);
      } else {
        sprintf(sd_log_entry, PSTR("Pump1 not allowed to start due to water level"));
        WriteToSdLog(sd_log_entry);
        Serial.println(F("Water level indicates empty. Not allowed to start pump"));
      }
    }
    
    CapacitiveSoilSensorRead(SOIL_SENSOR_2_IN_PIN_NUM, soil_sensor_raw_value, soil_sensor_percentage);
    if (soil_sensor_percentage <= SOIL_MOISTURE_DRY_LIMIT_PERCENTAGE) {
      Serial.print(F("Soil sensor percentage for sensor #1 (pin: "));
      Serial.print(SOIL_SENSOR_1_IN_PIN_NUM);
      Serial.println(F(") is below dry limit. Water needed."));
      
      sprintf(sd_log_entry, PSTR("SS2 indicates dry soil %d, water needed"), soil_sensor_percentage);
      WriteToSdLog(sd_log_entry);
      
      if (current_average_water_level_distane_cm < WATER_LEVEL_DISTANCE_EMPTY_CM) {
        Serial.println(F("Water level ok. Allowed to start pump"));
        TogglePump(PUMP_2_PIN_NUM, PUMP_ON);
        delay(PUMP_ON_TIME_S);
        TogglePump(PUMP_2_PIN_NUM, PUMP_OFF);
        
        sprintf(sd_log_entry, PSTR("Pump2 used"));
        WriteToSdLog(sd_log_entry);
      } else {
        sprintf(sd_log_entry, PSTR("Pump2 not allowed to start due to water level"));
        WriteToSdLog(sd_log_entry);
        Serial.println(F("Water level indicates empty. Not allowed to start pump"));
      }
    }

    CapacitiveSoilSensorRead(SOIL_SENSOR_3_IN_PIN_NUM, soil_sensor_raw_value, soil_sensor_percentage);
    if (soil_sensor_percentage <= SOIL_MOISTURE_DRY_LIMIT_PERCENTAGE) {
      Serial.print(F("Soil sensor percentage for sensor #1 (pin: "));
      Serial.print(SOIL_SENSOR_1_IN_PIN_NUM);
      Serial.println(F(") is below dry limit. Water needed."));

      sprintf(sd_log_entry, PSTR("SS3 indicates dry soil %d, water needed"), soil_sensor_percentage);
      WriteToSdLog(sd_log_entry);
      
      if (current_average_water_level_distane_cm < WATER_LEVEL_DISTANCE_EMPTY_CM) {
        Serial.println(F("Water level ok. Allowed to start pump"));
        TogglePump(PUMP_3_PIN_NUM, PUMP_ON);
        delay(PUMP_ON_TIME_S);
        TogglePump(PUMP_3_PIN_NUM, PUMP_OFF);

        sprintf(sd_log_entry, PSTR("Pump3 used"));
        WriteToSdLog(sd_log_entry);
      } else {
        sprintf(sd_log_entry, PSTR("Pump3 not allowed to start due to water level"));
        WriteToSdLog(sd_log_entry);
        Serial.println(F("Water level indicates empty. Not allowed to start pump"));
      }
    }
    
    // Check if Venus needs water
    if (wakeup_counter >= VENUS_FLYTRAP_WATER_INTERVAL) {
      Serial.println(F("Venus flytrap water interval reached"));
      
      sprintf(sd_log_entry, PSTR("Venus flytrap wakeup counter %d, water needed"), wakeup_counter);
      WriteToSdLog(sd_log_entry);
      
      if (venus_flytrap_pump_activation < VENUS_FLYTRAP_WATER_EMPTY_CYCLES) {
        Serial.println(F("Activating venus flytrap pump"));
        TogglePump(PUMP_4_PIN_NUM, PUMP_ON);
        delay(PUMP_ON_TIME_S);
        TogglePump(PUMP_4_PIN_NUM, PUMP_OFF);

        sprintf(sd_log_entry, PSTR("Pump4 used"));
        WriteToSdLog(sd_log_entry);

        // Reset counter (Do it inside if-statement on success, because we want to try again next time if water level has increased instead of waiting 4 days)
        wakeup_counter = 0;
        venus_flytrap_pump_activation += 1;
      } else {
        sprintf(sd_log_entry, PSTR("Venus flytrap activation counter limit reached"));
        WriteToSdLog(sd_log_entry);
        Serial.print(F("Venus flytrap pump activation counter: "));
        Serial.print(venus_flytrap_pump_activation);
        Serial.print(F(" above set limit:"));
        Serial.print(VENUS_FLYTRAP_WATER_EMPTY_CYCLES);
        Serial.println(F(". Water can be empty. Pump not activated"));
      }
    }

    sprintf(sd_log_entry, PSTR("Going to sleep"));
    WriteToSdLog(sd_log_entry);
    
    SetAlarmForNextInterval(SLEEP_INTERVAL_MINUTES);
    delay(1000);  // Give some time for serial output 
  }
  
  GoToSleep();
}
