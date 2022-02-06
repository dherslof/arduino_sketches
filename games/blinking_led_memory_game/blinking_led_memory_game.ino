

/* Board: UNO
 *
 * Description: A simple memory game!
 *              Game starts at level 1. The LEDs will light up in
 *              a random order, press the buttons in the same order to advance to
 *              next level. The sequence will grow for each level.
 *
 * Instructions: When the arduino boots up it will be in idle, this can be seen by all the
 *               LEDs are being ON. Press the button 1 (button most to the left on development board) to start.
 *               If the sequence which the buttons are entered correct, same as the LED pattern, a 'winning' sequence is displayed
 *               on the LEDs and after a couple of seconds the next round will start. If wrong sequence is entered, you will have
 *               a last try. If you would fail on this attemt as well, the LEDs will do a game-over sequence and go back to idle.
 *
 *               Would you like to end, or reset the game, hold down the button 1 for a minimum of 1,5 seconds. After this the game will go
 *               back to idle.
 *
 *               Good Luck!
 *
 * Author: Daniel Herslöf <d_herslof@hotmail.com>
 */

// Button Pins
const uint8_t BUTTON_1 = 5;
const uint8_t BUTTON_2 = 4;
const uint8_t BUTTON_3 = 3;
const uint8_t BUTTON_4 = 2;

// LED Pins
const uint8_t LED1 = 11;
const uint8_t LED2 = 10;
const uint8_t LED3 = 9;
const uint8_t LED4 = 8;

// Level sequence length
const uint8_t LEVEL_1_AMOUNT = 2;
const uint8_t LEVEL_2_AMOUNT = 5;
const uint8_t LEVEL_3_AMOUNT = 8;
const uint8_t LEVEL_4_AMOUNT = 12;
const uint8_t LEVEL_5_AMOUNT = 15;

enum GameState : uint8_t {
  IDLE,
  GAME_ONGOING
};

// Game variables
uint8_t level = 1;
uint8_t input_num = 0;
uint8_t tries_left = 2;
GameState state = GameState::IDLE;

bool level_1_done = false;
bool level_2_done = false;
bool level_3_done = false;
bool level_4_done = false;
bool level_5_done = false;


// Lvl 1
uint8_t level_1_generated_sequence[LEVEL_1_AMOUNT];
uint8_t level_1_input_sequence[LEVEL_1_AMOUNT];

// Lvl 2
uint8_t level_2_generated_sequence[LEVEL_2_AMOUNT];
uint8_t level_2_input_sequence[LEVEL_2_AMOUNT];

// Lvl 3
uint8_t level_3_generated_sequence[LEVEL_3_AMOUNT];
uint8_t level_3_input_sequence[LEVEL_3_AMOUNT];

// Lvl 4
uint8_t level_4_generated_sequence[LEVEL_4_AMOUNT];
uint8_t level_4_input_sequence[LEVEL_4_AMOUNT];

// Lvl 5
uint8_t level_5_generated_sequence[LEVEL_5_AMOUNT];
uint8_t level_5_input_sequence[LEVEL_5_AMOUNT];

// Helper variables
const uint16_t long_press_time = 1500;

enum ButtonPressTime : uint8_t {
  SHORT,
  LONG,
  NOT_PRESSED,
};

struct buttonPress {
  bool isPressed = false;
  ButtonPressTime press_time = ButtonPressTime::NOT_PRESSED;
};

// Get the number of user inputs allowed. 0 if unknown.
uint8_t getLevelInputLen(const uint8_t current_level) {
  switch(current_level) {
    case 1:
      return LEVEL_1_AMOUNT;
    case 2:
      return LEVEL_2_AMOUNT;
    case 3:
      return LEVEL_3_AMOUNT;
    case 4:
      return LEVEL_4_AMOUNT;
    case 5:
      return LEVEL_5_AMOUNT;
    default:
      Serial.print("Unknown level: ");
      Serial.print(current_level);
      Serial.println(", unable to get level input lenght");
      break;
  }
  return 0;
}

void allLedsOn() {
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED4, HIGH);
}

void allLedsOff() {
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);
}

// LED blinking sequence when level failed - game over
void levelFailedLedSequence() {
  for(uint8_t n = 0; n <= 5; n++) {
    allLedsOff();
    delay(200);
    allLedsOn();
    delay(200);
  }
}

