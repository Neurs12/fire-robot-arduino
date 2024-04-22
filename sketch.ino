// Fire robot using Arduino. (Code FIREINTHEHOLE)
// Based on specifications from SPK-STEM & HCMUTE's paper.

// Might not be correct in some places due to limited resources.

// Constant pin values for inputs.
const int POWER_BUTTON = A0;

const int IR_RIGHT = A1;
const int IR_LEFT = A2;

const int FRONT_CONTACT = A3;

// Constant pin values for outputs.
const int RIGHT_WHEEL_FORWARD = 9;
const int RIGHT_WHEEL_BACKWARD = 10;

const int LEFT_WHEEL_FORWARD = 5;
const int LEFT_WHEEL_BACKWARD = 6;

const int NOZZLE_RAISE = 7;
const int NOZZLE_LOWER = 8;

const int SPRAY_STRONG = 11;
const int SPRAY_WEAK = 12;

const int ARDUINO_LED = 13;

void setupInputs() {
  // Start/stop button.
  pinMode(POWER_BUTTON, INPUT_PULLUP);

  // IR Sensors.
  // Right sensor.
  pinMode(IR_RIGHT, INPUT_PULLUP);
  // Left sensor.
  pinMode(IR_LEFT, INPUT_PULLUP);

  // Contact sensor.
  pinMode(FRONT_CONTACT, INPUT_PULLUP);
}

void setupOutputs() {
  // Right wheel functionalities.
  pinMode(RIGHT_WHEEL_FORWARD, OUTPUT);
  pinMode(RIGHT_WHEEL_BACKWARD, OUTPUT);
  
  // Left wheel functionalities.
  pinMode(LEFT_WHEEL_FORWARD, OUTPUT);
  pinMode(LEFT_WHEEL_FORWARD, OUTPUT);

  // Nozzle functionalities.
  pinMode(NOZZLE_RAISE, OUTPUT);
  // Lower the nozzle.
  pinMode(NOZZLE_LOWER, OUTPUT);
  // Strong spray.
  pinMode(SPRAY_STRONG, OUTPUT);
  //Weak spray.
  pinMode(SPRAY_WEAK, OUTPUT);

  // LED Indicator (custom) / Might be needed to indicates the power state.
  pinMode(ARDUINO_LED, OUTPUT);
}

void setup() {
  // Define input ports.
  setupInputs();
  
  // Define output ports.
  setupOutputs();
}

void loop() {
  if (digitalRead(POWER_BUTTON) == 0) {
    delay(1000);
    return;
  }
}
