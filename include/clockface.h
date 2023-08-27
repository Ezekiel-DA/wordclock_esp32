#pragma once

#include "utils.h"

void it(CRGB c = CRGB::White) {
    _leds[0] = c;
    _leds[1] = c;
}

void is(CRGB c = CRGB::White) {
    _leds[3] = c;
    _leds[4] = c;
}

void tenM(CRGB c = CRGB::White) {
    _leds[6] = c;
    _leds[7] = c;
    _leds[8] = c;
}

void half(CRGB c = CRGB::White) {
    _leds[9] = c;
    _leds[10] = c;
    _leds[11] = c;
    _leds[12] = c;
}

void quarter(CRGB c = CRGB::White) {
    _leds[13] = c;
    _leds[14] = c;
    _leds[15] = c;
    _leds[16] = c;
    _leds[17] = c;
    _leds[18] = c;
    _leds[19] = c;
}

void twenty(CRGB c = CRGB::White) {
    _leds[20] = c;
    _leds[21] = c;
    _leds[22] = c;
    _leds[23] = c;
    _leds[24] = c;
    _leds[25] = c;
}

void fiveM(CRGB c = CRGB::White) {
    _leds[26] = c;
    _leds[27] = c;
    _leds[28] = c;
    _leds[29] = c;
}

void minutes(CRGB c = CRGB::White) {
    _leds[31] = c;
    _leds[32] = c;
    _leds[33] = c;
    _leds[34] = c;
    _leds[35] = c;
    _leds[36] = c;
    _leds[37] = c;
}

void past(CRGB c = CRGB::White) {
    _leds[46] = c;
    _leds[47] = c;
    _leds[48] = c;
    _leds[49] = c;
}

void to(CRGB c = CRGB::White) {
    _leds[50] = c;
    _leds[51] = c;
}

void three(CRGB c = CRGB::White) {
    _leds[60] = c;
    _leds[61] = c;
    _leds[62] = c;
    _leds[63] = c;
    _leds[64] = c;
}

void eleven(CRGB c = CRGB::White) {
    _leds[65] = c;
    _leds[66] = c;
    _leds[67] = c;
    _leds[68] = c;
    _leds[69] = c;
    _leds[70] = c;
}

void four(CRGB c = CRGB::White) {
    _leds[71] = c;
    _leds[72] = c;
    _leds[73] = c;
    _leds[74] = c;
}

void one(CRGB c = CRGB::White) {
    _leds[75] = c;
    _leds[76] = c;
    _leds[77] = c;
}

void two(CRGB c = CRGB::White) {
    _leds[78] = c;
    _leds[79] = c;
    _leds[80] = c;
}

void eight(CRGB c = CRGB::White) {
    _leds[86] = c;
    _leds[87] = c;
    _leds[88] = c;
    _leds[89] = c;
    _leds[90] = c;
}

void nine(CRGB c = CRGB::White) {
    _leds[91] = c;
    _leds[92] = c;
    _leds[93] = c;
    _leds[94] = c;
}

void seven(CRGB c = CRGB::White) {
    _leds[95] = c;
    _leds[96] = c;
    _leds[97] = c;
    _leds[98] = c;
    _leds[99] = c;
}

void five(CRGB c = CRGB::White) {
    _leds[100] = c;
    _leds[101] = c;
    _leds[102] = c;
    _leds[103] = c;
}

void six(CRGB c = CRGB::White) {
    _leds[104] = c;
    _leds[105] = c;
    _leds[106] = c;
}

void ten(CRGB c = CRGB::White) {
    _leds[114] = c;
    _leds[115] = c;
    _leds[116] = c;
}

void twelve(CRGB c = CRGB::White) {
    _leds[117] = c;
    _leds[118] = c;
    _leds[119] = c;
    _leds[120] = c;
    _leds[121] = c;
    _leds[122] = c;
}

void oclock(CRGB c = CRGB::White) {
    _leds[124] = c;
    _leds[125] = c;
    _leds[126] = c;
    _leds[127] = c;
    _leds[128] = c;
    _leds[129] = c;
}

void minute1(bool state=true) {
  digitalWrite(TOP_LEFT_LED_PIN, state);
}

void minute2(bool state=true) {
  digitalWrite(TOP_RIGHT_LED_PIN, state);
}

void minute3(bool state=true) {
  digitalWrite(BOTTOM_LEFT_LED_PIN, state);
}

void minute4(bool state=true) {
  digitalWrite(BOTTOM_RIGHT_LED_PIN, state);
}

void displayClockFace(uint8_t hour, uint8_t minute) {
  clearClockFace(false); // soft clear, i.e. don't include bday areas, as we might be running bday code in a separate thread
  it();
  is();

    switch (minute % 5) { // NB: all fall through is by design; if 4 mins, 4,3,2 and 1 need to be lit, if 3 mins, 3,2 and 1, etc.
        case 4:
          minute4();
        case 3:
          minute3();
        case 2:
          minute2();
        case 1:
          minute1();
      }

  if (minute < 5)
      oclock();
  else if (minute >= 5 && minute < 10) {
      fiveM();
      minutes();
      past();
  } else if (minute >= 10 && minute < 15) {
      tenM();
      minutes();
      past();
  } else if (minute >= 15 && minute < 20) {
      quarter();
      past();
  } else if (minute >= 20 && minute < 25) {
      twenty();
      minutes();
      past();
  } else if (minute >= 25 && minute < 30) {
      twenty();
      fiveM();
      minutes();
      past();
  } else if (minute >= 30 && minute < 35) {
      half();
      past();
  } else if (minute >= 35 && minute < 40) {
      twenty();
      fiveM();
      minutes();
      to();
  } else if (minute >= 40 && minute < 45) {
      twenty();
      minutes();
      to();
  } else if (minute >= 45 && minute < 50) {
      quarter();
      to();
  } else if (minute >= 50 && minute < 55) {
      tenM();
      minutes();
      to();
  } else if (minute >= 55) {
      fiveM();
      minutes();
      to();
  }

  switch (hour) {
      case 0:
      case 12:
          minute <= 34 ? twelve() : one();
          break;
      case 1:
      case 13:
          minute <= 34 ? one() : two();
          break;
      case 2:
      case 14:
          minute <= 34 ? two() : three();
          break;
      case 3:
      case 15:
          minute <= 34 ? three() : four();
          break;
      case 4:
      case 16:
          minute <= 34 ? four() : five();
          break;
      case 5:
      case 17:
          minute <= 34 ? five() : six();
          break;
      case 6:
      case 18:
          minute <= 34 ? six() : seven();
          break;
      case 7:
      case 19:
          minute <= 34 ? seven() : eight();
          break;
      case 8:
      case 20:
          minute <= 34 ? eight() : nine();
          break;
      case 9:
      case 21:
          minute <= 34 ? nine() : ten();
          break;
      case 10:
      case 22:
          minute <= 34 ? ten() : eleven();
          break;
      case 11:
      case 23:
          minute <= 34 ? eleven() : twelve();
          break;
  }
}
