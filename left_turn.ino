#include <LedControl.h>

// Create a new LedControl instance
// Parameters: DIN pin, CLK pin, CS pin, number of MAX7219 modules
LedControl lc = LedControl(11, 13, 10, 1);

const int buttonPin1 = 2;
const int buttonPin2 = 3;

bool walkRequested = false;
bool walkActive = false;     // Keeps Walk ON until lane changes

unsigned long walkStartTime = 0;
const unsigned long walkDelay = 1500;  // 1000 ms (1 second) delay before Walk starts
bool waitingToActivateWalk = false;


byte handPattern[8] = {
    B00000000, // Row 0
    B00111000, // Row 1
    B11111110, // Row 2
    B11111111, // Row 3
    B11111110, // Row 4
    B11110000, // Row 5
    B00111000, // Row 6
    B00000000  // Row 7
};

byte walk[8] = {
  B00000000, // Row 0
    B10011000, // Row 1
    B01001000, // Row 2
    B0111100, // Row 3
    B00100111, // Row 4
    B11001011, // Row 5
    B00001000, // Row 6
    B00000100  // Row 7

};

void displayWalk() {
  for (int row = 0; row < 8; row++) {
         lc.setRow(0, row, walk[row]); // Turn on the LED at row, col
        }
}

void displayHand() {
  for (int row = 0; row < 8; row++) {
        lc.setRow(0, row, handPattern[row]); // Turn on the LED at row, col
        }

}
void setup() {
    // Initialize the MAX7219 module
    lc.shutdown(0, false);       // Wake up the MAX7219
    lc.setIntensity(0, 0);       // Set brightness level (0 is min, 15 is max)
    lc.clearDisplay(0);          // Clear the display

    pinMode(buttonPin1,INPUT_PULLUP);
    pinMode(buttonPin2,INPUT_PULLUP);
    pinMode(4,INPUT);
    pinMode(5,INPUT);



    Serial.begin(9600);
}

void loop() {
//lane == light with the left turn lane
//!lane == light that can't go straight :)


      // Turn on all the LEDs
int lane = digitalRead(4);
int left = digitalRead(5);
int buttonState = digitalRead(buttonPin1);

// If button is pressed during !lane, store the request
if (lane == 0 && buttonState == HIGH) {  // LOW = pressed
  walkRequested = true;
  Serial.println("Button pressed during !lane, Walk request stored!");
}

// Step 1: Wait until lane becomes active, and only then consider walk activation
if (lane == 1 && walkRequested && !waitingToActivateWalk && !walkActive) {
  if (left == LOW) {
    // Left is not active, safe to begin walk delay
    walkStartTime = millis();
    waitingToActivateWalk = true;
    Serial.println("Left turn ended — Starting walk delay...");
  } else {
    Serial.println("Waiting for left turn to end...");
  }
}

// Step 2: After walk delay, activate Walk ONLY IF left turn is still inactive
if (waitingToActivateWalk && (millis() - walkStartTime >= walkDelay)) {
  if (left == LOW) {
    walkActive = true;
    walkRequested = false;
    waitingToActivateWalk = false;
    Serial.println("Walk signal activated (after delay)!");
  } else {
    // Left turn came back on after the delay started → wait again
    waitingToActivateWalk = false;  // Cancel delay — restart check
    Serial.println("Left turn came on during delay. Cancelling walk activation and waiting again...");
  }
}

// Display logic
if (walkActive) {
  displayWalk();
} else {
  displayHand();
}

// Reset Walk if lane ends
if (lane == 0 && walkActive) {
  walkActive = false;
  Serial.println("Walk signal turned off.");
}

delay(100);

}
