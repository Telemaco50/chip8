#include <cstdint>
#include <SDL3/SDL.h>

#include "keyboardsdl.h"

// Traduce una tecla del PC a la tecla del CHIP-8 (0x0-0xF).
// Devuelve -1 si la tecla no pertenece al teclado del CHIP-8.
//
//   CHIP-8        PC
//   1 2 3 C       1 2 3 4
//   4 5 6 D       Q W E R
//   7 8 9 E       A S D F
//   A 0 B F       Z X C V
static int traducirTecla(SDL_Keycode tecla)
{
    switch (tecla)
    {
        case SDLK_1: return 0x1;
        case SDLK_2: return 0x2;
        case SDLK_3: return 0x3;
        case SDLK_4: return 0xC;
        case SDLK_Q: return 0x4;   // en SDL3 las letras van en mayúscula: SDLK_Q (en SDL2 era SDLK_q)
        case SDLK_W: return 0x5;
        case SDLK_E: return 0x6;
        case SDLK_R: return 0xD;
        case SDLK_A: return 0x7;
        case SDLK_S: return 0x8;
        case SDLK_D: return 0x9;
        case SDLK_F: return 0xE;
        case SDLK_Z: return 0xA;
        case SDLK_X: return 0x0;
        case SDLK_C: return 0xB;
        case SDLK_V: return 0xF;
        default:     return -1;
    }
}

TKeyboardSDL::TKeyboardSDL()
{
    m_logger = TLogger::getInstance();
}

TKeyboardSDL::~TKeyboardSDL()
{

}

void TKeyboardSDL::init()
{

}

void TKeyboardSDL::update(uint8_t* key_map, bool* running)
{
    // Procesar TODOS los eventos pendientes (while, no if).
    // Este debe ser el ÚNICO sitio del programa que llama a SDL_PollEvent.
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP:
            {
                // ESC también cierra el emulador
                if (e.key.key == SDLK_ESCAPE)
                {
                    *running = false;
                    break;
                }

                int tecla = traducirTecla(e.key.key);   // en SDL3: e.key.key (en SDL2: e.key.keysym.sym)
                if (tecla != -1)
                {
                    // Pulsada = 1, soltada = 0
                    key_map[tecla] = (e.type == SDL_EVENT_KEY_DOWN) ? 1 : 0;
                }
                break;
            }

            case SDL_EVENT_QUIT:   // clic en la X de la ventana
                *running = false;
                break;

            default:
                break;
        }
    }
}

void TKeyboardSDL::deinit()
{

}
