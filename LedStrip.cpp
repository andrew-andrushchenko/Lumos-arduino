#include "LedStrip.h"

void initStrip()
{
  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, LED_COUNT).setCorrection( TypicalLEDStrip );
  if (CURRENT_LIMIT > 0)
  {
      FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT / STRIPES_COUNT);
  }
  FastLED.setBrightness(stripBrightness);
}

void fillAll(CRGB newColor)
{
  for (int i = 0; i < LED_COUNT; i++)
  {
    leds[i] = newColor;
  }
}

uint32_t ledColorAt(uint16_t position)
{
  return (((uint32_t)leds[position].r << 16) | ((uint32_t)leds[position].g << 8 ) | (uint32_t)leds[position].b);
}

void fade()
{
#if LED_COUNT < 255
  uint8_t i;
#else
  uint16_t i;
#endif

  for (i = 0; i < LED_COUNT; i++) 
  {
    if (ledColorAt(i) == 0) continue;
    leds[i].fadeToBlackBy(50);
  }
}