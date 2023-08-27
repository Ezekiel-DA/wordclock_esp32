#include <Arduino.h>
#include <WiFi.h>
#include <esp32fota.h>
#include <WiFiProv.h>
#include <ezTime.h>
#include <FastLED.h>
#include <Effortless_SPIFFS.h>
#include <ZzzMovingAvg.h>
#include <AceButton.h>
using namespace ace_button;
#include <nvs_flash.h>

#include "utils.h"
#include "clockface.h"
#include "fullscreenEffects.h"
#include "httpServices.h"
#include "wifiProvisioning.h"

#define FIRMWARE_VERSION 8

#define BDAY_DO_DAY 1
#define BDAY_DO_MONTH 6
#define BDAY_CLAIRE_DAY 24
#define BDAY_CLAIRE_MONTH 7

#define AMB_LOW_THRESHOLD 250
#define AMB_HIGH_THRESHOLD 700

#define BUTTON_PIN 0

#define RESET_BUTTON_TIME 5

ZzzMovingAvg <16> ambLightMovingAvg;
esp32FOTA otaHandler("wordclock2-base-firmware", FIRMWARE_VERSION);
Timezone tz;
AceButton button((uint8_t) BUTTON_PIN);

TaskHandle_t startupEffectTaskHandle = 0;
TaskHandle_t FOTAIndicatorTaskHandle = 0;
TaskHandle_t matrixEffectTaskHandle = 0;
TaskHandle_t bdayEffectTaskHandle = 0;

bool resetButton = false;

void FOTAIndicatorAnimation(void*) {
  while(1) {
    minute1();
    minute2();
    minute3();
    minute4();
    delay(500);
    minute1(false);
    minute2(false);
    minute3(false);
    minute4(false);
    delay(500);
  }
}

void handleMainButton(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  static long pressedAt = 0;

  switch(eventType) {
    case AceButton::kEventPressed:
      pressedAt = millis();
      break;
    case AceButton::kEventReleased:
      auto elapsed = millis() - pressedAt;

      if (elapsed > (1000 * RESET_BUTTON_TIME)) {
        resetButton = true;
      }
      break;
  }
}

void resetProvisioningAndReboot() {
  Serial.println("Resetting provisioning and rebooting...");
  wifi_prov_mgr_reset_provisioning();
  delay(500);
  nvs_flash_erase();
  delay(500);
  esp_restart();
}

void setup() {
    Serial.begin(115200);
    Serial.println("Word Clock II - starting up...");
    Serial.print("Running firmware version "); Serial.println(FIRMWARE_VERSION);

    WiFi.onEvent(SysProvEvent);
    WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, "bisous", "PROV_LEDCLOCK");

    pinMode(TOP_LEFT_LED_PIN, OUTPUT);
    pinMode(TOP_RIGHT_LED_PIN, OUTPUT);
    pinMode(BOTTOM_LEFT_LED_PIN, OUTPUT);
    pinMode(BOTTOM_RIGHT_LED_PIN, OUTPUT);
    pinMode(AMB_LIGHT_PIN, INPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    
    button.setEventHandler(handleMainButton);
    
    FastLED.addLeds<WS2812B, WORD_LEDS_PIN, GRB>(_leds, 130); // don't add FastLED's "typical LED" correction; it makes the whites pink-ish
    FastLED.setBrightness(75);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 2400);
    clearClockFace();
    FastLED.show();

    otaHandler.checkURL = "http://wordclock2-doclaire.s3-website.us-east-2.amazonaws.com/deployment.json";

    xTaskCreate(rainbowLoop, "startupEffect", 10000, NULL, 1, &startupEffectTaskHandle);

    rainbowLoopGlobalSpeed = 300;

    while (WiFi.status() != WL_CONNECTED) {
      events();
      button.check();
      delay(50);
      if (resetButton) {
        resetProvisioningAndReboot();
      }
    }

    rainbowLoopGlobalSpeed = 100;

    while (timeStatus() != timeSet) {
      events();
      button.check();
      delay(50);
    }
    
    xTaskNotifyGive(startupEffectTaskHandle); // poke the task to delete itself at the end of the next animation step, instead of forcibly doing it possibly while FastLED is busy, which seems to blow everything up
    delay(100); // give the task some time to clear the display before we start trying to show the clock again

    clearClockFace();
    FastLED.show();
    
    Serial.println("UTC: " + UTC.dateTime());

    String timezone = getTimezoneFromIP();
    Serial.print("Auto-detecting timezone: "); Serial.println(timezone);
