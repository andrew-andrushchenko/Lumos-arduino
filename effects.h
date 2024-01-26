#pragma once

#include <Arduino.h>
#include "LedStrip.h"
#include "utils.h"

enum class Effect
{
  StripOff = 0,
  Fireplace,
  LavaLamp,
  Rainbow,
  TheaterRainbow,
  Plasma,
  Fireflies,
  Sparkles,
  UserColors
};

// FIREPLACE
inline uint8_t hue = 0;

void fireplaceCycle();
CHSV colorBy(uint8_t val);

// LAVA LAMP
void lavaLampCycle();

// RAINBOW
CRGB wheel(uint8_t WheelPosition);
void rainbowCycle();
void theaterChaseRainbowCycle();

// PLASMA
#define qsuba(x, b) ((x > b) ? x - b : 0)

inline CRGBPalette16 currentPalette = OceanColors_p; // Palette definitions
inline CRGBPalette16 targetPalette;
inline TBlendType currentBlending = LINEARBLEND;

void plasmaCycle(uint8_t freq1, uint8_t freq2, int8_t phase1, int8_t phase2);

// FIREFLIES
#define MAX_SPEED 30

inline uint8_t firefliesAmount = 3;
inline bool firefliesResetFlag = true;

inline int16_t speed[10]; // 10 is max expected FIREFLIES_AMOUNT
inline int16_t pos[10];
inline CRGB firefliesColors[10];
inline bool autogenerateColors = true;

void firefliesCycle();

// SPARKLES
void sparklesCycle();

// USER COLORS. TODO: temporary and should be removed!
inline uint8_t pairNo = 0;
inline uint8_t ucHues[8] = { 0, 50, 80, 90, 120, 150, 170, 200 };
void userColorsCycle();