// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      27

int8_t pins[] = {6,9,10,12,-1};

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define UPDATE_INTERVAL_MS 10

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  pixels.begin(); // This initializes the NeoPixel library.
}

uint32_t sparkle_color = pixels.Color(250,250,250);
uint32_t off_color = pixels.Color(0,0,0);
long lastDebounceTime = 0;
long debounceDelay = 1000;

void button(){
  while(1){
    int reading = digitalRead(3);
    if(reading != HIGH) {
      lastDebounceTime = millis();
    }
    if ((millis() - lastDebounceTime) > debounceDelay) {
      break;
    }
  }
  while(1){
    int reading = digitalRead(3);
    if(reading != HIGH){
      break;
    }
  }
}

void sparkle(int duration_ms, uint32_t initial_color, uint32_t final_color) {
  uint8_t initial_red = getRed(initial_color);
  uint8_t initial_green = getGreen(initial_color);
  uint8_t initial_blue = getBlue(initial_color);

  uint8_t final_red = getRed(final_color);
  uint8_t final_green = getGreen(final_color);
  uint8_t final_blue = getBlue(final_color);

  int16_t shift_red = (int16_t)final_red - initial_red;
  int16_t shift_green = (int16_t)final_green - initial_green;
  int16_t shift_blue = (int16_t)final_blue - initial_blue;

  // doesn't use the standard interval, controls how fast the neopixels sparkle instead
  int delayval = 80;
  int num_slices = duration_ms / delayval;
  for(int i = 0; i < num_slices; i++){
    // pre-calculating parts of this could reduce computation but might generate artifacts since integer math drops remainders
    // specifically, calculating shift-per-slice could be way off for shifts per slice with a large fractional part
    int red = shift_red * (i / num_slices) + initial_red;
    int green = shift_green * (i / num_slices) + initial_green;
    int blue = shift_blue * (i / num_slices) + initial_blue;
    for(int j = 0; j < NUMPIXELS; j++){
      if((((i % 3) + j) % 3) == 0) {
        pixels.setPixelColor(j, pixels.Color(red, green, blue));
      } else {
        pixels.setPixelColor(j, off_color);
      }
    }
    update_strands();
    delay(delayval);
  }
}

void clear_strip(){
  for(int j = 0; j < NUMPIXELS; j++){
    pixels.setPixelColor(j, off_color);
  }
  update_strands();
}

uint8_t getRed(uint32_t color) {
  return (color & 0xff0000) >> 16;
}

uint8_t getGreen(uint32_t color) {
  return (color & 0xff00) >> 8;
}

uint8_t getBlue(uint32_t color) {
  return color & 0xff;
}

void fade(int duration_ms, uint32_t initial_color, uint32_t final_color) {
  uint8_t initial_red = getRed(initial_color);
  uint8_t initial_green = getGreen(initial_color);
  uint8_t initial_blue = getBlue(initial_color);

  uint8_t final_red = getRed(final_color);
  uint8_t final_green = getGreen(final_color);
  uint8_t final_blue = getBlue(final_color);

  int16_t shift_red = (int16_t)final_red - initial_red;
  int16_t shift_green = (int16_t)final_green - initial_green;
  int16_t shift_blue = (int16_t)final_blue - initial_blue;

  int num_slices = duration_ms / UPDATE_INTERVAL_MS;
  for(int i = 0; i < num_slices; i++) {
    // pre-calculating parts of this could reduce computation but might generate artifacts since integer math drops remainders
    // specifically, calculating shift-per-slice could be way off for shifts per slice with a large fractional part
    int red = shift_red * (i / num_slices) + initial_red;
    int green = shift_green * (i / num_slices) + initial_green;
    int blue = shift_blue * (i / num_slices) + initial_blue;
    for(int j = 0; j < NUMPIXELS; j++){
      pixels.setPixelColor(j, pixels.Color(red, green, blue));
    }
    update_strands();
    delay(UPDATE_INTERVAL_MS);
  }
}

void do_flash() {
  fade(175, pixels.Color(0, 0, 0), pixels.Color(250, 250, 250));
}

void do_fade() {
  // Instead of having a "hold value" integrated into the function, just break this into two separate fades
  uint32_t hold_color = pixels.Color(70, 70, 70);
  fade(350, pixels.Color(250, 250, 250), hold_color);
  fade(150, hold_color, pixels.Color(0, 0, 70));
}

void do_sparkle() {
  sparkle(20000, pixels.Color(255, 255, 255), pixels.Color(5, 5, 5));
}

void loop() {
  clear_strip();
  button();
  delay(2000);
  do_flash();
  do_fade();
  do_sparkle();
  clear_strip();
}

void update_strands(){
  // copy to other strips here
  int i = 0;
  while(pins[i]!=-1){
    pixels.setPin(pins[i]);
    pixels.show();
    i++;
  }
}
