#include "src/capacitive_soil_sensor.hpp"
#include "src/pump_relay_ctrl.hpp"
#include "src/sd_card_manager.hpp"

// TODO: Find a sensor to determine if the water in the tank are empty. Pumps should not run dry.
// TODO: Add sd-card reader so log can be stored. 
// TODO: Add push button for changing mode to static_mode (pumps running on fixed time every other day or something like that. Not based on soil_sensor)

#define PUMP_1_RELAY 1 // Relay for pump 1
#define PUMP_2_RELAY 2 // Relay for pump 2
#define PUMP_3_RELAY 3 // Relay for pump 3
#define PUMP_4_RELAY 4 // Relay for pump 4

#define PUMP_1_PIN_NUM 13 // DIG OUT PIN for pump 1
#define PUMP_2_PIN_NUM 12 // DIG OUT PIN for pump 2
#define PUMP_3_PIN_NUM 11  // DIG OUT PIN for pump 3
#define PUMP_4_PIN_NUM 10  // DIG OUT PIN for pump 4

#define SOIL_SENSOR_1_OUT_PIN_NUM 7 // DIG OUT PIN for soil sensor 1
#define SOIL_SENSOR_2_OUT_PIN_NUM 6 // DIG OUT PIN for soil sensor 2
#define SOIL_SENSOR_3_OUT_PIN_NUM 5 // DIG OUT PIN for soil sensor 3
#define SOIL_SENSOR_4_OUT_PIN_NUM 4 // DIG OUT PIN for soil sensor 4

#define SOIL_SENSOR_1_IN_PIN_NUM 1 // ANALOG IN PIN for soil sensor 1
#define SOIL_SENSOR_2_IN_PIN_NUM 2 // ANALOG IN PIN for soil sensor 2
#define SOIL_SENSOR_3_IN_PIN_NUM 3 // ANALOG IN PIN for soil sensor 3
#define SOIL_SENSOR_4_IN_PIN_NUM 4 // ANALOG IN PIN for soil sensor 4

#define SOILD_SENSOR_1_ID 1
#define SOILD_SENSOR_2_ID 2
#define SOILD_SENSOR_3_ID 3
#define SOILD_SENSOR_4_ID 4

#define SOIL_SENSOR_1_WET_LIMIT 500 // Threshold value for wet soil for soil sensor 1
#define SOIL_SENSOR_1_DRY_LIMIT 200 // Threshold value for dry soil for soil sensor 1
#define SOIL_SENSOR_2_WET_LIMIT 500 // Threshold value for wet soil for soil sensor 2
#define SOIL_SENSOR_2_DRY_LIMIT 200 // Threshold value for dry soil for soil sensor 2
#define SOIL_SENSOR_3_WET_LIMIT 500 // Threshold value for wet soil for soil sensor 3
#define SOIL_SENSOR_3_DRY_LIMIT 200 // Threshold value for dry soil for soil sensor 3
#define SOIL_SENSOR_4_WET_LIMIT 500 // Threshold value for wet soil for soil sensor 4
#define SOIL_SENSOR_4_DRY_LIMIT 200 // Threshold value for dry soil for soil sensor 4

#define SOIL_MOISTURE_DRY_LIMIT 30  // Dry limit in procentage. Soil value below this indicates that the soil needs water  

#define SYS_LED_PIN_NUM 2  // DIG OUT PIN for system LED
// TODO: define sd card pins here (current pins from example)
#define SD_CS_PIN 4
#define SD_MOSI_PIN 11
#define SD_MISO_PIN 12
#define SD_CLK_PIN 13

#define EXECUTION_TIME_INTERVAL_MS 60000 // Execution run time interval in ms. Defines how often program should execute
#define PUMP_ON_TIME_S 5                 // Time to keep the pump ON in seconds

// Declarations 
PumpRelay pump_relay_ctrl;

CapacitiveSoilSensor capacitive_soil_sensor_1(SOIL_SENSOR_1_IN_PIN_NUM, SOIL_SENSOR_1_OUT_PIN_NUM, SOILD_SENSOR_1_ID);
CapacitiveSoilSensor capacitive_soil_sensor_2(SOIL_SENSOR_2_IN_PIN_NUM, SOIL_SENSOR_2_OUT_PIN_NUM, SOILD_SENSOR_2_ID);
CapacitiveSoilSensor capacitive_soil_sensor_3(SOIL_SENSOR_3_IN_PIN_NUM, SOIL_SENSOR_3_OUT_PIN_NUM, SOILD_SENSOR_3_ID);
CapacitiveSoilSensor capacitive_soil_sensor_4(SOIL_SENSOR_4_IN_PIN_NUM, SOIL_SENSOR_4_OUT_PIN_NUM, SOILD_SENSOR_4_ID);

