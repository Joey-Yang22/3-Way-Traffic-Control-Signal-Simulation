#include <LedControl.h>

// Create a new LedControl instance
// Parameters: DIN pin, CLK pin, CS pin, number of MAX7219 modules
LedControl lc = LedControl(11, 13, 10, 1);

const int buttonPin1 = 2;
const int buttonPin2 = 3;

bool walkRequested = false;
bool walkActive = false;     // Keeps Walk ON until lane changes

unsigned long walkStartTime = 0;
const unsigned long walkDelay = 1500;  // Delay in milliseconds before Walk shows
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
 int buttonState = digitalRead(buttonPin1);  // LOW = pressed

 //Store walk request if button pressed during lane phase
  if (lane == 1 && buttonState == HIGH) {
    walkRequested = true;
    Serial.println("Crosswalk B: Button pressed during lane. Walk request stored.");
  }

  //When lane becomes inactive, start walk delay if walk was requested
  if (lane == 0 && walkRequested && !waitingToActivateWalk && !walkActive) {
    walkStartTime = millis();             // Start delay timer
    waitingToActivateWalk = true;
    walkRequested = false;                // Clear walk request after we begin waiting
    Serial.println("Crosswalk B: Starting delay before Walk signal...");
  }

  //Activate walk after delay period passes
  if (waitingToActivateWalk && (millis() - walkStartTime >= walkDelay)) {
    walkActive = true;
    waitingToActivateWalk = false;
    Serial.println("Crosswalk B: Walk signal activated after delay.");
  }

  //Display logic
  if (walkActive) {
    displayWalk();
  } else {
    displayHand(); // Always show this instantly if Walk isn't active
  }

  // Stop Walk signal when lane becomes active again
  if (lane == 1 && walkActive) {
    walkActive = false;
    Serial.println("Crosswalk B: Walk signal turned off.");
  }

  delay(100); // Minor loop delay

}
