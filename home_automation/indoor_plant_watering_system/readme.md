# Indoor Plant Watering System
Four pump system for autoamtic watering of indoor plants.

## Description 
The system reads the soil sensor, and activates the pump based on the value. Dry & wet values has to be 
configured for every plant independent. The pump activation time and run interval can also be configured 
to suitable values. 

Number of pump activations and runs will be stored in a log file on the SD card which makes it possible to 
read and evaluate at a later stage. 

## Parts
* Capacitive Soil Sensor
* Water pump
* Relay
* SD-card reader

For details, see BOM [here](doc/BOM.txt)

## Board
Current default board is UNO. Hardcoded HW PIN values. **EXAMPLE** wiring can be seen in the [doc](doc) directory (Note that pins due not match the code).
