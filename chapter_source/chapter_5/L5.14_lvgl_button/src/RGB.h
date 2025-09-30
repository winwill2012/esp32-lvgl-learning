//
// Created by Admin on 2025/7/11.
//

#ifndef RGB_H
#define RGB_H

#include "FastLED.h"

#define DATA_PIN 18
#define NUM_LEDS 1

class RGB
{
public:
    static void begin();
    static void powerOn();
    static void powerOff();

private :
    static CRGB leds[NUM_LEDS];
};


#endif //RGB_H
