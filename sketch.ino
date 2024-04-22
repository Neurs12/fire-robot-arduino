/**
  Fire Robot Arduino

  Copyright (C) 2024, Neurs.
  Released under the MIT License.

  https://github.com/Neurs12/fire-robot-arduino
*/

// Might not be correct due to limited resources.

// Enums to triggers different types of robot's functions
enum TaskState {
  FIND_OBJECT, SPRAY_WATER, BACK_UP, STOP
};

// Time related variables / Delay time for each cycle in main loop.
const int LOOP_DELAY = 100;
// Must be divisible by LOOP_DELAY. (x % LOOP_DELAY == 0)
const int NOZZLE_MOVEMENT_TIME = 1000;
const int SPRAY_TIME = 2000;
const int BACK_UP_TIME = 5000;

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

// Variables to control the states of the robot.
bool isObjectClose = false;
int nozzleMovedTime = 0;
int sprayedTime = 0;
int backedUpTime = 0;

byte powerButtonLastState = 1;
bool powerLastState = false;
TaskState currentTask = TaskState::FIND_OBJECT;
bool isRunning = true;

void reset() {
  isObjectClose = false;
  nozzleMovedTime = 0;
  sprayedTime = 0;
  backedUpTime = 0;

  moveWheels(0, 0);

  digitalWrite(SPRAY_STRONG, LOW);
  digitalWrite(SPRAY_WEAK, LOW);

  digitalWrite(NOZZLE_LOWER, HIGH);
  delay(NOZZLE_MOVEMENT_TIME);
  digitalWrite(NOZZLE_LOWER, LOW);
}

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

short findObject() {
  byte irRightRead = digitalRead(IR_RIGHT);
  byte irLeftRead = digitalRead(IR_LEFT);
  byte contactRead = digitalRead(FRONT_CONTACT);

  // 1: Object might far way.
  if (!irRightRead && !irLeftRead) {
    moveWheels(0, 0);
    return 1;
  }

  // 0: Object right in front the robot.
  if (!contactRead) {
    moveWheels(0, 0);
    return 0;
  }

  if (irRightRead && irLeftRead) {
    moveWheels(150, 150);
  }
  if (irRightRead && !irLeftRead) {
    moveWheels(150, 0);
  }
  if (!irRightRead && irLeftRead) {
    moveWheels(0, 150);
  }

  return -1;
}

bool sprayWater(bool strongMode) {
  if (sprayedTime >= SPRAY_TIME) {
    nozzleMovedTime = 0;
    sprayedTime = 0;
    digitalWrite(strongMode ? SPRAY_STRONG : SPRAY_WEAK, LOW);
    return true;
  }

  if (nozzleMovedTime < NOZZLE_MOVEMENT_TIME) {
    digitalWrite(NOZZLE_RAISE, HIGH);
    nozzleMovedTime += LOOP_DELAY;

    return false;
  }
  digitalWrite(NOZZLE_RAISE, LOW);

  digitalWrite(strongMode ? SPRAY_STRONG : SPRAY_WEAK, HIGH);
  sprayedTime += LOOP_DELAY;

  // The object somehow moved, go back to find object action.
  if (findObject() == -1) {
    sprayedTime = 0;
    digitalWrite(strongMode ? SPRAY_STRONG : SPRAY_WEAK, LOW);
    digitalWrite(NOZZLE_RAISE, LOW);
    currentTask = TaskState::FIND_OBJECT;
  }

  return false;
}

bool backUp() {
  moveWheels(-150, -150);
  backedUpTime += LOOP_DELAY;

  if (backedUpTime >= BACK_UP_TIME) {
    moveWheels(0, 0);
    backedUpTime = 0;

    return true;
  }

  return false;
}

void powerButtonHandler() {
  byte currentState = digitalRead(POWER_BUTTON);

  if (!currentState && currentState != powerButtonLastState) {
    isRunning = !isRunning;
  }

  if (isRunning) {
    if (powerLastState != isRunning) {
      currentTask = TaskState::FIND_OBJECT;
      digitalWrite(ARDUINO_LED, HIGH);
    }
  } else {
    if (currentTask != TaskState::STOP) {
      currentTask = TaskState::STOP;
      digitalWrite(ARDUINO_LED, LOW);
      reset();
    }

    delay(500);
  }

  powerButtonLastState = currentState;
  powerLastState = isRunning;
}

void setup() {
  // Sync with standard baud rate.
  Serial.begin(9600);

  // Define input ports.
  setupInputs();
  // Define output ports.
  setupOutputs();
}

void loop() {
  powerButtonHandler();

  if (isRunning) {
    if (currentTask == TaskState::FIND_OBJECT) {
      short result = findObject();

      // Object found, move to spray water action.
      if (result > -1) {
        isObjectClose = result == 1;
        currentTask = TaskState::SPRAY_WATER;
      }
    }

    if (currentTask == TaskState::SPRAY_WATER) {
      bool result = sprayWater(isObjectClose);

      // Spray finished, move to back up action.
      if (result) {
        currentTask = TaskState::BACK_UP;
      }
    }

    if (currentTask == TaskState::BACK_UP) {
      bool result = backUp();

      // All actions finished, stop the robot, waiting for next restart.
      if (result) {
        currentTask = TaskState::STOP;
      }
    }

  }

  // Main loop.
  delay(LOOP_DELAY);
}
