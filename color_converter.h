#pragma once
#include <Arduino.h>

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