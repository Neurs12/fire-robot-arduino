// Might not be correct due to limited resources.

// Constant pin values for inputs.
const byte POWER_BUTTON = A0;

const byte IR_RIGHT = A1;
const byte IR_LEFT = A2;

const byte FRONT_CONTACT = A3;

// Constant pin values for outputs.
const byte RIGHT_WHEEL_FORWARD = 9;
const byte RIGHT_WHEEL_BACKWARD = 10;

const byte LEFT_WHEEL_FORWARD = 5;
const byte LEFT_WHEEL_BACKWARD = 6;

const byte NOZZLE_RAISE = 7;
const byte NOZZLE_LOWER = 8;

const byte SPRAY_STRONG = 11;
const byte SPRAY_WEAK = 12;

const byte ARDUINO_LED = 13;

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
  pinMode(NOZZLE_LOWER, OUTPUT);
  pinMode(SPRAY_STRONG, OUTPUT);
  pinMode(SPRAY_WEAK, OUTPUT);

  // LED Indicator (custom) / Indicates the power state.
  pinMode(ARDUINO_LED, OUTPUT);
}

void setup() {
  // Define input ports.
  setupInputs();
  
  // Define output ports.
  setupOutputs();
}

// Program's states.
bool isRunning = true;

void loop() {
  powerButtonHandler();

  // Debug purpose
  Serial.println("A");
  // Main loop.
  delay(100);
}

// Power button states.
// Handle power button hold down event.
byte lastState = 1;

void powerButtonHandler() {
  byte latestState = digitalRead(POWER_BUTTON);
  if (latestState == 0 && latestState != lastState) {
    isRunning = !isRunning;
  }

  if (isRunning) {
    digitalWrite(ARDUINO_LED, HIGH);
  } else {
    digitalWrite(ARDUINO_LED, LOW);
    // Might leave it here to reduce load for the board.
    delay(1000);
  }

  lastState = latestState;
}
