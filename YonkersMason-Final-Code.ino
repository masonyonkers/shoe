/*
 * Adafruit Flora Blink Shoe
 * 
 * This program monitors a ball-in-tube sensor to blink LEDs:
 * - Upon a change and then return to the specified ACTIVE_STATE (LOW or HIGH)
 *   the activityLevel is incremented up to maxActivityLevel at most.
 * - The higher the activityLevel the lower the duration between LED blinks.
 * - activityLevel is decremented every activityLevelDelay if a state change is
 *   not detected, causing the LED blink rate to slow over time if no movement
 *   is detected.
 * - Whenever activityLevel is decrmented to 0, a cooldownDuration long timer is
 *   started before entering an inactive state where the LEDs no longer blink.
 * 
 * Connections:
 * - Tilt sensor: 6 and GND
 * - LEDs:        10, 12 and GND
 */

// Pin definitions
const int TILT_PIN = 6;                                // Tilt sensor
const int LED_PIN_10 = 10;                             // LED 1
const int LED_PIN_12 = 12;                             // LEDs 2 & 3

// NOTE: unsigned longs are used for comparison with the values returned by millis()
// which returns an unsigned long

// Tilt sensor state
int currentTiltState = LOW;
int previousTiltState = LOW;
const unsigned long debounceDelay = 50;                // Time state must remain unchanged to register
unsigned long lastDebounceTime = 0;                    // Record time last debounce started

// LED flashing
bool ledFlashing = false;
bool ledState = LOW;
const unsigned long cooldownDuration = 5000;           // Time spent before disabling flashing at activityLevel = 0
unsigned long baseInterval = 500;                      // Base flash interval, slow
unsigned long fastestInterval = 50;                    // Fastest interval, fast
unsigned long currentFlashInterval = baseInterval;     // Flash speed, initially set to base
unsigned long lastFlashTime = 0;                       // Record last time a flash happened
unsigned long activityLevelDelay = 1000;               // Seconds between decrementing activityLevel

// Activity tracking
bool waitingToStop = false;                            // Flag for waiting period after activity reaches zero
int activityLevel = 0;                                 // Current activity level initially set to 0
const int maxActivityLevel = 10;                       // Maximum activity level
const int ACTIVE_STATE = HIGH;                          // Only count this tilt state as "active movement"
unsigned long lastActivityTime = 0;                    // Allows for calculating time since hitting zero

void setup() {
  // Initialize serial out
  Serial.begin(9600);

  // Configure pins
  pinMode(TILT_PIN, INPUT_PULLUP);  // Use internal pullup resistor
  pinMode(LED_PIN_10, OUTPUT);
  pinMode(LED_PIN_12, OUTPUT);

  // Read initial state
  previousTiltState = digitalRead(TILT_PIN);
}

/*
 * Print the current activity level and flash rate
 */
void printActivityLevel() {
  Serial.print("Activity Level: ");
  Serial.print(activityLevel);
  if (ledFlashing) {
    Serial.print(" (");
    Serial.print(1000.0 / currentFlashInterval, 1);
    Serial.println(" Hz)");
  } else {
    Serial.println();
  }
}

/*
 * Handle movement detection and activity level management
 */
void handleMovement() {
  unsigned long currentTime = millis();

  // Only increase activity level on the "active" tilt state
  if (currentTiltState == ACTIVE_STATE) {
    if (ledFlashing) {
      // Already flashing - increase activity level
      if (activityLevel < maxActivityLevel) {
        activityLevel++;
      }
      // Reset the waiting period
      waitingToStop = false;
    } else {
      // Start flashing
      ledFlashing = true;
      lastFlashTime = currentTime;
      ledState = HIGH;
      digitalWrite(LED_PIN_10, ledState);
      digitalWrite(LED_PIN_12, ledState);
      activityLevel++;
      Serial.println("Started");
    }

    updateFlashRate();
    lastActivityTime = currentTime;
  }
}

/*
 * Update the flash rate based on current activity level
 */
void updateFlashRate() {
  if (activityLevel == 0) {
    currentFlashInterval = baseInterval;
  } else {
    // Calculate blink rate linearly
    float minHz = 1000.0 / baseInterval;
    float maxHz = 1000.0 / fastestInterval;
    float hzStep = (maxHz - minHz) / maxActivityLevel;

    float targetHz = minHz + (hzStep * activityLevel);
    currentFlashInterval = (unsigned long)(1000.0 / targetHz);
  }
}

void loop() {
  // Read the current tilt sensor state
  int reading = digitalRead(TILT_PIN);

  // Debounce the tilt sensor reading
  if (reading != previousTiltState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // If the reading has been stable for the debounce delay
    if (reading != currentTiltState) {
      currentTiltState = reading;

      if (currentTiltState == ACTIVE_STATE) {
        handleMovement();
        printActivityLevel();
      }
    }
  }

  // Handle LED flashing
  if (ledFlashing) {
    unsigned long currentTime = millis();

    // Do not stop unless cooldownDuration has been met
    bool shouldStop = false;
    if (waitingToStop && (currentTime - lastActivityTime >= cooldownDuration)) {
      // 5 seconds have passed since activity reached zero
      shouldStop = true;
    }

    if (shouldStop) {
      ledFlashing = false;
      waitingToStop = false;
      digitalWrite(LED_PIN_10, LOW);
      digitalWrite(LED_PIN_12, LOW);
      Serial.println("Stopped");
    } else {
      // Continue flashing at the current interval
      if (currentTime - lastFlashTime >= currentFlashInterval) {
        // Flip LED state
        ledState = !ledState;
        digitalWrite(LED_PIN_10, ledState);
        digitalWrite(LED_PIN_12, ledState);
        // Record last flast time
        lastFlashTime = currentTime;
      }
    }
  }

  // Gradually decrease activity level if movement slows
  if (ledFlashing && (millis() - lastActivityTime > activityLevelDelay)) {
    if (activityLevel > 0) {
      activityLevel--;
      updateFlashRate();
      lastActivityTime = millis();
      printActivityLevel();

      // Check if activity just reached zero
      if (activityLevel == 0 && !waitingToStop) {
        waitingToStop = true;
        Serial.println("Cooling down...");
      }
    }
  }

  // Store the reading for next iteration
  previousTiltState = reading;

  delay(10);
}