// LED blinking sequence when level failed - game over
void newLevelLedSequence() {
  allLedsOff();

  delay(1000);
  digitalWrite(LED4, HIGH);
  delay(200);
  digitalWrite(LED3, HIGH);
  delay(200);
  digitalWrite(LED2, HIGH);
  delay(200);
  digitalWrite(LED1, HIGH);
  delay(1000);
  digitalWrite(LED1, LOW);
  delay(200);
  digitalWrite(LED2, LOW);
  delay(200);
  digitalWrite(LED3, LOW);
  delay(200);
  digitalWrite(LED4, LOW);
  delay(200);
  digitalWrite(LED1, LOW);
  delay(1000);
}

// Get the sequence array length
uint8_t getSequenceSize(const uint8_t level) {
  uint8_t sequence_size = 0;
  switch(level) {
    case 1:
      sequence_size = LEVEL_1_AMOUNT;
      break;
    case 2:
      sequence_size = LEVEL_2_AMOUNT;
      break;
    case 3:
      sequence_size = LEVEL_3_AMOUNT;
      break;
    case 4:
      sequence_size = LEVEL_4_AMOUNT;
      break;
    case 5:
      sequence_size = LEVEL_5_AMOUNT;
      break;
    default:
      Serial.print("Unknown sequence size for level:");
      Serial.println(level);
      break;

  }
  return sequence_size;
}

// Generate the LED sequence for level
void generateLevelSequence(const uint8_t current_level) {

  Serial.print("Generating sequence for level: ");
  Serial.println(current_level);

  switch(current_level) {
    case 1:
      for (uint8_t n = 0; n < LEVEL_1_AMOUNT; n++) {
        const uint8_t led_nr = random(1,  5); // Random number from 1 to 4
        Serial.print("random led: ");
        Serial.println(led_nr);
        level_1_generated_sequence[n] = led_nr;
      }
      break;
    case 2:
      for (uint8_t n = 0; n < LEVEL_2_AMOUNT; n++) {
        const uint8_t led_nr = random(1,  5); // Random number from 1 to 4
        Serial.print("random led: ");
        Serial.println(led_nr);
        level_2_generated_sequence[n] = led_nr;
      }
      break;
   case 3:
      for (uint8_t n = 0; n < LEVEL_3_AMOUNT; n++) {
        const uint8_t led_nr = random(1,  5); // Random number from 1 to 4
        Serial.print("random led: ");
        Serial.println(led_nr);
        level_3_generated_sequence[n] = led_nr;
      }
      break;
    case 4:
      for (uint8_t n = 0; n < LEVEL_4_AMOUNT; n++) {
        const uint8_t led_nr = random(1,  5); // Random number from 1 to 4
        Serial.print("random led: ");
        Serial.println(led_nr);
        level_4_generated_sequence[n] = led_nr;
      }
      break;
    case 5:
      for (uint8_t n = 0; n < LEVEL_5_AMOUNT; n++) {
        const uint8_t led_nr = random(1,  5); // Random number from 1 to 4
        Serial.print("random led: ");
        Serial.println(led_nr);
        level_5_generated_sequence[n] = led_nr;
      }
      break;
   default:
      Serial.println("Unkwon level - unable to generate sequence");
  }
}

// Blink selected LED
void blinkLed(const uint8_t led_nr) {
  allLedsOff();
  delay(200);

  switch(led_nr) {
    case 0:
      Serial.println("No LED with index 0");
    case 1:
      digitalWrite(LED1, HIGH);
      break;
    case 2:
      digitalWrite(LED2, HIGH);
      break;
    case 3:
      digitalWrite(LED3, HIGH);
      break;
    case 4:
      digitalWrite(LED4, HIGH);
      break;
    default:
      Serial.println("Unknown number of LEDS! Expecteded number is 4, higher number requested");
      break;
  }

  delay(500);
  allLedsOff();
}

// Blink the LED sequence for the user to remember
void blinkLevelLedSequence(const uint8_t level) {
  auto nr_of_blinks = getSequenceSize(level);

  switch(level) {
    case 1:
      for(uint8_t n = 0; n < nr_of_blinks; n++) {
        //Serial.print("Blinking led nr: ");
        //Serial.println(level_1_generated_sequence[n]);
        blinkLed(level_1_generated_sequence[n]);
      }
      break;
    case 2:
      for(uint8_t n = 0; n < nr_of_blinks; n++) {
        //Serial.print("Blinking led nr: ");
        //Serial.println(level_2_generated_sequence[n]);
        blinkLed(level_2_generated_sequence[n]);
      }
      break;
    case 3:
      for(uint8_t n = 0; n < nr_of_blinks; n++) {
        //Serial.print("Blinking led nr: ");
        //Serial.println(level_3_generated_sequence[n]);
        blinkLed(level_3_generated_sequence[n]);
      }
      break;
    case 4:
      for(uint8_t n = 0; n < nr_of_blinks; n++) {
        //Serial.print("Blinking led nr: ");
        //Serial.println(level_4_generated_sequence[n]);
        blinkLed(level_4_generated_sequence[n]);
      }
      break;
    case 5:
      for(uint8_t n = 0; n < nr_of_blinks; n++) {
        //Serial.print("Blinking led nr: ");
        //Serial.println(level_5_generated_sequence[n]);
        blinkLed(level_5_generated_sequence[n]);
      }
      break;
    default:
      Serial.println("Unknown level - unable to blink sequence");
      break;
  }

  Serial.print("LED blinking done for level: ");
  Serial.println(level);
}