CapacitiveSoilSensor capacitive_soil_sensor_arr[4] = {capacitive_soil_sensor_1, capacitive_soil_sensor_2, capacitive_soil_sensor_3, capacitive_soil_sensor_4};

SdCardManager sd_card_mngr(SD_CS_PIN);
String sd_card_logfile_base_name = "sys_log_";
String sd_card_logfile_suffix = ".txt";
String sd_card_logfile_name;

// system data
uint64_t total_pump_activation_counter {0};
uint64_t total_number_of_executions_counter{0};

// Settings
bool automatic_pump_mode = false; 
bool test_mode = true;

void setup() {
  // Init serial 
  Serial.begin(9600);

  // Boot message
  Serial.println("dherslof - Indoor Plant Watering system is initializing");

  // Init system LED
  pinMode(SYS_LED_PIN_NUM, OUTPUT);
  // Visualize boot process
  digitalWrite(SYS_LED_PIN_NUM, HIGH);

  // Init pump relays 
  pump_relay_ctrl.setPinOutRelay1(PUMP_1_PIN_NUM);
  pump_relay_ctrl.setPinOutRelay2(PUMP_2_PIN_NUM);
  pump_relay_ctrl.setPinOutRelay3(PUMP_3_PIN_NUM);
  pump_relay_ctrl.setPinOutRelay4(PUMP_4_PIN_NUM);

  // Init soil sensors
  for (auto& soil_sensor : capacitive_soil_sensor_arr) {
    soil_sensor.setPwrOn();
  }

  if (sd_card_mngr.init()) {
    for (uint8_t n = 1; n < 100; n++) {
       sd_card_logfile_name = sd_card_logfile_base_name + n + sd_card_logfile_suffix;
       if (!sd_card_mngr.fileExists(sd_card_logfile_name)) {
          sd_card_mngr.createFile(sd_card_logfile_name);
          break;
       }
    }
  Serial.println("No SD card available, no logging will be done");
  }
  
    
  // Visualize boot process done
  digitalWrite(SYS_LED_PIN_NUM, LOW);
  delay(500);
  digitalWrite(SYS_LED_PIN_NUM, HIGH);
  delay(500);
  digitalWrite(SYS_LED_PIN_NUM, LOW);
  delay(500);
  digitalWrite(SYS_LED_PIN_NUM, HIGH);
  Serial.println("System initialized ");
}

void loop() {
    
  // Detect run mode
  if (automatic_pump_mode) {
    // Increase run counter
    total_number_of_executions_counter += 1;
  
    // Get start timestamp 
    const auto execution_start_time {millis()};
  
    // Read all sensor values in procentage, and start pump if value indicates dry soil 
    for (auto& soil_sensor : capacitive_soil_sensor_arr) {
      if (soil_sensor.currentMoisturePercentage() <= SOIL_MOISTURE_DRY_LIMIT) {
        const auto soil_sensor_id = soil_sensor.getSensorId();
        pump_relay_ctrl.setRelayOnForSpecifiedTime(soil_sensor_id, PUMP_ON_TIME_S);
        
        // Increase pump activation counter
        total_pump_activation_counter += 1; 
      }
    }
  
    // Get execution time stop time and determine potential sleep time
    const auto execution_stop_time {millis()};
    const auto execution_total_time {execution_stop_time - execution_start_time}; 
    if (execution_total_time < EXECUTION_TIME_INTERVAL_MS) {
      const auto delay_time {EXECUTION_TIME_INTERVAL_MS - execution_total_time};
      delay(delay_time);
    }
  // automatic_pump_mode
  } else if (test_mode) {
    // Test code parts here without modification to normal execution 

    // Verifiera om relät ska vara open or closed med riktig pump senare

    // Funkar
    //Serial.println("Setting relay 1 on 5s");
    //pump_relay_ctrl.setRelayOnForSpecifiedTime(PUMP_1_RELAY, 5);
    //Serial.println("done, sleeping 4s");
    //delay(4000);

    // funkar!
    //Serial.println("Setting relay 1 on");
    //pump_relay_ctrl.setRelayOn(PUMP_1_RELAY);
    //Serial.println("done, sleeping 5s");
    //delay(5000);
    //pump_relay_ctrl.setRelayOff(PUMP_1_RELAY);
    //Serial.println("done, sleeping 5s");
    //delay(5000);
  }



  
}
