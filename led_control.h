#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <SDL3/SDL.h>
#include <array>

// Number of LEDs in the display
constexpr int LED_COUNT = 5;

// Global array storing the color of each LED
extern std::array<SDL_Color, LED_COUNT> ledColors;

// Update the color of a single LED.
// ledId: index of the LED in the array
// color: base RGB color (0-255)
// brightness: intensity percentage (0-100)
void updateLedColor(int ledId, SDL_Color color, int brightness);

#endif // LED_CONTROL_H
