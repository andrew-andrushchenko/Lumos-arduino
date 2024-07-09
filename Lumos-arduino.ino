#include "settings.h"
#include "LedStrip.h"
#include "effects.h"
#include "timer.h"

#include <SoftwareSerial.h>

struct HsvColor
{
  uint8_t h;
  uint8_t s;
  uint8_t v;
};

HsvColor RgbToHsv(uint8_t r,
                  uint8_t g,
                  uint8_t b)
{
  HsvColor hsv;
  uint8_t rgbMin, rgbMax;

  rgbMin = r < g ? (r < b ? r : b) : (g < b ? g : b);
  rgbMax = r > g ? (r > b ? r : b) : (g > b ? g : b);

  hsv.v = rgbMax;
  if (hsv.v == 0)
  {
    hsv.h = 0;
    hsv.s = 0;
    return hsv;
  }

  hsv.s = 255 * long(rgbMax - rgbMin) / hsv.v;
  if (hsv.s == 0)
  {
    hsv.h = 0;
    return hsv;
  }

  if (rgbMax == r)
    hsv.h = 0 + 43 * (g - b) / (rgbMax - rgbMin);
  else if (rgbMax == g)
    hsv.h = 85 + 43 * (b - r) / (rgbMax - rgbMin);
  else
    hsv.h = 171 + 43 * (r - g) / (rgbMax - rgbMin);

  return hsv;
}

SoftwareSerial bt_serial(BT_RX_PIN, BT_TX_PIN); // Arduino RX, Arduino TX

Effect effect = Effect::StripOff;
Timer timer(20, Timer::TimeUnit::MILLIS);

void setup()
{
  pinMode(BT_RX_PIN, INPUT);
  pinMode(BT_TX_PIN, OUTPUT);
  randomSeed(analogRead(0));

  initStrip();
  bt_serial.begin(9600);

#if LUMOS_DEBUG
  Serial.begin(9600);
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
  Log.verbose("%s\n", "Init successful");
#endif
}

void loop()
{
  effectLoop();
  tryReceiveData();
  parseData();
}

void effectLoop()
{
  if (stripBrightness == 0)
    return;

  if (!timer.isReady())
    return;

  switch (effect)
  {
  case Effect::StripOff:
    return;

  case Effect::Fireplace:
    fireplaceCycle();
    Log.verbose("fireplaceCycle() end. Brightness: %d, fireplaceHue: %d\n", stripBrightness, fireplaceHue);
    break;

  case Effect::LavaLamp:
    lavaLampCycle();
    break;

  case Effect::Rainbow:
    rainbowCycle();
    break;

  case Effect::TheaterRainbow:
    theaterChaseRainbowCycle();
    break;

  case Effect::Plasma:
    plasmaCycle(23, 15, 6, 7);
    break;

  case Effect::Fireflies:
    firefliesCycle();
    break;

  case Effect::Sparkles:
    sparklesCycle();
    break;
  }

  FastLED.show();
}

// Data obtaining part
const uint8_t bufferSize = 20;
const char startMarker = '!';
const char endMarker = '$';

char receivedCmd[bufferSize];
bool newDataAvailable = false;

bool isTransferInProgress = false;
uint8_t r_index = 0;

void tryReceiveData()
{
  char receivedChar;

  while (bt_serial.available() > 0 && !newDataAvailable)
  {
    receivedChar = bt_serial.read();

    if (isTransferInProgress)
    {
      if (receivedChar != endMarker)
      {
        receivedCmd[r_index] = receivedChar;

        r_index++;

        if (r_index >= bufferSize)
          r_index = bufferSize - 1;
      }
      else
      {
        receivedCmd[r_index] = '\0';
        isTransferInProgress = false;
        r_index = 0;
        newDataAvailable = true;
      }
    }
    else if (receivedChar == startMarker)
    {
      isTransferInProgress = true;
    }
  }
}

void parseData()
{
  if (!newDataAvailable)
    return;

  timer.stop();
#if LUMOS_DEBUG
  Log.verbose("%s -> %s\n", "Raw received line", receivedCmd);
#endif
  char *pch;
  long tokens[3] = {0};

  uint8_t i = 0;
  pch = strtok(receivedCmd, ";");
  while (pch != NULL)
  {
    tokens[i] = strtol(pch, NULL, 0);

    pch = strtok(NULL, ";");
    i++;
  }

#if LUMOS_DEBUG
  for (uint8_t j = 0; j < 3; j++)
  {
    Log.verbose("%s[%d] = %l\n", "tokens", j, tokens[j]);
  }
#endif
  // Data parsing logic
  // Command stored in tokens[0]
  switch (tokens[0])
  {
  case 0: // Change effect
    selectEffect((uint8_t)tokens[1]);
#if LUMOS_DEBUG
    Log.verbose("%s: %d\n", "Selected effect ordinal", effect);
#endif
    break;
  case 1: // Change brightness
    stripBrightness = map(tokens[1], 0, 100, 0, 255);
#if LUMOS_DEBUG
    Log.verbose("%s: %d\n", "Brightness", stripBrightness);
#endif
    break;
  case 2: // Change fireplace color
    // RGB long stored in tokens[1]
    int r = (tokens[1] >> 16) & 0xFF;
    int g = (tokens[1] >> 8) & 0xFF;
    int b = tokens[1] & 0xFF;

    HsvColor color = RgbToHsv(r, g, b);
    fireplaceHue = color.h;

#if LUMOS_DEBUG
    Log.verbose("%s: %l\n", "Raw rgb color int", tokens[1]);
    Log.verbose("%s: (%d, %d, %d)\n", "Result RGB", r, g, b);
    Log.verbose("%s: %d\n", "H component", fireplaceHue);
#endif
    break;
  case 3: // Change fireflies amount
    // Fireflies amount num stored in tokens[1]
    firefliesAmount = (uint8_t)tokens[1];
    firefliesResetFlag = true;
#if LUMOS_DEBUG
    Log.verbose("%s: %d\n", "Fireflies amount", firefliesAmount);
#endif
    break;
  case 4: // Change fireflies color generation mode
    autogenerateColors = tokens[1] == 1;
    firefliesResetFlag = true;
#if LUMOS_DEBUG
    Log.verbose("%s: %T\n", "Fireflies color autogenerate", autogenerateColors);
#endif
    break;
  case 5: // Change firefly color
    // Firefly to change is stored in tokens[1]
    // Color to change is stored in tokens[2]
    int ff_r = (tokens[2] >> 16) & 0xFF;
    int ff_g = (tokens[2] >> 8) & 0xFF;
    int ff_b = tokens[2] & 0xFF;

    firefliesColors[tokens[1]] = CRGB{ff_r, ff_g, ff_b};
    firefliesResetFlag = true;
#if LUMOS_DEBUG
    Log.verbose("%s: %l\n", "Raw rgb color int", tokens[1]);
    Log.verbose("%s[%l] color: RGB(%d, %d, %d)\n", "Fireflies", tokens[1], ff_r, ff_g, ff_b);
#endif
    break;
  }

  newDataAvailable = false;
  timer.start();
}

void selectEffect(uint8_t effectOrdinal)
{
  effect = static_cast<Effect>(effectOrdinal);

  if (effect == Effect::StripOff)
    timer.stop();

  if (effect == Effect::Fireflies)
    firefliesResetFlag = true;

  FastLED.clear();
}
