# Lumos arduino

Arduino part of the Lumos LED stripe project - an unique and beautiful DIY bluethooth-controlled room backlight based on WS2812B addressable LED stripe and HC-06 UART bluetooth module.

| | | |
|-|-|-|
| ![Lumos preview 1](assets/lumos-preview-violet.gif) | ![Lumos preview 2](assets/lumos-preview-green.gif) | ![Lumos preview 3](assets/lumos-preview-orange.gif) |

## Essential hardware
* Arduino board
* WS2812B addressable LED stripe
* HC-06 bluetooth module

## Device design
![Device scheme](assets/device-design.png)

> **Note** <br/>
> Since there are no available bluetooth module in [Tinkercad](https://www.tinkercad.com/) components palette it was replaced with some
substitude component with 6 pins. <br/><br/>
Pins we're interested in:<br/>
>```
> VCC(2), GND(3), TXD(4), RXD(5)
>```
>And here is how actual HC-06 bluetooth module looks like:<br/>
<img src="assets/hc-06-bt-module.png" width="300">

## Precompile configuration

> **Note**<br/>
To be able to compile and use this sketch you should install the [FastLED](https://github.com/FastLED/FastLED) arduino library.

This define line allows us to enable or disable debug mode to see (or not see XD) data transfer communications. (Set it to 1 to enable and 0 to disable).

```arduino
#define LUMOS_DEBUG                     0
```

Then we have **LED_COUNT** parameter which defines how many LEDs our stripe have. Put your amount here. **LED_PIN** parameter points to an arduino pin which will send commands to the LED stripe. This pin is wired with stripe's **DIN**.

```arduino
#define LED_COUNT                      60
#define LED_PIN                         7
```

Since we're using two-dimensional Perlin noise we should move around generated values with some step size. You can put your's here. You can play with the values but the default 15 is an optimal one.

```arduino
#define COLOR_SHIFT_STEP               15
```

The next bunch of settings defines by how much components like **Hue**, **Saturation** and **Value** should bend around some base value creating realistic fire effect.

```arduino
#define HUE_DEVIATION                  21
#define MIN_SATURATION_DEVIATION      245
#define MAX_SATURATION_DEVIATION      255
#define MIN_VALUE_DEVIATION            70
#define MAX_VALUE_DEVIATION           200
```

Here we have bluetooth data transfer pins to define. Look carefully how you connected your HC-06 module data lines (RXD, TXD) and provide the correct arduino pins. <br/>
For instance if you connect your HC-06 **RXD** pin to arduino's 10th pin, and **TXD** pin to arduino's 11th pin, then the setup will look like this:

```arduino
#define BT_RX_PIN                      10
#define BT_TX_PIN                      11
```