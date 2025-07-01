#include "led_control.h"

std::array<SDL_Color, LED_COUNT> ledColors;

void updateLedColor(int ledId, SDL_Color color, int brightness) {
    if (ledId < 0 || ledId >= LED_COUNT)
        return;
    SDL_Color adjusted{
        Uint8(color.r * brightness / 100),
        Uint8(color.g * brightness / 100),
        Uint8(color.b * brightness / 100),
        255
    };
    ledColors[ledId] = adjusted;
}
