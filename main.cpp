#include <SDL3/SDL.h>
#include <array>
#include <cmath>
#include <cstdio>
#include "led_control.h"
const int LED_RADIUS = 15;
const int PENTAGRAM_RADIUS = 150;
const int DISPLAY_SIZE = 400;
const int PANEL_WIDTH = 400;
const int PANEL_HEIGHT = 500;

struct Vec2 { float x; float y; };

std::array<Vec2, LED_COUNT> pentagramPoints(float cx, float cy, float r) {
    std::array<Vec2, LED_COUNT> pts{};
    for (int i = 0; i < LED_COUNT; ++i) {
        double angle = M_PI / 2 + i * 2 * M_PI / LED_COUNT;
        pts[i] = {cx + r * std::cos(angle), cy - r * std::sin(angle)};
    }
    return pts;
}

struct Button {
    SDL_Rect rect;
    bool down = false;
};

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::fprintf(stderr, "SDL init failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* displayWin = SDL_CreateWindow("LEDs", DISPLAY_SIZE, DISPLAY_SIZE, 0);
    SDL_Window* panelWin = SDL_CreateWindow("Control", PANEL_WIDTH, PANEL_HEIGHT, 0);
    SDL_SetWindowPosition(displayWin, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_SetWindowPosition(panelWin, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    if (!displayWin || !panelWin) {
        std::fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_Renderer* displayRen = SDL_CreateRenderer(displayWin, nullptr);
    SDL_Renderer* panelRen = SDL_CreateRenderer(panelWin, nullptr);
    if (!displayRen || !panelRen) {
        std::fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    auto ledPositions = pentagramPoints(DISPLAY_SIZE/2.0f, DISPLAY_SIZE/2.0f, PENTAGRAM_RADIUS);
    ledColors.fill({0,0,0,255});

    auto btnPositions = pentagramPoints(PANEL_WIDTH/2.0f, PANEL_WIDTH/4.0f, PENTAGRAM_RADIUS/2.0f);
    std::array<Button, LED_COUNT> buttons;
    for (int i=0;i<LED_COUNT;++i) {
        buttons[i].rect = { (int)btnPositions[i].x - 30, (int)btnPositions[i].y - 15, 60,30 };
    }
    Button selectAllBtn{ {PANEL_WIDTH/2 - 50, (int)(PANEL_WIDTH/2.0f), 100,30} };
    SDL_Rect colorSlider{50, PANEL_WIDTH/2 + 50, PANEL_WIDTH-100, 20};
    SDL_Rect brightSlider{50, PANEL_WIDTH/2 + 100, PANEL_WIDTH-100, 20};
    Button applyBtn{ {PANEL_WIDTH/2 - 120, PANEL_WIDTH/2 + 150, 100,30} };
    Button resetBtn{ {PANEL_WIDTH/2 + 20, PANEL_WIDTH/2 + 150, 100,30} };
    int color = 0;
    int brightness = 100;

    bool running = true;
    while(running) {
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) running = false;
            if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && e.button.windowID == SDL_GetWindowID(panelWin)) {
                int mx = e.button.x; int my = e.button.y;
                SDL_Point click{mx, my};
                for(int i=0;i<LED_COUNT;++i) {
                    if(SDL_PointInRect(&click, &buttons[i].rect))
                        buttons[i].down = !buttons[i].down;
                }
                if(SDL_PointInRect(&click, &selectAllBtn.rect)) {
                    for(auto &b:buttons) b.down = true;
                }
                if(SDL_PointInRect(&click, &applyBtn.rect)) {
                    float sat = 1.0f; // fixed
                    for(int i=0;i<LED_COUNT;++i) {
                        if(buttons[i].down) {
                            float angle = color * M_PI/180.0f;
                            float r = 0.5f*(std::sin(angle)+1);
                            float g = 0.5f*(std::sin(angle+2*M_PI/3)+1);
                            float b = 0.5f*(std::sin(angle+4*M_PI/3)+1);
                            SDL_Color base{ Uint8(r*255), Uint8(g*255), Uint8(b*255), 255 };
                            updateLedColor(i, base, brightness);
                        }
                    }
                }
                if(SDL_PointInRect(&click, &resetBtn.rect)) {
                    color = 0; brightness = 100;
                    for(auto &b: buttons) { b.down = false; }
                    ledColors.fill({0,0,0,255});
                }
            }
            if(e.type == SDL_EVENT_MOUSE_MOTION && e.motion.state & SDL_BUTTON_LMASK && e.motion.windowID == SDL_GetWindowID(panelWin)) {
                int mx = e.motion.x;
                if(mx >= colorSlider.x && mx <= colorSlider.x + colorSlider.w)
                    color = int( (mx - colorSlider.x) * 359 / colorSlider.w );
                if(mx >= brightSlider.x && mx <= brightSlider.x + brightSlider.w)
                    brightness = int( (mx - brightSlider.x) * 100 / brightSlider.w );
            }
        }

        SDL_SetRenderDrawColor(displayRen,255,255,255,255);
        SDL_RenderClear(displayRen);
        // pentagram lines
        for(int i=0;i<LED_COUNT;i++){
            int j=(i+2)%LED_COUNT;
            SDL_RenderLine(displayRen,
                ledPositions[i].x, ledPositions[i].y,
                ledPositions[j].x, ledPositions[j].y);
        }
        for(int i=0;i<LED_COUNT;i++){
            SDL_SetRenderDrawColor(displayRen,ledColors[i].r,ledColors[i].g,ledColors[i].b,255);
            for(int dx=-LED_RADIUS;dx<=LED_RADIUS;dx++){
                for(int dy=-LED_RADIUS;dy<=LED_RADIUS;dy++){
                    if(dx*dx+dy*dy<=LED_RADIUS*LED_RADIUS)
                        SDL_RenderPoint(displayRen, ledPositions[i].x + dx, ledPositions[i].y + dy);
                }
            }
        }
        SDL_RenderPresent(displayRen);

        SDL_SetRenderDrawColor(panelRen,245,245,245,255);
        SDL_RenderClear(panelRen);
        // draw buttons
        SDL_SetRenderDrawColor(panelRen,0,0,0,255);
        for(int i=0;i<LED_COUNT;++i){
            SDL_FRect r{(float)buttons[i].rect.x, (float)buttons[i].rect.y,
                        (float)buttons[i].rect.w, (float)buttons[i].rect.h};
            SDL_RenderRect(panelRen,&r);
            if(buttons[i].down){
                SDL_RenderFillRect(panelRen,&r);
            }
        }
        SDL_FRect selRect{(float)selectAllBtn.rect.x,(float)selectAllBtn.rect.y,
                          (float)selectAllBtn.rect.w,(float)selectAllBtn.rect.h};
        SDL_FRect applyRect{(float)applyBtn.rect.x,(float)applyBtn.rect.y,
                            (float)applyBtn.rect.w,(float)applyBtn.rect.h};
        SDL_FRect resetRect{(float)resetBtn.rect.x,(float)resetBtn.rect.y,
                            (float)resetBtn.rect.w,(float)resetBtn.rect.h};
        SDL_FRect colorRect{(float)colorSlider.x,(float)colorSlider.y,
                            (float)colorSlider.w,(float)colorSlider.h};
        SDL_FRect brightRect{(float)brightSlider.x,(float)brightSlider.y,
                             (float)brightSlider.w,(float)brightSlider.h};
        SDL_RenderRect(panelRen,&selRect);
        SDL_RenderRect(panelRen,&applyRect);
        SDL_RenderRect(panelRen,&resetRect);
        SDL_RenderRect(panelRen,&colorRect);
        SDL_RenderRect(panelRen,&brightRect);
        // slider indicators
        SDL_FRect colorPos{(float)(colorSlider.x + color * colorSlider.w / 359 - 5), (float)colorSlider.y - 5.f, 10.f, (float)colorSlider.h + 10.f};
        SDL_FRect brightPos{(float)(brightSlider.x + brightness * brightSlider.w / 100 - 5), (float)brightSlider.y - 5.f, 10.f, (float)brightSlider.h + 10.f};
        SDL_RenderFillRect(panelRen,&colorPos);
        SDL_RenderFillRect(panelRen,&brightPos);
        SDL_RenderPresent(panelRen);
        SDL_Delay(16);
    }
    SDL_DestroyRenderer(displayRen);
    SDL_DestroyRenderer(panelRen);
    SDL_DestroyWindow(displayWin);
    SDL_DestroyWindow(panelWin);
    SDL_Quit();
    return 0;
}