// Start next level
void nextLevel(const uint8_t level) {
  newLevelLedSequence();
  delay(1000);
  generateLevelSequence(level);
  Serial.println("level sequence generated");

  blinkLevelLedSequence(level);
}

// Retry level if any tries left, if not display level fail and go back to idle
void retry_level(const uint8_t level) {

  tries_left -= 1;
  if (tries_left == 0) {
    Serial.println("No tries left. Game Over!");
    levelFailedLedSequence();
    resetGameToIdle();
    return;
  }

  delay(500);
  allLedsOn();
  delay(300);
  allLedsOff();
  delay(500);

  switch(level) {
    case 1:
      memset(level_1_input_sequence, 0, sizeof(level_1_input_sequence));
      break;
    case 2:
      memset(level_2_input_sequence, 0, sizeof(level_2_input_sequence));
      break;
    case 3:
      memset(level_3_input_sequence, 0, sizeof(level_3_input_sequence));
      break;
    case 4:
      memset(level_4_input_sequence, 0, sizeof(level_4_input_sequence));
      break;
    case 5:
      memset(level_5_input_sequence, 0, sizeof(level_5_input_sequence));
      break;
    default:
      Serial.println("Received unknown level, unable to reset level for next try");
      break;
  }

  blinkLevelLedSequence(level);
  input_num = 0;
}

// Start a new game
void startNewGame() {
  Serial.println("new game starting");

  state = GameState::GAME_ONGOING;
  tries_left = 2;
  nextLevel(level);
}

// Level complete - Trigger starting of next level
void levelComplete(const uint8_t level_done) {
  Serial.print("Level: ");
  Serial.print(level_done);
  Serial.println(" completed");

  level += 1;
  input_num = 0;
  tries_left = 2;

  if(level == 6) {
    Serial.println("Level 5 completed - maximum level reached");
    resetGameToIdle();
    return;
  }

  Serial.print("Starting level: ");
  Serial.println(level);
  nextLevel(level);
}

// Reset all game variables and go back to idle state
void resetGameToIdle() {
  state = GameState::IDLE;
  level = 1;
  input_num = 0;
  tries_left = 2;

  memset(level_1_generated_sequence, 0, sizeof(level_1_generated_sequence));
  level_1_done = false;
  memset(level_2_generated_sequence, 0, sizeof(level_2_generated_sequence));
  level_2_done = false;
  memset(level_3_generated_sequence, 0, sizeof(level_3_generated_sequence));
  level_3_done = false;
  memset(level_4_generated_sequence, 0, sizeof(level_4_generated_sequence));
  level_4_done = false;
  memset(level_5_generated_sequence, 0, sizeof(level_5_generated_sequence));
  level_5_done = false;

  allLedsOff();
  delay(1000);
  allLedsOn();
  Serial.println("Reset game to idle - DONE");

}

// Function for checking if button is pressed
buttonPress isButtonPressed(const uint8_t button) {
  uint16_t pressed_time = 0;
  buttonPress button_pressed;

  while (digitalRead(button) == HIGH) {
    delay(20); // debounce
    pressed_time += 50;
    Serial.print("Button pin #");
    Serial.print(button);
    Serial.print(" press-time: ");
    Serial.println(pressed_time);
  }

  if (pressed_time > 0) {
    if (pressed_time <= long_press_time) {
      button_pressed.isPressed = true;
      button_pressed.press_time = ButtonPressTime::SHORT;
    } else {
      button_pressed.isPressed = true;
      button_pressed.press_time = ButtonPressTime::LONG;
    }
  }
   return button_pressed;
}

// Read buttons and return the one being pressed - otherwise 0
uint8_t readButtons() {
  const auto b1 = isButtonPressed(BUTTON_1);
  if (b1.isPressed) {
    return 1;
  }

  const auto b2 = isButtonPressed(BUTTON_2);
  if (b2.isPressed) {
    return 2;
  }

  const auto b3 = isButtonPressed(BUTTON_3);
  if (b3.isPressed) {
    return 3;
  }

  const auto b4 = isButtonPressed(BUTTON_4);
  if (b4.isPressed) {
    return 4;
  }

  // No button pressed
  return 0;
}

