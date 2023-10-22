// Push button (digital in) test sketch. Serial consol used for debugging. 
#define PUSH_BTN_PIN 2
#define REPORT_LOOP_NUM 40000

uint32_t loop_counter = 0;
int8_t push_btn_state = 0; // Assume no press as default

void setup() {
  Serial.begin(9600);
  pinMode(PUSH_BTN_PIN, INPUT);
  Serial.println("Push button test sketch setup done - starting!");
}

void loop() {
  loop_counter += 1;
  
  push_btn_state = digitalRead(PUSH_BTN_PIN);

  if (push_btn_state == HIGH) {
    Serial.println("Butten press detected");
  }

  if ((loop_counter % REPORT_LOOP_NUM) == 0) {
    Serial.println("Program running, still monitoring push button");
    loop_counter = 0;
  }
}
