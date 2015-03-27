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

int delayval = 10; // delay for half a second

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  pixels.begin(); // This initializes the NeoPixel library.
}

int num_slices = 50; // fifty slices, each 100 ms long
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
  delayval = 80;
  num_slices = 250;
  int step_size = 2; // num_slices / 255;
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

void flash(){
  num_slices = 25;
  delayval = 7;
  for(int i=0;i<num_slices;i++){
    for(int j=0;j<NUMPIXELS;j++){
      pixels.setPixelColor(j,pixels.Color(i*10,i*10,i*10));
    }
    update_strands();
    delay(delayval);
  }

}

void fade(){
  num_slices = 25;
  delayval = 20;
  int maxval = 250;
  int b = 0;
  for(int i=maxval;i>0;i=i-5){
    for(int j=0;j<NUMPIXELS;j++){
      if(i < 70) { b = 70; } else { b = i; }
      pixels.setPixelColor(j,pixels.Color(i,i,b));
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

void loop() {
  delayval = 50; // start out with 10ms timeslices
  clear_strip();
  button();
  delay(2000);
  flash();
  fade();
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
