#ifndef __DISPLAYSDL_H__
#define __DISPLAYSDL_H__

#include <cstdint>
#include <SDL3/SDL.h>
#include "display.h"
#include "logger.h"

class TDisplaySDL : public TDisplay
{
    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;

    //Log
    std::shared_ptr<TLogger> m_logger;

public:
    TDisplaySDL();
    ~TDisplaySDL();

    virtual void init() override;
    virtual void draw(uint8_t framebuffer[][64],uint16_t width,uint16_t height)override;
    virtual void update()override;
    virtual void deinit()override;
};

#endif