// Compare the input sequence compared to the generated - return false if not the same
bool isInputSequenceSameAsGenerated(const uint8_t level) {

  // Compare arrays for current level
  switch(level) {
    case 1:
      for(uint8_t n = 0; n < LEVEL_1_AMOUNT; n++) {
        // return if not the same
        if (level_1_input_sequence[n] != level_1_generated_sequence[n]) {
          Serial.print("Encountered wrong input: ");
          Serial.print(level_1_input_sequence[n]);
          Serial.print(", expected: ");
          Serial.println(level_1_generated_sequence[n]);
          return false;
        }
      }
      break;
    case 2:
      for(uint8_t n = 0; n < LEVEL_2_AMOUNT; n++) {
        if (level_2_input_sequence[n] != level_2_generated_sequence[n]) {
          Serial.print("Encountered wrong input: ");
          Serial.print(level_2_input_sequence[n]);
          Serial.print(", expected: ");
          Serial.println(level_2_generated_sequence[n]);
          return false;
        }
      }
      break;
    case 3:
      for(uint8_t n = 0; n < LEVEL_3_AMOUNT; n++) {
        if (level_3_input_sequence[n] != level_3_generated_sequence[n]) {
          Serial.print("Encountered wrong input: ");
          Serial.print(level_3_input_sequence[n]);
          Serial.print(", expected: ");
          Serial.println(level_3_generated_sequence[n]);
          return false;
        }
      }
      break;
    case 4:
      for(uint8_t n = 0; n < LEVEL_4_AMOUNT; n++) {
        if (level_4_input_sequence[n] != level_4_generated_sequence[n]) {
          Serial.print("Encountered wrong input: ");
          Serial.print(level_4_input_sequence[n]);
          Serial.print(", expected: ");
          Serial.println(level_4_generated_sequence[n]);
          return false;
        }
      }
      break;
    case 5:
      for(uint8_t n = 0; n < LEVEL_5_AMOUNT; n++) {
        if (level_5_input_sequence[n] != level_5_generated_sequence[n]) {
          Serial.print("Encountered wrong input: ");
          Serial.print(level_5_input_sequence[n]);
          Serial.print(", expected: ");
          Serial.println(level_5_generated_sequence[n]);
          return false;
        }
      }
      break;
    default:
      Serial.println("Received unknown level, unable to compare input against generated sequence");
      break;
  }

  return true;
}



void setup() {

  pinMode(BUTTON_1, INPUT);
  pinMode(BUTTON_2, INPUT);
  pinMode(BUTTON_3, INPUT);
  pinMode(BUTTON_4, INPUT);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);

  // Debug
  Serial.begin(115200);

  allLedsOn();
}



