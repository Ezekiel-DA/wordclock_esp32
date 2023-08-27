#pragma once

#include <FastLED.h>

#include "utils.h"

#define BDAY_HUE_STRIDE 5
#define BDAY_HUE_STEP 1

static uint16_t rainbowLoopGlobalSpeed = 100;
static uint8_t rainbowLoopHue = 0;

void rainbowLoop(void*) {
  while (1) {
    clearClockFace();
    for (uint8_t y = 0; y < MATRIX_HEIGHT; ++y) {
      for (uint8_t x = 0; x < MATRIX_WIDTH; ++x) {
        _leds[XY(x, y)] = CHSV(rainbowLoopHue, 255, 255);
        FastLED.show();

        auto notified = ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1));
        if (notified) {
          setAllLEDs(CRGB::Black, _leds, NUM_LEDS);
          FastLED.show();
          vTaskDelete(NULL);
        }
        
        delay(rainbowLoopGlobalSpeed);
        rainbowLoopHue += 5;
      }
      //rainbowLoopHue += 100;
    }
  }
}

void bdayDoEffectLoop(void*) {
  clearClockFace();

  // initialize a rainbow
  uint8_t hues[16] = {0};
  uint8_t bdayHue = 0;
  for (uint8_t i = 0; i < 16; ++i) {
    hues[i] = bdayHue += BDAY_HUE_STRIDE;
  }
  
  while(1) {
    EVERY_N_MILLIS(32) {
      uint8_t bdayTextIndex = 0;
      for (uint8_t i = 39; i <= 43; ++i) { // HAPPY
        _leds[i] = CHSV(hues[bdayTextIndex++] -= BDAY_HUE_STEP, 255, 255);
      }
      for (uint8_t i = 52; i <= 59; ++i) { // BIRTHDAY
        _leds[i] = CHSV(hues[bdayTextIndex++] -= BDAY_HUE_STEP, 255, 255);
      }
      for (uint8_t i = 82; i <= 84; ++i) { // DOM
        _leds[i] = CHSV(hues[bdayTextIndex++] -= BDAY_HUE_STEP, 255, 255);
      }
  
      FastLED.show();

      auto notified = ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1));
      if (notified) {
        setAllLEDs(CRGB::Black, _leds, NUM_LEDS);
        FastLED.show();
        vTaskDelete(NULL);
      }
    }  
  }
}


void bdayClaireEffectLoop(void*) {
  static uint8_t bdayHue = 0;

  clearClockFace();

  while(1) {
    EVERY_N_MILLIS(32) {
      uint8_t bdayTextIndex = 0;
      for (uint8_t i = 39; i <= 43; ++i) { // HAPPY
        _leds[i] = CHSV(bdayHue + (BDAY_HUE_STEP*bdayTextIndex++), 255, 255);
      }
      for (uint8_t i = 52; i <= 59; ++i) { // BIRTHDAY
        _leds[i] = CHSV(bdayHue + (BDAY_HUE_STEP*bdayTextIndex++), 255, 255);
      }
      for (uint8_t i = 107; i <= 112; ++i) { // CLAIRE
        _leds[i] = CHSV(bdayHue + (BDAY_HUE_STEP*bdayTextIndex++), 255, 255);
      }
  
      //FastLED.show();
      bdayHue += 1;

      auto notified = ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1));
      if (notified) {
        setAllLEDs(CRGB::Black, _leds, NUM_LEDS);
        FastLED.show();
        vTaskDelete(NULL);
      }
    }  
  }
}

void matrixEffectLoop(void*) {
  clearClockFace();
  
  while (1) {
    EVERY_N_MILLIS(75) // falling speed
    {
      // move code downward
      // start with lowest row to allow proper overlapping on each column
      for (int8_t row=MATRIX_HEIGHT-1; row>=0; row--)
      {
        for (int8_t col=0; col<MATRIX_WIDTH; col++)
        {
          if (_leds[XY(col, row)] == CRGB(175,255,175))
          {
            _leds[XY(col, row)] = CRGB(27,130,39); // create trail
            if (row < MATRIX_HEIGHT-1) _leds[XY(col, row+1)] = CRGB(175,255,175);
          }
        }
      }
  
      // fade all _leds
      for(int i = 0; i < NUM_LEDS; i++) {
        if (_leds[i].g != 255) _leds[i].nscale8(192); // only fade trail
      }
  
      // check for empty screen to ensure code spawn
      bool emptyScreen = true;
      for(int i = 0; i < NUM_LEDS; i++) {
        if (_leds[i])
        {
          emptyScreen = false;
          break;
        }
      }
  
      // spawn new falling code
      if (random8(3) == 0 || emptyScreen) // lower number == more frequent spawns
      {
        int8_t spawnX = random8(MATRIX_WIDTH);
        _leds[XY(spawnX, 0)] = CRGB(175,255,175 );
      }
  
      FastLED.show();

      auto notified = ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1));
      if (notified) {
        setAllLEDs(CRGB::Black, _leds, NUM_LEDS);
        FastLED.show();
        vTaskDelete(NULL);
      }
    }
  }
}
