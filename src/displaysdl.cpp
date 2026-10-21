#include <cstdint>
#include <cstdlib>
#include <string>
#include <SDL3/SDL.h>

#include "displaysdl.h"

// Cada píxel del CHIP-8 se pinta como un cuadrado de kZoom x kZoom píxeles reales
constexpr int kZoom = 10;

TDisplaySDL::TDisplaySDL()
{
    m_logger = TLogger::getInstance();
}

TDisplaySDL::~TDisplaySDL()
{

}

void TDisplaySDL::init()
{
    // Inicializar SDL. En SDL3 devuelve bool (true = OK); 
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::string errorSdl(SDL_GetError());
        m_logger->log("SDL Initialization Error: " + errorSdl, ELogLevel::ERROR);
        exit(1);
    }

    // Crear la ventana: 64x32 escalado x10 = 640x320.
    // En SDL3 ya no se pasan las posiciones x, y ni SDL_WINDOW_SHOWN (se muestra por defecto)
    m_window = SDL_CreateWindow("CHIP-8", 64 * kZoom, 32 * kZoom, 0);
    if (!m_window)
    {
        std::string errorWindow(SDL_GetError());
        m_logger->log("Window creation Error: " + errorWindow, ELogLevel::ERROR);
        exit(1);
    }

    // Renderer: el "pincel" para dibujar (sustituye a la SDL_Surface del tutorial)
    m_renderer = SDL_CreateRenderer(m_window, nullptr);
    if (!m_renderer)
    {
        std::string errorRenderer(SDL_GetError());
        m_logger->log("Renderer creation Error: " + errorRenderer, ELogLevel::ERROR);
        exit(1);
    }
}

void TDisplaySDL::draw(uint8_t framebuffer[][64], uint16_t width, uint16_t height)
{
    // Fondo negro
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderClear(m_renderer);

    // Píxeles encendidos en verde
    SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);

    for (auto i = 0; i < height; i++)        // i = fila    (y)
    {
        for (auto j = 0; j < width; j++)     // j = columna (x)
        {
            if (framebuffer[i][j] == 1)
            {
                // Un rectángulo de kZoom x kZoom en la posición escalada.
                // En SDL3 los rectángulos de dibujo usan float (SDL_FRect)
                SDL_FRect pixel = {
                    static_cast<float>(j * kZoom),
                    static_cast<float>(i * kZoom),
                    static_cast<float>(kZoom),
                    static_cast<float>(kZoom)
                };
                SDL_RenderFillRect(m_renderer, &pixel);
            }
        }
    }
}

void TDisplaySDL::update()
{
    // Mostrar en la ventana lo que se ha dibujado en draw()
    SDL_RenderPresent(m_renderer);

    // Aquí NO se leen eventos: de eso se encarga TKeyboardSDL::update(),
    // que es el único sitio del programa que llama a SDL_PollEvent.
}

void TDisplaySDL::deinit()
{
    // Destruir en orden inverso a la creación
    if (m_renderer)
    {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }
    if (m_window)
    {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
    SDL_Quit();
}
