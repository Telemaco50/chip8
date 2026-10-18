// Fase 0: abrir una ventana negra con SDL3 y cerrarla con la X o con ESC.
// Todavía no hay emulador: solo comprobamos que el entorno funciona.

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>  // en Windows adapta main() al punto de entrada de SDL

// Pantalla del CHIP-8: 64x32 píxeles. La escalamos para que se vea en un monitor moderno.
constexpr int kAnchoChip8 = 64;
constexpr int kAltoChip8  = 32;
constexpr int kEscala     = 10;

int main(int argc, char* argv[])
{
    // 1. Inicializar SDL (solo el subsistema de vídeo de momento).
    //    En SDL3 devuelve bool: true = OK.
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Error en SDL_Init: %s", SDL_GetError());
        return 1;
    }

    // 2. Crear la ventana.
    SDL_Window* ventana = SDL_CreateWindow("CHIP-8",
                                           kAnchoChip8 * kEscala,
                                           kAltoChip8 * kEscala,
                                           0);
    if (ventana == nullptr) {
        SDL_Log("Error en SDL_CreateWindow: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // 3. Crear el renderer: es el "pincel" con el que pintamos en la ventana.
    //    nullptr = que SDL elija el mejor driver (Direct3D, OpenGL...).
    SDL_Renderer* renderer = SDL_CreateRenderer(ventana, nullptr);
    if (renderer == nullptr) {
        SDL_Log("Error en SDL_CreateRenderer: %s", SDL_GetError());
        SDL_DestroyWindow(ventana);
        SDL_Quit();
        return 1;
    }

    // 4. Bucle principal (el "Tick" de Unreal).
    bool ejecutando = true;
    while (ejecutando) {
        // 4a. Vaciar la cola de eventos: teclado, ratón, cerrar ventana...
        SDL_Event evento;
        while (SDL_PollEvent(&evento)) {
            if (evento.type == SDL_EVENT_QUIT) {
                ejecutando = false;
            }
            if (evento.type == SDL_EVENT_KEY_DOWN && evento.key.key == SDLK_ESCAPE) {
                ejecutando = false;
            }
        }

        // 4b. Pintar: fondo negro y presentar en pantalla.
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // R, G, B, Alpha
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);

        // 4c. Ceder la CPU ~16 ms (≈60 fps) para no poner un núcleo al 100%.
        //     Más adelante esto lo sustituirás por un control de tiempo de verdad.
        SDL_Delay(16);
    }

    // 5. Limpiar en orden inverso a la creación.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(ventana);
    SDL_Quit();
    return 0;
}
