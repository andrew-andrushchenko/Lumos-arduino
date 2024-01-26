#include "effects.h"

// FIREPLACE
#define COLOR_SHIFT_STEP 15

#define HUE_DEVIATION 21
#define MIN_SATURATION_DEVIATION 245
#define MAX_SATURATION_DEVIATION 255
#define MIN_VALUE_DEVIATION 70
#define MAX_VALUE_DEVIATION 200

void fireplaceCycle()
{
  static uint32_t counter = 0;

#if LED_COUNT < 255
  uint8_t i;
#else
  uint16_t i;
#endif

  for (i = 0; i < LED_COUNT; i++)
  {
    leds[i] = colorBy((inoise8(i * COLOR_SHIFT_STEP, counter)));
  }

  counter += 20;
}

CHSV colorBy(uint8_t val)
{
  return CHSV(
      hue + map(val, 0, 255, 0, HUE_DEVIATION),
      constrain(map(val, 0, 255, MAX_SATURATION_DEVIATION, MIN_SATURATION_DEVIATION), 0, 255),
      constrain(map(val, 0, 255, MIN_VALUE_DEVIATION, MAX_VALUE_DEVIATION), 0, 255));
}

// LAVA LAMP
void lavaLampCycle()
{
  static uint32_t counter = 0;

#if LED_COUNT < 255
  uint8_t i;
#else
  uint16_t i;
#endif

  for (i = 0; i < LED_COUNT; i++)
  {
    uint32_t pixelHue = counter + (i * 65536L / LED_COUNT);
    leds[i] = pixelHue;
  }

  if (counter == 5 * 65536)
  {
    counter = 0;
  }

  counter += 256;
}

// RAINBOW
CRGB wheel(uint8_t WheelPosition)
{
  static CRGB c;

  if (WheelPosition < 85)
  {
    c.r = WheelPosition * 3;
    c.g = 255 - WheelPosition * 3;
    c.b = 0;
  }
  else if (WheelPosition < 170)
  {
    WheelPosition -= 85;
    c.r = 255 - WheelPosition * 3;
    c.g = 0;
    c.b = WheelPosition * 3;
  }
  else
  {
    WheelPosition -= 170;
    c.r = 0;
    c.g = WheelPosition * 3;
    c.b = 255 - WheelPosition * 3;
  }

  return c;
}

void rainbowCycle()
{
  static uint16_t j = 0;

#if LED_COUNT < 255
  uint8_t i;
#else
  uint16_t i;
#endif

  for (i = 0; i < LED_COUNT; i++)
  {
    CRGB color = wheel(((i * 256 / LED_COUNT) + j) & 255);
    leds[LED_COUNT - 1 - i] = color;
  }

  if (j == 256)
    j = 0;

  j++;
}

void theaterChaseRainbowCycle()
{
#if LED_COUNT < 255
  uint8_t i;
  static uint8_t j = 0;
#else
  uint16_t i;
  static uint16_t j = 0;
#endif

  for (uint8_t q = 0; q < 3; q++)
  {
    for (i = 0; i < LED_COUNT; i += 3)
    {
      CRGB c = wheel((i + j) % 255);
      leds[i + q] = c;
    }

    FastLED.show();

    m_delay(50);

    for (i = 0; i < LED_COUNT; i += 3)
    {
      leds[i + q] = CRGB{0, 0, 0};
    }
  }

  if (j == LED_COUNT)
    j = 0;
  j++;
}

// PLASMA
void plasmaCycle(uint8_t freq1, uint8_t freq2, int8_t phase1, int8_t phase2)
{
  uint8_t thisPhase = beatsin8(phase1, -64, 64); // Setting phase change for a couple of waves.
  uint8_t thatPhase = beatsin8(phase2, -64, 64);

#if LED_COUNT < 255
  uint8_t k;
#else
  uint16_t k;
#endif

  if (LED_COUNT >= 10)
  {
    for (k = 0; k < LED_COUNT; k++)
    {
      uint8_t colorIndex = cubicwave8((k * freq1) + thisPhase) / 2 + cos8((k * freq2) + thatPhase) / 2;
      uint8_t thisBright = qsuba(colorIndex, beatsin8(7, 0, 96));
      leds[k] = ColorFromPalette(currentPalette, colorIndex, thisBright, currentBlending);
    }
  }

  uint8_t maxChanges = 24;
  nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);

  uint8_t baseC = random8();
  targetPalette = CRGBPalette16(CHSV(baseC + random8(32), 192, random8(128, 255)),
                                CHSV(baseC + random8(32), 255, random8(128, 255)),
                                CHSV(baseC + random8(32), 192, random8(128, 255)),
                                CHSV(baseC + random8(32), 255, random8(128, 255)));
}

// FIREFLIES
void firefliesCycle()
{
  if (firefliesResetFlag)
  {
    firefliesResetFlag = false;

    for (uint8_t i = 0; i < firefliesAmount; i++)
    {
      if (autogenerateColors)
      {
        firefliesColors[i] = CHSV(random(0, 9) * 28, 255, 255);
      }
      pos[i] = random(0, LED_COUNT);
      speed[i] += random(-5, 6);
    }
  }

  FastLED.clear();

  for (uint8_t i = 0; i < firefliesAmount; i++)
  {
    speed[i] += random(-5, 6);

    if (speed[i] == 0)
      speed[i] += (-5, 6);

    if (abs(speed[i]) > MAX_SPEED)
      speed[i] = 0;

    pos[i] += speed[i] / 10;

    if (pos[i] < 0)
    {
      pos[i] = 0;
      speed[i] = -speed[i];
    }

    if (pos[i] > LED_COUNT - 1)
    {
      pos[i] = LED_COUNT - 1;
      speed[i] = -speed[i];
    }

    leds[pos[i]] = firefliesColors[i];
  }
}

// SPARKLES
void sparklesCycle()
{
  uint8_t thisNum = random(0, LED_COUNT);
  if (ledColorAt(thisNum) == 0)
  {
    leds[thisNum] = CHSV(random(0, 255), 255, 255);
  }

  fade();
}

// USER COLORS. TODO: temporary and should be removed!
void userColorsCycle()
{
 #if LED_COUNT < 255
  uint8_t i;
#else
  uint16_t i;
#endif

  for (i = 0; i < LED_COUNT; i++)
  {
    leds[i] = CHSV(ucHues[(pairNo + i % 2) % 8], 255, 255);
  }
}