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

void sparkle(){
  int delayval = 80;
  int num_slices = 250;
  int step_size = 2;
  for(int i=0;i<num_slices;i++){
    for(int j=0;j<NUMPIXELS;j++){
      if((((i % 3) + j) % 3) == 0) {
        pixels.setPixelColor(j,pixels.Color(255-i,255-i,255-i));
      } else {
        pixels.setPixelColor(j,off_color);
      }
    }
    update_strands();
    delay(delayval);
  }
  clear_strip();
  update_strands();
}

void simple_fade(uint32_t duration_ms, uint8_t initial_brightness, uint8_t final_brightness) {
  int num_slices = duration_ms / UPDATE_INTERVAL_MS;
  int total_shift = final_brightness - initial_brightness;
  for(int i = 0; i < num_slices; i++) {
    // pre-calculating parts of this could reduce computation but might generate artifacts since integer math drops remainders
    // specifically, calculating shift-per-slice could be way off for shifts per slice with a large fractional part
    int current_value = total_shift * (i / num_slices) + initial_brightness;
    for(int j = 0; j < NUMPIXELS; j++){
      pixels.setPixelColor(j, pixels.Color(current_value, current_value, current_value));
    }
    update_strands();
    delay(UPDATE_INTERVAL_MS);
  }
}

void white_flash(uint32_t duration_ms, uint8_t initial_brightness, uint8_t final_brightness) {
  // use the final brightness as the blue hold value so blue never holds until the end
  //fade_to_blue(duration_ms, initial_brightness, final_brightness, final_brightness);
  simple_fade(duration_ms, initial_brightness, final_brightness);
}

void fade_to_blue(uint32_t duration_ms, uint8_t initial_brightness, uint8_t final_brightness, uint8_t blue_hold) {
  int num_slices = duration_ms / UPDATE_INTERVAL_MS;
  int total_shift = final_brightness - initial_brightness;
  for(int i = 0; i < num_slices; i++) {
    // pre-calculating parts of this could reduce computation but might generate artifacts since integer math drops remainders
    // specifically, calculating shift-per-slice could be way off for shifts per slice with a large fractional part
    int current_value = total_shift * (i / num_slices) + initial_brightness;
    int b = current_value;
    // if decreasing and below hold value or increasing and above hold value, apply hold value
    if ((total_shift < 0 && current_value < blue_hold) ||
        (total_shift > 0 && current_value > blue_hold)) {
      b = blue_hold;
    }

    for(int j = 0; j < NUMPIXELS; j++){
      pixels.setPixelColor(j, pixels.Color(current_value, current_value, b));
    }
    update_strands();
    delay(UPDATE_INTERVAL_MS);
  }
}

void clear_strip(){
  for(int j = 0; j < NUMPIXELS; j++){
    pixels.setPixelColor(j, off_color);
  }
  update_strands();
}

void loop() {
  clear_strip();
  button();
  delay(2000);
  white_flash(175, 0, 250);
  fade_to_blue(500, 250, 0, 70);
  sparkle();
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
