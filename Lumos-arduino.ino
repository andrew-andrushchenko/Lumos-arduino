#define LUMOS_DEBUG                     0

#define LED_COUNT                      60
#define LED_PIN                         7

#define COLOR_SHIFT_STEP               15

#define HUE_DEVIATION                  21
#define MIN_SATURATION_DEVIATION      245
#define MAX_SATURATION_DEVIATION      255
#define MIN_VALUE_DEVIATION            70
#define MAX_VALUE_DEVIATION           200

#define BT_RX_PIN                      10
#define BT_TX_PIN                      11

#if LUMOS_DEBUG == 1
#define LOG_D(x) Serial.println(x)
#else
#define LOG_D(x)
#endif

#include <FastLED.h>
#include <SoftwareSerial.h>

CRGB leds[LED_COUNT];
SoftwareSerial bluetooth(BT_RX_PIN, BT_TX_PIN); // Arduino RX, Arduino TX

uint8_t hue = 0;
uint8_t brightnessLevel = 0;

void setup()
{
#if LUMOS_DEBUG == 1
    Serial.begin(9600);
#endif
    LOG_D("Listening for incoming data...");

    FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, LED_COUNT);
    FastLED.setBrightness(brightnessLevel);

    bluetooth.begin(9600);
}

void loop()
{
    stripeCycle();

    if (bluetooth.available())
    {
        String cmd = "";
        while (bluetooth.available())
        {
            cmd += (char) bluetooth.read();
            m_delay(30);
        }

        LOG_D("Obtained value: " + cmd);

        parseCommand(cmd);
    }
}

inline void m_delay(int delayMillis)
{
    unsigned long prevMillis = millis();
    while (millis() - prevMillis <= delayMillis);
}

inline void parseCommand(const String& cmd)
{
    if (cmd.charAt(0) == 'c')
    {
        hue = map(cmd.substring(1).toInt(), 0, 360, 0, 255);

        LOG_D(hue);
    }

    if (cmd.charAt(0) == 'b')
    {
        int brightness = cmd.substring(1).toInt();
        brightnessLevel = map(brightness, 0, 100, 0, 255);
        FastLED.setBrightness(brightnessLevel);

        LOG_D(brightnessLevel);
    }
}

int counter = 0;
inline void stripeCycle()
{
    for (int i = 0; i < LED_COUNT; i++)
    {
        leds[i] = getLEDColor((inoise8(i * COLOR_SHIFT_STEP, counter)));
    }

    counter += 20;
    FastLED.show();

    m_delay(20);
}

inline CHSV getLEDColor(uint8_t val)
{
    return CHSV(
        hue + map(val, 0, 255, 0, HUE_DEVIATION),
        constrain(map(val, 0, 255, MAX_SATURATION_DEVIATION, MIN_SATURATION_DEVIATION), 0, 255),
        constrain(map(val, 0, 255, MIN_VALUE_DEVIATION, MAX_VALUE_DEVIATION), 0, 255)
    );
}