//    String timezone = "America/New_York";
    tz.setLocation(timezone);
}

void loop() {
  uint16_t now = millis();
  
  events(); // for EZTime's NTP sync etc
  button.check();

  if (resetButton) {
    resetProvisioningAndReboot();
  }

  static uint16_t prevAmbLightRead = millis();
  static uint8_t brightness = 128;
  if ((uint16_t)(now - prevAmbLightRead) >= (1*1000)) {
    int rawAmb = analogRead(AMB_LIGHT_PIN);
    Serial.print("Raw: "); Serial.println(rawAmb);
    int amb = ambLightMovingAvg.add(rawAmb);
        
    if (amb < AMB_LOW_THRESHOLD)
      brightness = 50;
    else if (amb > AMB_HIGH_THRESHOLD)
      brightness = 255;
    else
      brightness = ((amb - AMB_LOW_THRESHOLD)/(AMB_HIGH_THRESHOLD-AMB_LOW_THRESHOLD))*250;
      
    Serial.print("Amb: "); Serial.println(amb);
    Serial.print("Brightness: "); Serial.println(brightness);
    //FastLED.setBrightness(brightness);
    prevAmbLightRead = now;
  }
    
  uint8_t hour = tz.hourFormat12();
  uint8_t minute = tz.minute();
  //Serial.println("Local time: " + tz.dateTime());

  static uint16_t prevFOTACheck = millis();
  
  if ((uint16_t)(now - prevFOTACheck) >= (60*1000))
  {
    bool updatedNeeded = otaHandler.execHTTPcheck();
    if (updatedNeeded)
    {
      clearClockFace();
      FastLED.show();
      xTaskCreate(FOTAIndicatorAnimation, "FOTAIndicatorAnimation", 10000, NULL, 1, &FOTAIndicatorTaskHandle);
      otaHandler.execOTA();
      vTaskDelete(FOTAIndicatorTaskHandle); // we could probably notify the task like we're doing for the other ones, but this one is simple enough that we can just nuke it
    }
     
    prevFOTACheck = now;
  }

  if (minute == 30) {
    xTaskCreate(matrixEffectLoop, "FollowTheWhiteRabbit", 10000, NULL, 1, &matrixEffectTaskHandle);
    delay(60*1000);
    xTaskNotifyGive(matrixEffectTaskHandle); // poke the task to delete itself at the end of the next animation step, instead of forcibly doing it possibly while FastLED is busy, which seems to blow everything up
    delay(100); // give the task some time to clear the display before we start trying to show the clock again
  }
    if (minute >= 01 && minute <= 14) {
    if (tz.day() == BDAY_DO_DAY && tz.month() == BDAY_DO_MONTH) {
      xTaskCreate(bdayDoEffectLoop, "HappyBdayDominique", 10000, NULL, 1, &bdayEffectTaskHandle);
      delay(60*1000*15);
      xTaskNotifyGive(bdayEffectTaskHandle); // poke the task to delete itself at the end of the next animation step, instead of forcibly doing it possibly while FastLED is busy, which seems to blow everything up
      delay(100); // give the task some time to clear the display before we start trying to show the clock again
    }
  }
//    if (tz.day() == BDAY_CLAIRE_DAY && tz.month() == BDAY_CLAIRE_MONTH && !bdayEffectTaskHandle) {
//      xTaskCreate(bdayClaireEffectLoop, "HappyBdayClaire", 10000, NULL, 1, &bdayEffectTaskHandle);
////        delay(60*1000*15);
////        xTaskNotifyGive(bdayEffectTaskHandle); // poke the task to delete itself at the end of the next animation step, instead of forcibly doing it possibly while FastLED is busy, which seems to blow everything up
////        delay(100); // give the task some time to clear the display before we start trying to show the clock again
//    }
////    }
  
  displayClockFace(hour, minute);

  FastLED.show();

  delay(16);
}
