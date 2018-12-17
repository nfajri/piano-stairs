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

#define SENSOR_LENGTH       26
#define SENSOR_TIMEOUT      20000 // microseconds
#define SENSOR_HISTORY_NUM  10
#define LED_STRIP_NUM       120

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

Adafruit_NeoPixel leds[SENSOR_LENGTH];
long thresholds[SENSOR_LENGTH];
long sensorHistory[SENSOR_LENGTH][SENSOR_HISTORY_NUM];
uint8_t historyIndex;

void setup() {
  Serial.begin(115200);

  // IO initializaions
  for (uint8_t i = 0; i < SENSOR_LENGTH; i++) {
    pinMode(sensorPins[i], OUTPUT);
    
    leds[i] = Adafruit_NeoPixel(LED_STRIP_NUM, ledPins[i], NEO_GRB + NEO_KHZ400);
    leds[i].begin(); // This initializes the NeoPixel library.
  }

  calibrate();
}

void loop() {  
  // testLed();
  testSensor();
}

void testLed() {
  for (uint8_t i = 0; i < SENSOR_LENGTH; i++) {
    ledOn(i);
  }
  delay(1000);
  
  for (uint8_t i = 0; i < SENSOR_LENGTH; i++) {
    ledOff(i);
  }
  delay(1000);
}

void testSensor() {
  for (int i = 0; i < SENSOR_LENGTH; i++) {
    long val = sensorRead(sensorPins[i]);

    long oldAvg = 0;
    
    for (uint8_t j = 0; j < SENSOR_HISTORY_NUM; j++) {
      oldAvg += sensorHistory[i][j];
    }
    
    oldAvg = oldAvg / SENSOR_HISTORY_NUM;

    if (val > 0 && val < thresholds[i]) {
      Serial.print("1");
      ledOn(i);
    } else {
      Serial.print("0");
      ledOff(i);
    }
//    Serial.print(oldAvg);
//    Serial.print(", "); 

    sensorHistory[i][historyIndex] = val;
  }
  Serial.println();

  historyIndex = (historyIndex + 1) % SENSOR_HISTORY_NUM;
  // delay(5);
}

void calibrate() {
  // How many steps' worth of data we use for calibration purposes
  uint8_t maxSteps = 30;

  // Begin by accumulating several steps' worth of baseline data
  for (uint8_t curStep = 0; curStep < maxSteps; curStep++) {
    for (uint8_t pin = 0; pin < SENSOR_LENGTH; pin++) {
       thresholds[pin] += sensorRead(sensorPins[pin]);
    }
  }

  // Average out the baseline readings based on how many input readings we did
  for (uint8_t pin = 0; pin < SENSOR_LENGTH; pin++) {
    thresholds[pin] = thresholds[pin] / maxSteps;
  }
  
  // Fill the entire sensorHistory array with baseline data
  for (uint8_t i = 0; i < SENSOR_LENGTH; i++) {
    for (uint8_t j = 0; j < SENSOR_HISTORY_NUM; j++) {
       sensorHistory[i][j] = thresholds[i];
    }
  }

  Serial.println("Tresholds:");
  for (uint8_t i = 0; i < SENSOR_LENGTH; i++) {
    thresholds[i] = thresholds[i] - (thresholds[i] / 5);
    Serial.print(thresholds[i]);
    Serial.print(", ");
  }
  Serial.println();
}

void ledOn(uint8_t led) {
  for (uint8_t i = 0; i < LED_STRIP_NUM; i++) {
    leds[led].setPixelColor(i, leds[led].Color(200,200,200)); 
  }
  leds[led].show(); // This sends the updated pixel color to the hardware.
}

void ledOff(uint8_t led) {
  for (uint8_t i = 0; i < LED_STRIP_NUM; i++) {
    leds[led].setPixelColor(i, leds[led].Color(0,0,0));
  }
  leds[led].show(); // This sends the updated pixel color to the hardware.
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
  return pulseIn(pin, HIGH, SENSOR_TIMEOUT);  
}
