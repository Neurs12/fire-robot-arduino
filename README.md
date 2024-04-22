# Fire robot implementation (Arduino Nano)
This code was written based on the paper by SPK-STEM and HCMUTE, This is my class's assignment.

A funny code for it: FIREINTHEHOLE

## Behind the scene
When you import the code to Arduino, the program will set `isRunning` to false, indicates that the program is not running.

**`TaskState` enum:**
```cpp
enum TaskState {
  FIND_OBJECT, SPRAY_WATER, BACK_UP, STOP
};
```

This will allows us to store the current state of the robot, and changing the action of the robot accordingly.

**Time related variables:**
```cpp
const int LOOP_DELAY = 100;
// Must be divisible by LOOP_DELAY. (x % LOOP_DELAY == 0)
const int NOZZLE_MOVEMENT_TIME = 1000;
const int SPRAY_TIME = 2000;
const int BACK_UP_TIME = 5000;
```

For a precise timing, all variables must be divisible by LOOP_DELAY to ensure that the time performing an action goes over the defined time.

**Constant pins instruction:**
```cpp
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
```

As defined in the paper, nothing much to discuss here.

**Control the states of the robot:**
```cpp
bool isObjectClose = false;
int nozzleMovedTime = 0;
int sprayedTime = 0;
int backedUpTime = 0;

byte powerButtonLastState = 1;
bool powerLastState = false;
TaskState currentTask = TaskState::FIND_OBJECT;
bool isRunning = false;
```

The first four must be reset when a power off occurs (reset). This will control how the Arduino remembers the current actions and execute it.

The last four is for power button handling and stating the current task and if the robot is running or not.

**Reset function:**
```cpp
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
```

While the robot's running, there might be lots of events happening right at that moment, so we have to shut every operations, when comes to the nozzle, we'll just lower it down, while blocking the board from receiving any input while doing this.

**Move wheels function:**
```cpp
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
```

If the input is lower than 0, that means we're going backward, bigger than 0 is forward. But if the input is 0, stop the wheel.

**Setup functions:**
```cpp
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
```

Setup for each pins.

**Find object:**
```cpp
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
```

- First, it will read from the two IR sensor, then the contact sensor.
- Before perform any moving action, check if the two IR sensor both detected something or the contact sensor detects anything, based on this information, we can provide it to the spray water function to adjust the strength of the spray.
- Check for each IR sensor, if something is blocking the way, don't go there, spin using the other wheel.
- Return -1, that means nothing.

**Spray water function:**
```cpp
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
```

- First, it will check if the sprayed time is bigger than the given time or not, if it's bigger, reset the timer on both the nozzle and spray, then disable the spray mode based on `strongMode` boolean value.
- Then it'll check if the nozzle moved time is smaller than given or not, if yes, continue set it to `HIGH` to keep raising.
- When the condition is broken, disable the raising pin.
- Then enable the water spray, based on `strongMode` boolean value.
- Handles when the object is not in fornt of us, diable the nozzle and go back to find the object.
- return true means the function has finished its job, false means the function has yet to be done.

**Back up function:**
```cpp
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
```

- This will make the robot move backward based on the time given, that's all.

**Power button handler:**
```cpp
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
```

- It'll read the power button.
- Check the current state with the previous state, if it's valid, switch the `isRunning` value.
- Then check `isRunning` and `powerLastState` for the previous power state, so we'll know that it's actually the first time the power state has changed, avoiding getting called over and over again.
- Set the `currentTask` to the first task and turn on the LED built in Arduino board.
- If it's not running, check if the state is already stopped or not to avoid loop call, if not stopped, set it to `STOP`, turn of the LED and call reset function since this function will block everything while lower the nozzle.

**Setup function:**
```cpp
void setup() {
  // Sync with standard baud rate.
  Serial.begin(9600);

  // Define input ports.
  setupInputs();
  // Define output ports.
  setupOutputs();
}
```

Simple one, using provided function from above.

**Loop function:**
```cpp
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
```

- Everytime loop, check for power button for new state.
- Then check `isRunning`.
- If true, run the current task.
- In every tasks, there's a result variable assigned with the according function provided to the given task.
- Everytime a task is complete, change the current task to the next one.
- For `sprayWater` function, it will also check and revert back to task `FIND_OBJECT` if the object is no longer there.

And that's pretty much it!
