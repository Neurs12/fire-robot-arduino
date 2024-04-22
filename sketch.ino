/**
  Fire Robot Arduino

  Copyright (C) 2024, Neurs.
  Released under the MIT License.

  https://github.com/Neurs12/fire-robot-arduino
*/

// Might not be correct due to limited resources.


// Time related variables
// Delay time for each cycle in main loop.
const int LOOP_DELAY = 100;
const int SPRAY_TIME = 5000;
const int BACK_UP_TIME = 5000;

// Enums to triggers different types of robot's functions
enum TaskState {
  FIND_OBJECT, SPRAY_WATER, BACK_UP, STOP
};

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

void moveWheels(short rightWheel, short leftWheel) {
  if (rightWheel != 0) {
    analogWrite(rightWheel > 0 ? RIGHT_WHEEL_FORWARD : RIGHT_WHEEL_BACKWARD, abs(rightWheel));
  } else {
    analogWrite(RIGHT_WHEEL_FORWARD, 0);
    analogWrite(RIGHT_WHEEL_BACKWARD, 0);
  }

  if (leftWheel != 0) {
    analogWrite(leftWheel > 0 ? LEFT_WHEEL_FORWARD : LEFT_WHEEL_BACKWARD, abs(leftWheel));
  } else {
    analogWrite(LEFT_WHEEL_FORWARD, 0);
    analogWrite(LEFT_WHEEL_BACKWARD, 0);
  }
}

void setupInputs() {
  // Start/stop button.
  pinMode(POWER_BUTTON, INPUT_PULLUP);

  // IR Sensors.
  pinMode(IR_RIGHT, INPUT_PULLUP);
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
  pinMode(LEFT_WHEEL_BACKWARD, OUTPUT);

  // Nozzle functionalities.
  pinMode(NOZZLE_RAISE, OUTPUT);
  pinMode(NOZZLE_LOWER, OUTPUT);
  pinMode(SPRAY_STRONG, OUTPUT);
  pinMode(SPRAY_WEAK, OUTPUT);

  // LED Indicator (custom) / Indicates the power state.
  pinMode(ARDUINO_LED, OUTPUT);
}

void setup() {
  Serial.begin(9600);

  // Define input ports.
  setupInputs();
  // Define output ports.
  setupOutputs();
}

// Program's states.
bool isRunning = true;
TaskState currentTask = TaskState::FIND_OBJECT;

void loop() {
  powerButtonHandler();

  if (isRunning) {
    if (currentTask == TaskState::FIND_OBJECT) {
      bool result = findObject();

      // Object found, move to spray water mode.
      if (result) {
        currentTask = TaskState::SPRAY_WATER;
      }
    }
  }

  // Main loop.
  delay(LOOP_DELAY);
}

bool findObject() {
  byte irRightRead = digitalRead(IR_RIGHT);
  byte irLeftRead = digitalRead(IR_LEFT);
  byte contactRead = digitalRead(FRONT_CONTACT);

  if (irRightRead && irLeftRead) {
    moveWheels(150, 150);
  }
  if (irRightRead && !irLeftRead) {
    moveWheels(150, 0);
  }
  if (!irRightRead && irLeftRead) {
    moveWheels(0, 150);
  }

  // Object found, return true for further instructions from mainloop.
  if ((!irRightRead && !irLeftRead) || !contactRead) {
    moveWheels(0, 0);
    return true;
  }

  return false;
}

// Power button states.
// Handle power button hold down event.
byte powerButtonLastState = 1;

void powerButtonHandler() {
  byte currentState = digitalRead(POWER_BUTTON);

  if (!currentState && currentState != powerButtonLastState) {
    isRunning = !isRunning;
  }

  if (isRunning) {
    digitalWrite(ARDUINO_LED, HIGH);
  } else {
    digitalWrite(ARDUINO_LED, LOW);
    // Might leave it here to reduce load for the board.
    delay(1000);
  }

  powerButtonLastState = currentState;
}
