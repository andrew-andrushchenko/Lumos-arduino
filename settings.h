#pragma once

#define LUMOS_DEBUG 0

#define CURRENT_LIMIT 2000  // The current limit (mA). Leads to automatic brightness correction. 0 to turn off.
#define STRIPES_COUNT 1     // Number of LED stripes connected in parallel.
#define LED_COUNT 60
#define LED_PIN 7

#define BT_RX_PIN 10
#define BT_TX_PIN 11

#if LUMOS_DEBUG
#include <Logger.h>
#endif