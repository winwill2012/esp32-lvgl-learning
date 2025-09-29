//
// Created by Admin on 2025/7/11.
//

#include "RGB.h"

CRGB RGB::leds[NUM_LEDS];

void RGB::begin()
{
    FastLED.addLeds<WS2812B, DATA_PIN, GRB >(leds, NUM_LEDS);
}

void RGB::powerOn()
{
    leds[0] = CRGB::Red;
    FastLED.show();
}

void RGB::powerOff()
{
    leds[0] = CRGB::Black;
    FastLED.show();
}
