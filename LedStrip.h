#pragma once

#include <FastLED.h>
#include "settings.h"

inline CRGB leds[LED_COUNT];
inline uint8_t stripBrightness = 0;

void initStrip();

void fillAll(CRGB newColor);
uint32_t ledColorAt(uint16_t position);
void fade();