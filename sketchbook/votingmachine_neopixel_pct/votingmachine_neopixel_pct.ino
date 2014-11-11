/*

  votingbooth_2strip_pushbuttons


 Second stab at demo app for crowd acclaim voting booth

 When         Who  What
  8-Sep-2014  erd  Initial version from HL1606strip demo (requires HL1602 on pins 2, 3, 4)
  9-Sep-2014  erd  Second version using dual NeoPixel strips (Din on 5 and 6)
 11-Sep-2014  erd  Modified for single logical strip of 180 pixels (3x60) on pin 5
 23-Oct-2014  erd  Mods for GCAC Open House
  3-Nov-2014  erd  Mods for percentage operation

*/
#include <Adafruit_NeoPixel.h>

#define DEBOUNCE 200

#define PIN 6
#define LEN 8

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEN*3, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

#define INTENSITY 32
#define SPEED 100

#define BARMAX 8

#define MIDDLE_REVERSED 1
#define MIDDLE_SHORT 1
#define INVERT_TEST 1

// address of lowest pixel on each bar
#define LBAR 0
#define MBAR ((LEN + (MIDDLE_REVERSED * (LEN - 1) - (MIDDLE_REVERSED * MIDDLE_SHORT))))
#define RBAR ((2 * LEN) - MIDDLE_SHORT)

// define vote-display parameter
#define L_button 9
#define M_button 8
#define R_button 7

#define LEFT   (LEN * 0)
#define MIDDLE (LEN * 1)
#define RIGHT  (LEN * 2)

uint32_t L_color = 0;
uint32_t M_color = 0;
uint32_t R_color = 0;

int L_votes = 0;
int M_votes = 0;
int R_votes = 0;

int L_drop = 0;
int M_drop = 0;
int R_drop = 0;

void setup() {
  uint32_t i;

  // init input buttons
  pinMode(L_button, INPUT);
  digitalWrite(L_button, HIGH); // turn on pullup
  pinMode(M_button, INPUT);
  digitalWrite(M_button, HIGH); // turn on pullup
  pinMode(R_button, INPUT);
  digitalWrite(R_button, HIGH); // turn on pullup

  // init strips
  for(i=0; i<strip.numPixels(); i++)
       strip.setPixelColor(i, 0);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  // Set colors
  L_color = strip.Color(INTENSITY, 0, 0);
  M_color = strip.Color(0, INTENSITY, 0);
  R_color = strip.Color(0, 0, INTENSITY);

  Serial.begin(9600);
  delay(200);
  Serial.println("setup()");

}

void loop() {
  uint8_t i;
  uint32_t color;

  int total_votes = 0;

  strip.show();

  // check inputs and change state
  L_drop = INVERT_TEST - digitalRead(L_button);
  M_drop = INVERT_TEST - digitalRead(M_button);
  R_drop = INVERT_TEST - digitalRead(R_button);

  if (L_drop) {
      L_votes++;
      //total_votes = L_votes+M_votes+R_votes;
      //anim(LBAR, L_color, LOW, L_votes, total_votes);
  }
  if (M_drop) {
      M_votes++;
      //total_votes = L_votes+M_votes+R_votes;
      //anim(MBAR, M_color, HIGH, M_votes, total_votes);
  }
  if (R_drop) {
      R_votes++;
      //total_votes = L_votes+M_votes+R_votes;
      //anim(RBAR, R_color, LOW, R_votes, total_votes);
  }
  setbar(L_votes, M_votes, R_votes);
  delay(200);  // software debounce
}

void setbar(int lv, int mv, int rv)
{
  uint8_t i;
  uint32_t color;
  int vtotal = lv+mv+rv;
  int cutoff;
  int remainder;
  float ratio;

  Serial.print("L: ");
  Serial.print(lv);
  Serial.print("[");
  if (vtotal)
    Serial.print((lv * BARMAX) / vtotal);
  else
    Serial.print(0);
  Serial.print("/");
  Serial.print(BARMAX);
  Serial.print("]");
  Serial.print(" M: ");
  Serial.print(mv);
  Serial.print("[");
  if (vtotal)
    Serial.print((mv * BARMAX) / vtotal);
  else
    Serial.print(0);
  Serial.print("/");
  Serial.print(BARMAX);
  Serial.print("]");
  Serial.print(" R: ");
  Serial.print(rv);
  Serial.print("[");
  if (vtotal)
    Serial.print((rv * BARMAX) / vtotal);
  else
    Serial.print(0);
  Serial.print("/");
  Serial.print(BARMAX);
  Serial.print("]");
  Serial.print(" T: ");
  Serial.println(lv+mv+rv);

  // calculate left strip numbers
  if (vtotal)
    ratio = float(lv * BARMAX) / float(vtotal);
  else
    ratio = 0.0;
  cutoff = int(ratio);
  remainder = 100*float(ratio - cutoff);
  Serial.print(" L ");
  Serial.print(ratio);
  Serial.print(" ");
  Serial.print(cutoff);
  Serial.print(" ");
  Serial.println(remainder);

  // draw main bar
  for(i=0; i < BARMAX; i++) {
    color = (i < cutoff ? L_color : 0);
    strip.setPixelColor(i+LBAR, color);
  }
  // draw half-lit bar topper
  if ((cutoff < BARMAX) && (remainder > 0)) {
    strip.setPixelColor(LBAR+cutoff, L_color / 4);
  }

  // calculate middle strip numbers
  if (vtotal)
    ratio = float(mv * BARMAX) / float(vtotal);
  else
    ratio = 0.0;
  cutoff = int(ratio);
  remainder = 100*float(ratio - cutoff);
  Serial.print(" M ");
  Serial.print(ratio);
  Serial.print(" ");
  Serial.print(cutoff);
  Serial.print(" ");
  Serial.println(remainder);

  // draw main bar
  for(i=0; i < BARMAX - MIDDLE_SHORT; i++) {
    color = (i < cutoff ? R_color : 0);
    strip.setPixelColor(i+RBAR, color);
  }
  // draw half-lit bar topper
  if ((cutoff < BARMAX) && (remainder > 0)) {
    strip.setPixelColor(RBAR+cutoff, R_color / 4);
  }

  strip.show();
}
void anim(int start, uint32_t color, int invert, int votes, int total)
{
  uint8_t i;

  for (i=0; i < BARMAX; i++) {
    if (i < (BARMAX-1))
      strip.setPixelColor(start+i, 0);  // black out one pixel above our blip
    strip.setPixelColor(start+i, color);  // light up our blip
    strip.show();
    delay(SPEED);
  }
  if (total < BARMAX) {
    for (i=BARMAX-1; i > votes/total; --i) {
        strip.setPixelColor(start+i, 0);
        strip.show();
        delay(SPEED);
    }
  }
}
