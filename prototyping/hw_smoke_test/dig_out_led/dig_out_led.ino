// LED (digital out) test sketch. Serial consol used for debugging. 
#define LED_PIN 2
#define DELAY_TIME_MS 500
#define REPORT_LOOP_NUM 100

uint32_t loop_counter = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("LED test sketch setup done - starting!");
}

void loop() {
  loop_counter += 1;
  Serial.println("LED ON for 500ms");
  digitalWrite(LED_PIN, HIGH);
  delay(DELAY_TIME_MS);
  Serial.println("LED OFF for 500ms");
  digitalWrite(LED_PIN, LOW);
  delay(DELAY_TIME_MS);

  if ( (loop_counter % REPORT_LOOP_NUM) == 0) {
    Serial.print("ON/OFF loop counter = ");
    Serial.println(loop_counter);
  }
}
