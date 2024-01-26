#include "settings.h"
#include "timer.h"
#include "effects.h"
#include "LedStrip.h"

#include <SoftwareSerial.h>

SoftwareSerial bluetooth(BT_RX_PIN, BT_TX_PIN); // Arduino RX, Arduino TX

Effect effect = Effect::StripOff;
Timer timer(20, Timer::TimeUnit::MILLIS);

void setup()
{
#if LUMOS_DEBUG
  Serial.begin(9600);
  Logger::setLogLevel(Logger::VERBOSE);
  Logger::verbose("setup()", "Up and running!");
#endif

  initStrip();
  bluetooth.begin(9600);
  randomSeed(analogRead(0));
}

void loop()
{
  effectCycle();
  listenForIncomingCommand();
  parseCommand();
}

inline void effectCycle()
{
  if (stripBrightness == 0)
  {
    return;
  }

  if (timer.isReady())
  {
    switch (effect)
    {
    case Effect::StripOff:
      return;

    case Effect::Fireplace:
      fireplaceCycle();
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

    case Effect::UserColors:
      userColorsCycle();
      break;
    }

    FastLED.show();
  }
}

// Data obtaining part
const uint8_t btBufferSize = 32;
char receivedCmd[btBufferSize];
bool newData = false;

inline void listenForIncomingCommand()
{
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (bluetooth.available() > 0 && !newData)
  {
    rc = bluetooth.read();

    if (recvInProgress)
    {
      if (rc != endMarker)
      {
        receivedCmd[ndx] = rc;
        ndx++;
        if (ndx >= btBufferSize)
        {
          ndx = btBufferSize - 1;
        }
      }
      else
      {
        receivedCmd[ndx] = '\0';
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }
    else if (rc == startMarker)
    {
      recvInProgress = true;
    }
  }
}

inline void parseCommand()
{
  if (newData)
  {
    timer.stop();
    if (receivedCmd[0] == 'e')
    {
      selectEffect(atoi(receivedCmd + 1));
    }

    if (receivedCmd[0] == 'b')
    {
      uint8_t brightness = atoi(receivedCmd + 1);
      stripBrightness = map(brightness, 0, 100, 0, 255);
      FastLED.setBrightness(stripBrightness);

      Serial.print("Brightness is set to: ");
      Serial.println(stripBrightness);
    }

    if (receivedCmd[0] == 'h')
    {
      hue = map(atof(receivedCmd + 1), 0, 360, 0, 255);

      Serial.print("Hue is set to: ");
      Serial.println(hue);
    }

    if (receivedCmd[0] == 'f')
    {
      if (receivedCmd[1] == 'a')
      {
        firefliesAmount = atoi(receivedCmd + 2);

        Serial.print("Fireflies amount is set to: ");
        Serial.println(firefliesAmount);
      }

      if (receivedCmd[1] == 'g')
      {
        uint8_t val = atoi(receivedCmd + 2);
        autogenerateColors = val == 0;

        Serial.print("Autogenerate colors is set to: ");
        Serial.println(val);
      }

      if (receivedCmd[1] == 'h')
      {
        Serial.print("Raw command for fireflies: ");
        Serial.println(receivedCmd);

        uint8_t index = atoi(receivedCmd[2]);

        Serial.print("Selected fireflie index: ");
        Serial.println(index);

        uint8_t f_hue = map(atof(receivedCmd + 4), 0, 360, 0, 255);

        Serial.print("Selected fireflie hue: ");
        Serial.println(f_hue);

        CHSV color = CHSV(f_hue, 255, 255);
        firefliesColors[index] = color;
      }

      firefliesResetFlag = true;
    }

    newData = false;
    timer.start();
  }
}

inline void selectEffect(uint8_t effectOrdinal)
{
  effect = static_cast<Effect>(effectOrdinal);

  Serial.print("Selcted effect ordinal: ");
  Serial.println(effectOrdinal);

  if (effect == Effect::Fireflies)
  {
    firefliesResetFlag = true;
  }

  if (effect == Effect::StripOff)
  {
    timer.stop();
  }
  
  FastLED.clear();
}