void loop() {

  // Check if in idle or game ongoing
  if (state == GameState::IDLE) {
    // check if a new game should start
    const auto button_pressed = isButtonPressed(BUTTON_1);
    if (button_pressed.isPressed) {

      Serial.println("Starting new game");
      startNewGame();
    }
  // Game ongoing
  } else {

    const auto ctrl_button_pressed = isButtonPressed(BUTTON_1);

    // Check if ctrl button has been long-pressed
    if (ctrl_button_pressed.isPressed && ctrl_button_pressed.press_time == ButtonPressTime::LONG) {
      // Ctrl-button long pressed - reset game to idle
      Serial.println("Ctrl-button long press - reset to idle");
      resetGameToIdle();
    }

    // Get user input
    uint8_t active_button = 0;
    if (ctrl_button_pressed.isPressed && ctrl_button_pressed.press_time == ButtonPressTime::SHORT) {
      active_button = 1;
    } else {
      active_button = readButtons();
    }

    // No active button
    const auto max_input_len = getLevelInputLen(level);
    if (active_button == 0 && input_num < max_input_len) {
      return;
    }

    // Switch handling dependent on current level
    switch(level) {
      case 1:

        if (input_num == LEVEL_1_AMOUNT && level_1_done == false) {
          Serial.println("Received all user input for level 1 - comparing");

          // Level failed
          if (!isInputSequenceSameAsGenerated(level)) {
            retry_level(level);
            return;
          }

          // Level complete
          levelComplete(level);
          level_1_done = true;
          return;

        } else {

          // Increment input number by 1
          input_num += 1;
          Serial.print("Input nr: ");
          Serial.println(input_num);

          // Get input position in input sequence array
          const uint8_t input_pos = input_num - 1;
          Serial.print("level1 - Adding user input from button: ");
          Serial.print(active_button);
          Serial.print(", at pos: ");
          Serial.println(input_pos);

          // Add user pressed button to the input sequence array
          level_1_input_sequence[input_pos] = active_button;
        }
        break;
      case 2:

        if (input_num == LEVEL_2_AMOUNT && level_2_done == false) {
          Serial.println("Received all user input for level2 - comparing");
          Serial.print("input:");
          for (uint8_t n = 0; n < LEVEL_2_AMOUNT; n++) {
            Serial.print(level_2_input_sequence[n]);
            Serial.print(", ");
          }
          Serial.println(" ");

          // Level failed
          if (!isInputSequenceSameAsGenerated(level)) {
            retry_level(level);
            return;
          }

          // Level complete
          levelComplete(level);
          level_2_done = true;
          return;

        } else {
          // Increment input number by 1
          input_num += 1;
          Serial.print("Input nr: ");
          Serial.println(input_num);

          // Get input position in input sequence array
          const uint8_t input_pos = input_num - 1;
          Serial.print("level2 - Adding user input from button: ");
          Serial.print(active_button);
          Serial.print(", at pos: ");
          Serial.println(input_pos);

          // Add user pressed button to the input sequence array
          level_2_input_sequence[input_pos] = active_button;
        }
        break;
      case 3:

        if (input_num == LEVEL_3_AMOUNT && level_3_done == false) {
          Serial.println("Received all user input for level3 - comparing");
          Serial.print("input:");
          for (uint8_t n = 0; n < LEVEL_3_AMOUNT; n++) {
            Serial.print(level_3_input_sequence[n]);
            Serial.print(", ");
          }
          Serial.println(" ");

          // Level failed
          if (!isInputSequenceSameAsGenerated(level)) {
            retry_level(level);
            return;
          }

          // Level complete
          levelComplete(level);
          level_3_done = true;
          return;

        } else {
          // Increment input number by 1
          input_num += 1;
          Serial.print("Input nr: ");
          Serial.println(input_num);

          // Get input position in input sequence array
          const uint8_t input_pos = input_num - 1;
          Serial.print("level3 - Adding user input from button: ");
          Serial.print(active_button);
          Serial.print(", at pos: ");
          Serial.println(input_pos);

          // Add user pressed button to the input sequence array
          level_3_input_sequence[input_pos] = active_button;
        }
        break;
      case 4:

        if (input_num == LEVEL_4_AMOUNT && level_4_done == false) {
          Serial.println("Received all user input for level4 - comparing");
          Serial.print("input:");
          for (uint8_t n = 0; n < LEVEL_4_AMOUNT; n++) {
            Serial.print(level_4_input_sequence[n]);
            Serial.print(", ");
          }
          Serial.println(" ");

          // Level failed
          if (!isInputSequenceSameAsGenerated(level)) {
            retry_level(level);
            return;
          }

          // Level complete
          levelComplete(level);
          level_4_done = true;
          return;

        } else {
          // Increment input number by 1
          input_num += 1;
          Serial.print("Input nr: ");
          Serial.println(input_num);

          // Get input position in input sequence array
          const uint8_t input_pos = input_num - 1;
          Serial.print("level4 - Adding user input from button: ");
          Serial.print(active_button);
          Serial.print(", at pos: ");
          Serial.println(input_pos);

          // Add user pressed button to the input sequence array
          level_4_input_sequence[input_pos] = active_button;
        }
        break;
      case 5:

        if (input_num == LEVEL_5_AMOUNT && level_5_done == false) {
          Serial.println("Received all user input for level5 - comparing");
          Serial.print("input:");
          for (uint8_t n = 0; n < LEVEL_5_AMOUNT; n++) {
            Serial.print(level_5_input_sequence[n]);
            Serial.print(", ");
          }
          Serial.println(" ");

          // Level failed
          if (!isInputSequenceSameAsGenerated(level)) {
            retry_level(level);
            return;
          }

          // Level complete - game done
          Serial.println("Maximum level reached! Congratulation! - resetting back to IDLE");
          resetGameToIdle();
          return;

        } else {
          // Increment input number by 1
          input_num += 1;
          Serial.print("Input nr: ");
          Serial.println(input_num);

          // Get input position in input sequence array
          const uint8_t input_pos = input_num - 1;
          Serial.print("level5 - Adding user input from button: ");
          Serial.print(active_button);
          Serial.print(", at pos: ");
          Serial.println(input_pos);

          // Add user pressed button to the input sequence array
          level_5_input_sequence[input_pos] = active_button;
        }
        break;
    } // Level switch
  } // Game ongoing
} // Main loop
