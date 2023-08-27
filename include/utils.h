#pragma once

#define WORD_LEDS_PIN 12
#define TOP_LEFT_LED_PIN 14
#define TOP_RIGHT_LED_PIN 27
#define BOTTOM_LEFT_LED_PIN 26
#define BOTTOM_RIGHT_LED_PIN 25
#define AMB_LIGHT_PIN 33

#define MATRIX_WIDTH 13
#define MATRIX_HEIGHT 10
#define NUM_LEDS MATRIX_WIDTH*MATRIX_HEIGHT

CRGB _leds[NUM_LEDS]; // probably move this somewhere less nonsensical...

void setAllLEDs(CRGB c, CRGB* strip, uint16_t numLeds) {
    for (uint16_t i = 0; i < numLeds; ++i) {
        strip[i] = c;
    }
}

void setAllLEDs(CHSV c, CRGB* strip, uint16_t numLeds) {
    for (uint16_t i = 0; i < numLeds; ++i) {
        strip[i] = c;
    }
}

/**
 * iHardClear: clear all, including bday text. You want this on startup etc, you don't most of the time when bday effects might be running
 */
void clearClockFace(bool iHardClear=true) {
  if (iHardClear)
    setAllLEDs(CRGB::Black, _leds, NUM_LEDS);

  else {
    for (uint8_t i = 0; i <= 38; ++i) {
      _leds[i] = CRGB::Black;
    }
    for (uint8_t i = 44; i <= 51; ++i) {
      _leds[i] = CRGB::Black;
    }
    for (uint8_t i = 60; i <= 81; ++i) {
      _leds[i] = CRGB::Black;
    }
    for (uint8_t i = 85; i <= 106; ++i) {
      _leds[i] = CRGB::Black;
    }
    for (uint8_t i = 113; i < NUM_LEDS; ++i) {
      _leds[i] = CRGB::Black;
    }
  }
  
  digitalWrite(TOP_LEFT_LED_PIN, LOW);
  digitalWrite(TOP_RIGHT_LED_PIN, LOW);
  digitalWrite(BOTTOM_LEFT_LED_PIN, LOW);
  digitalWrite(BOTTOM_RIGHT_LED_PIN, LOW);
}

uint16_t XY(uint8_t x, uint8_t y) {
  return x+(y*MATRIX_WIDTH);
}
