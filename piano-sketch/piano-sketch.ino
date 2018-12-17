/**
 * ORIGINAL
 * ------------------------------------------------------------------
 * piano-sketch.ino
 * Author: Bonnie Eisenman
 * twitter.com/brindelle
 *
 * Arduino-side code for taking in inputs from several sensors,
 * and if they pass the thresholds, send information to
 * Serial for processing.
 *
 * This is meant to run on a Teensy, with 8 analog inputs.
 * Make sure to adjust analogPins[] and numAnalogPins below
 * if you want to run this on e.g. an Arduino Uno,
 * which has only 6 analog inputs.
 * 
 * MODIFIED VERSION
 * ------------------------------------------------------------------
 * 
 * Using up to 26 ultrasonics sensor with up to 26 LED
 **/

#include <Adafruit_NeoPixel.h>

#define SENSOR_LENGTH 26
#define LED_STRIP_NUM 120

uint8_t sensorPins[] = {
  26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
  36, 37, 38, 39, 40, 41, 42, 43, 44, 45,
  46, 47, 48, 49, 50, 51
};
 
uint8_t ledPins[] = {
  51, 52,  2,  3,  4,  5,  6,  7,  8,  9,
  10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
  20, 21, 22, 23, 24, 25
};

Adafruit_NeoPixel pixels[SENSOR_LENGTH];

long thresholds[SENSOR_LENGTH];

// This value can be tuned depending on sensor installation.
// It will likely vary based on: ambient light conditions;
// positioning of the sensors; resistor values used; etc.
long absoluteThresh = 20;

// Keep track of the last N x H sensor readings,
// where N = number of inputs
// and H = history length.
// This allows us to do smoothing.
long sensorHistory[SENSOR_LENGTH][10];

// This is used to cycle through the sensorHistory array
// along the H dimension.
uint8_t bufferIndex = 0;

void setup() {
  Serial.begin(115200);

  // IO initializaions
  for (uint8_t i = 0; i < SENSOR_LENGTH; i++) {
    pinMode(sensorPins[i], OUTPUT);

    pixels[i] = Adafruit_NeoPixel(LED_STRIP_NUM, ledPins[i], NEO_GRB + NEO_KHZ400);
    pixels[i].begin(); // This initializes the NeoPixel library.
  }

  // Test LED
  for (uint8_t i = 0; i < SENSOR_LENGTH; i++) {
    ledOn(i);
  }
  
  // calibrate();
}

void loop() {
  

  /*
  for (int i = 0; i < SENSOR_LENGTH; i++) {
    long val = sensorRead(sensorPins[i]);
    long oldAvg = 0;
    
    for (uint8_t j = 0; j < 10; j++) {
      oldAvg += sensorHistory[i][j];
    }
    oldAvg = oldAvg / 10;
    
    if (val > oldAvg + absoluteThresh) {
      Serial.print("1");
      digitalWrite(ledPins[i], HIGH);
    } else {
      Serial.print("0");
      digitalWrite(ledPins[i], LOW);
    }
    
    sensorHistory[i][bufferIndex] = val;
  }
  
  bufferIndex = (bufferIndex + 1) % 10;
  Serial.println();
  delay(5);
  */
}

void ledOn(uint8_t pixel) {
  for (uint8_t i = 0; i < LED_STRIP_NUM; i++) {
    pixels[pixel].setPixelColor(i, pixels[pixel].Color(0,150,0)); // Moderately bright green color.
    pixels[pixel].show(); // This sends the updated pixel color to the hardware.
  }
}

void ledOff(uint8_t pixel) {
  for (uint8_t i = 0; i < LED_STRIP_NUM; i++) {
    pixels[pixel].setPixelColor(i, pixels[pixel].Color(0,0,0)); // Moderately bright green color.
    pixels[pixel].show(); // This sends the updated pixel color to the hardware.
  }
}

void calibrate() {
  // How many steps' worth of data we use for calibration purposes
  int maxSteps = 10;

  // Begin by accumulating several steps' worth of baseline data
  for (int curStep = 0; curStep < maxSteps; curStep++) {
    for (int pin = 0; pin < SENSOR_LENGTH; pin++) {
       thresholds[pin] += sensorRead(sensorPins[pin]);
    }
  }

  // Average out the baseline readings based on how many input readings we did
  for (int pin = 0; pin < SENSOR_LENGTH; pin++) {
    thresholds[pin] = thresholds[pin] / maxSteps;
  }
  
  // Fill the entire sensorHistory array with baseline data
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 10; j++) {
       sensorHistory[i][j] = thresholds[i];
    }
  } 
}

long sensorRead(uint8_t pin) {
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delayMicroseconds(2);
  digitalWrite(pin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pin, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH pulse
  // whose duration is the time (in microseconds) from the sending of the ping
  // to the reception of its echo off of an object.
  pinMode(pin, INPUT);
  return pulseIn(pin, HIGH);  
}
