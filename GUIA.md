# Guía CHIP-8 en C++

> Esta guía te dice **qué** hacer y te da **pistas**, no el código.
> Consejo: ábrela con `Ctrl+Shift+V` en VS Code para verla formateada.

---

# PARTE 1 · CHEAT SHEET

## 1.1 Hexadecimal, bits y nibbles

| Concepto | Qué es | Ejemplo |
|---|---|---|
| bit | 0 o 1 | |
| nibble | 4 bits = 1 dígito hexadecimal | `0xA` = `1010` |
| byte | 8 bits = 2 dígitos hexadecimales | `0x3F` = `0011 1111` |
| word (en CHIP-8) | 16 bits = 4 dígitos hexadecimales | un opcode, por ejemplo `0xD123` |

Cada dígito hexadecimal de un opcode es un nibble. Por eso el CHIP-8 se lee tan bien en hexadecimal.

## 1.2 Operadores de bits (tu nuevo `SUBSTRING`)

| Operador | Nombre | Para qué lo usarás | Ejemplo con `v = 0xABCD` |
|---|---|---|---|
| `&` | AND | **Quedarte** con unos bits (máscara) | `v & 0x00FF` → `0x00CD` |
| `\|` | OR | **Juntar** o encender bits | `0xAB00 \| 0x00CD` → `0xABCD` |
| `^` | XOR | **Invertir** bits (¡así dibuja el CHIP-8!) | `1 ^ 1` → `0`, `0 ^ 1` → `1` |
| `~` | NOT | Invertir todos los bits | |
| `<<` | Desplazar a la izquierda | Mover bits "hacia arriba" (×2 por posición) | `0xAB << 8` → `0xAB00` |
| `>>` | Desplazar a la derecha | Mover bits "hacia abajo" (÷2 por posición) | `0xAB00 >> 8` → `0x00AB` |

**Patrón universal: máscara + desplazamiento.** Para sacar el 3.er dígito hexadecimal de `0xABCD` (la `C`):
primero lo **aíslas** con una máscara que solo tenga `F` en esa posición, y después lo **desplazas** a la derecha
tantos bits como posiciones tenga a su derecha (×4 bits por dígito).

**Comprobar un bit concreto:** `(valor & 0x80) != 0` → ¿está encendido el bit más alto de un byte?

⚠️ **Precedencia de operadores:** `==` se evalúa **antes** que `&`. Por tanto `a & 0x80 == 1` **no** hace lo que parece.
**Pon siempre paréntesis** con operadores de bits.

## 1.3 C++ que vas a necesitar

| Cosa | Para qué | Nota |
|---|---|---|
| `#include <cstdint>` | `uint8_t`, `uint16_t` | Tipos de tamaño exacto. Úsalos para todo el estado de la máquina. |
| `#include <array>` | `std::array<uint8_t, 4096>` | Como un array de C, pero sabe su tamaño (`.size()`) y se puede copiar. |
| `constexpr` | Constantes en tiempo de compilación | `constexpr uint16_t kInicioPrograma = 0x200;` |
| Inicializar miembros en el `.h` | `uint16_t pc = 0x200;` o `std::array<...> memoria{};` | Las llaves `{}` ponen todo a cero. |
| Método `const` | `uint8_t valor() const;` | Promete que no modifica el objeto. |
| Devolver una referencia `const` | `const std::array<...>& pantalla() const;` | Deja leer sin copiar y sin permitir modificar. |
| `static_cast<T>(x)` | Conversión explícita | Ejemplo: de `int` a `uint8_t`, dejando claro que es intencionado. |
| `std::ifstream` | Leer la ROM | Ábrelo con `std::ios::binary`. Busca `seekg`, `tellg` y `read`. |
| `reinterpret_cast<char*>` | `read()` pide `char*` y tu memoria es `uint8_t` | Es de los pocos casos en que es legítimo. |
| `<random>` | Para la instrucción `CXNN` | `std::mt19937` + `std::uniform_int_distribution`. **No uses `rand()`.** |
| `std::printf("%04X\n", op)` | Imprimir un opcode en hexadecimal | Imprescindible para depurar. |

**Trampa de la promoción de enteros:** `uint8_t a = 200, b = 100;` → `a + b` vale **300** (un `int`), no 44.
Eso es útil para detectar el acarreo en `8XY4`. Al guardar el resultado en un `uint8_t` se trunca solo (300 → 44), que es justo lo que quieres.

## 1.4 La máquina CHIP-8

| Componente | Tamaño / tipo | Detalle |
|---|---|---|
| Memoria | 4096 × `uint8_t` | `0x000–0x1FF`: reservada (ahí va la fuente) · `0x200–0xFFF`: programa |
| `V0`…`VF` | 16 × `uint8_t` | `VF` es la **bandera**: el acarreo, el préstamo o la colisión la sobrescriben |
| `I` | `uint16_t` | Registro índice (apunta a memoria) |
| `PC` | `uint16_t` | Contador de programa. **Empieza en `0x200`** |
| Pila | 16 × `uint16_t` + un índice `sp` | Para `2NNN` (call) y `00EE` (return) |
| `delay_timer` | `uint8_t` | Si es > 0, baja 1 a **60 Hz** |
| `sound_timer` | `uint8_t` | Igual. Mientras sea > 0 → suena el pitido |
| Pantalla | 64 × 32 píxeles on/off | Por ejemplo, `std::array<uint8_t, 64*32>` con índice `y*64 + x` |
| Teclado | 16 teclas on/off | `std::array<bool, 16>` o de `uint8_t` |

**Mapeo de teclado** (convención: el teclado del CHIP-8 sobre el lado izquierdo del teclado de PC):

```
CHIP-8        PC
1 2 3 C       1 2 3 4
4 5 6 D       Q W E R
7 8 9 E       A S D F
A 0 B F       Z X C V
```

## 1.5 Fuente (sprites del 0 al F)

Son datos estándar, no lógica, así que te los doy para que no te equivoques al copiarlos. Van en memoria a partir de `0x050`.
Cada carácter ocupa 5 bytes, así que el carácter `n` empieza en `0x050 + n*5` (te hará falta en `FX29`).

```
0xF0,0x90,0x90,0x90,0xF0, // 0
0x20,0x60,0x20,0x20,0x70, // 1
0xF0,0x10,0xF0,0x80,0xF0, // 2
0xF0,0x10,0xF0,0x10,0xF0, // 3
0x90,0x90,0xF0,0x10,0x10, // 4
0xF0,0x80,0xF0,0x10,0xF0, // 5
0xF0,0x80,0xF0,0x90,0xF0, // 6
0xF0,0x10,0x20,0x40,0x40, // 7
0xF0,0x90,0xF0,0x90,0xF0, // 8
0xF0,0x90,0xF0,0x10,0xF0, // 9
0xF0,0x90,0xF0,0x90,0x90, // A
0xE0,0x90,0xE0,0x90,0xE0, // B
0xF0,0x80,0x80,0x80,0xF0, // C
0xE0,0x90,0x90,0x90,0xE0, // D
0xF0,0x80,0xF0,0x80,0xF0, // E
0xF0,0x80,0xF0,0x80,0x80  // F
```

Pruébalo: escribe `0xF0,0x90,0x90,0x90,0xF0` en binario, una fila debajo de otra. ¿Qué ves?

## 1.6 Las piezas de un opcode

Con el opcode `0xD123` como ejemplo:

| Nombre | Qué es | Valor en `0xD123` |
|---|---|---|
| (tipo) | 1.er nibble → decide el `case` del `switch` | `D` |
| `X` | 2.º nibble → índice de registro `VX` | `1` |
| `Y` | 3.er nibble → índice de registro `VY` | `2` |
| `N` | 4.º nibble → número de 4 bits | `3` |
| `NN` | Último byte → número de 8 bits | `0x23` |
| `NNN` | Últimos 12 bits → dirección de memoria | `0x123` |

## 1.7 Tabla de instrucciones

| Opcode | Qué hace | Pista / trampa |
|---|---|---|
| `00E0` | Borrar la pantalla | |
| `00EE` | Volver de la subrutina | Saca de la pila → `PC` |
| `1NNN` | Saltar a `NNN` | |
| `2NNN` | Llamar a la subrutina en `NNN` | Mete el `PC` en la pila **y después** salta |
| `3XNN` | Saltarse la siguiente instrucción si `VX == NN` | "Saltarse" = `PC += 2` |
| `4XNN` | Saltarse si `VX != NN` | |
| `5XY0` | Saltarse si `VX == VY` | |
| `6XNN` | `VX = NN` | |
| `7XNN` | `VX += NN` | **No** toca `VF` |
| `8XY0` | `VX = VY` | |
| `8XY1` | `VX \|= VY` | Quirk: el CHIP-8 original pone `VF = 0` |
| `8XY2` | `VX &= VY` | Ídem |
| `8XY3` | `VX ^= VY` | Ídem |
| `8XY4` | `VX += VY`, `VF` = acarreo | Calcula la bandera **antes** de escribir, y escribe `VF` **después** de `VX` |
| `8XY5` | `VX = VX - VY`, `VF` = 1 si **no** hay préstamo | Ídem |
| `8XY6` | Desplazar a la derecha 1 bit, `VF` = bit que sale | Quirk: ¿se desplaza `VX` o `VY`? |
| `8XY7` | `VX = VY - VX`, `VF` = 1 si no hay préstamo | |
| `8XYE` | Desplazar a la izquierda 1 bit, `VF` = bit que sale | Mismo quirk que `8XY6`. ¡Cuidado con los paréntesis! |
| `9XY0` | Saltarse si `VX != VY` | |
| `ANNN` | `I = NNN` | |
| `BNNN` | Saltar a `NNN + V0` | Quirk: en SUPER-CHIP es `XNN + VX` |
| `CXNN` | `VX = aleatorio & NN` | |
| `DXYN` | Dibujar un sprite de N filas desde `I` en (`VX`, `VY`) | La más difícil, explicada abajo |
| `EX9E` | Saltarse si la tecla `VX` está pulsada | |
| `EXA1` | Saltarse si la tecla `VX` **no** está pulsada | |
| `FX07` | `VX = delay_timer` | |
| `FX0A` | Esperar una tecla y guardarla en `VX` | Pista: "esperar" = no avanzar el `PC` (`PC -= 2`) |
| `FX15` | `delay_timer = VX` | |
| `FX18` | `sound_timer = VX` | |
| `FX1E` | `I += VX` | |
| `FX29` | `I` = dirección del carácter `VX` de la fuente | `0x050 + (VX & 0xF) * 5` |
| `FX33` | Guardar en BCD: centenas en `I`, decenas en `I+1`, unidades en `I+2` | `/` y `%` son tus amigos |
| `FX55` | Guardar `V0..VX` en memoria a partir de `I` | Quirk: el original incrementa `I` |
| `FX65` | Cargar `V0..VX` desde memoria a partir de `I` | Ídem |

**`DXYN` paso a paso:**
1. Coordenada inicial: `x = VX % 64`, `y = VY % 32` (la coordenada inicial **se envuelve**).
2. `VF = 0`.
3. Para cada fila `f` de `0` a `N-1`: el byte del sprite es `memoria[I + f]`.
4. Para cada bit `b` de `0` a `7` de ese byte, del más alto al más bajo: si está a 1, haz XOR del píxel (`x+b`, `y+f`).
   Si ese píxel estaba encendido y se apaga → `VF = 1`.
5. Si `x+b >= 64` o `y+f >= 32`, ese píxel **se recorta** (no se dibuja y no se envuelve).

---

# PARTE 2 · PASO A PASO: `Chip8.h` y `Chip8.cpp`

**Regla de oro:** `Chip8` **no** incluye SDL. Solo conoce bytes. `main.cpp` y `Platform` son quienes hablan con SDL.

## Paso 1 · `Chip8.h`: el "DDL" de tu máquina

Escribe la clase `Chip8` con:

- [ ] `#pragma once` y los includes que necesites (`<cstdint>`, `<array>`, `<string>`).
- [ ] Unas constantes `constexpr`: el ancho y el alto de pantalla, la dirección de inicio del programa y la de la fuente.
- [ ] **Una parte pública** (lo que verá `main.cpp`). Piensa qué necesita `main` de la máquina:
  - construirla,
  - cargarle una ROM (¿y si falla? → que devuelva `bool`),
  - ejecutar **una** instrucción,
  - bajar los temporizadores un tic,
  - leer la pantalla para pintarla (pista: una referencia `const`),
  - decirle qué teclas están pulsadas,
  - saber si tiene que sonar.
- [ ] **Una parte privada** con todo el estado de la tabla 1.4, inicializado en la propia declaración (`{}` o un valor).
- [ ] Opcional: métodos privados de ayuda, por ejemplo uno por cada grupo de opcodes (`ejecutar8(...)`, `ejecutarF(...)`).

✅ **Control:** compila (F7) aunque `Chip8.cpp` todavía no implemente nada, siempre que nadie llame a esos métodos.

## Paso 2 · Constructor

- [ ] Copia la fuente a memoria a partir de `0x050`. Pista: guárdala en un `constexpr std::array` y cópiala con un bucle o con `std::copy`.
- [ ] Si has inicializado los miembros en el `.h`, no tienes que hacer nada más. Si no, pon `PC` a `0x200` y el resto a cero.

## Paso 3 · `cargarRom(ruta)`

- [ ] Abre el fichero en modo binario. Si no se abre → `return false`.
- [ ] Averigua su tamaño. Pista: `std::ios::ate` abre el fichero con el cursor al final, y `tellg()` te dice en qué posición está.
- [ ] **Comprueba que cabe:** el tamaño tiene que ser `<= 4096 - 0x200`. Si no cabe → `false`.
- [ ] Vuelve al principio y lee los datos directamente en `memoria.data() + 0x200`.

✅ **Control:** carga la ROM del logo de IBM e imprime en hexadecimal los primeros 4 bytes desde `0x200`. En la ROM clásica `IBM Logo.ch8` son `00 E0 A2 2A`. Si usas otra versión, compáralos con la extensión "Hex Editor" de VS Code abriendo el `.ch8`.

## Paso 4 · `ciclo()`: fetch → decode → execute

- [ ] **Fetch:** junta `memoria[pc]` y `memoria[pc+1]` en un `uint16_t` (big-endian: el primero es el byte alto).
- [ ] **Avanza `pc += 2` justo después del fetch**, antes de ejecutar. Así los saltos solo tienen que sobrescribir el `PC`.
- [ ] **Decode:** saca `X`, `Y`, `N`, `NN` y `NNN` en variables locales.
- [ ] **Execute:** haz un `switch` sobre el 1.er nibble. Dentro de los grupos `0`, `8`, `E` y `F`, añade otro `switch` (sobre `N` o sobre `NN`).
- [ ] Pon un `default:` que imprima `"Opcode desconocido: %04X"`. Te va a salvar muchas veces.

## Paso 5 · 🎯 Hito: el logo de IBM

- [ ] Implementa **solo** `00E0`, `1NNN`, `6XNN`, `7XNN`, `ANNN` y `DXYN`.
- [ ] En `main.cpp`: crea un `Chip8`, carga la ROM y, en el bucle, ejecuta unos cuantos `ciclo()` por frame.
      Después pinta la pantalla: un rectángulo de 10×10 por cada píxel encendido.
      Pistas de SDL3: `SDL_FRect` (en SDL3 los rectángulos son `float`) y `SDL_RenderFillRect`.
- [ ] Pasa la ruta de la ROM por línea de comandos (`argv[1]`). En VS Code se configura en `.vscode/settings.json` con `"cmake.debugConfig": { "args": ["roms/ibm.ch8"] }`.

✅ **Control:** sale el logo de IBM en pantalla. **Si lo consigues, lo difícil ya está hecho.**

## Paso 6 · El resto, guiado por los tests

Descarga las ROMs de **Timendus/chip8-test-suite** y ve en este orden:

1. [ ] `2-ibm-logo.ch8` (ya lo tienes)
2. [ ] `3-corax+.ch8`: saltos condicionales, `8XY*`, `FX55`/`FX65`/`FX33`, `2NNN`/`00EE`
3. [ ] `4-flags.ch8`: el orden de `VF` en la aritmética
4. [ ] `5-quirks.ch8`: elige "CHIP-8" en el menú y ajusta los quirks hasta que salga todo en verde
5. [ ] `6-keypad.ch8`: `EX9E`, `EXA1`, `FX0A` (necesitas el teclado del paso 8)

Cada test pinta en pantalla qué instrucción falla. Lee su README.

## Paso 7 · Tiempo

- [ ] El bucle de `main` va a 60 frames por segundo. En cada frame:
  1. procesa los eventos,
  2. ejecuta unos **10–12 ciclos** (unas 700 instrucciones por segundo; que sea configurable),
  3. baja los temporizadores **una vez**,
  4. pinta.
- [ ] Mejora: en lugar de `SDL_Delay(16)` fijo, mide el tiempo transcurrido con `SDL_GetTicksNS()`.

## Paso 8 · Teclado y sonido

- [ ] Gestiona `SDL_EVENT_KEY_DOWN` y `SDL_EVENT_KEY_UP`: traduce la tecla de PC → tecla CHIP-8 (0–F) y pásasela a `Chip8`.
- [ ] Sonido: mientras `sound_timer > 0`, un pitido. En SDL3 busca `SDL_OpenAudioDeviceStream`. Déjalo para el final.

## Paso 9 · A jugar

Pong, Tetris, Space Invaders... Busca ROMs de dominio público (por ejemplo, el archivo de John Earnest, "chip8Archive").

---

# PARTE 3 · Sobre la guía de tmvtech

Te puede servir para ver una **estructura** posible, pero ten en cuenta que:

**Usa SDL2, Linux y un Makefile.** Tú usas SDL3, Windows y CMake. Equivalencias:

| SDL2 (tmvtech) | SDL3 (tú) |
|---|---|
| `#include <SDL2/SDL.h>` | `#include <SDL3/SDL.h>` |
| `SDL_Init(...)` devuelve `0` si va bien | Devuelve `true` si va bien |
| `SDL_CreateWindow(titulo, x, y, w, h, flags)` | `SDL_CreateWindow(titulo, w, h, flags)` (sin x, y) |
| `SDL_CreateRenderer(win, -1, flags)` | `SDL_CreateRenderer(win, nullptr)` |
| `SDL_QUIT`, `SDL_KEYDOWN`, `SDL_KEYUP` | `SDL_EVENT_QUIT`, `SDL_EVENT_KEY_DOWN`, `SDL_EVENT_KEY_UP` |
| `e.key.keysym.sym` | `e.key.key` |
| `SDL_Rect` para pintar | `SDL_FRect` (con `float`) |
| `SDL_OpenAudio` + callback | `SDL_OpenAudioDeviceStream` |

**Y tiene bugs que no debes copiar:**

1. **`8XYE`:** `if (reg & 0x80 == 1)` → por la precedencia (ver 1.2) siempre da falso. *Ejercicio: ¿por qué es así? ¿Y por qué, aun con paréntesis, `== 1` seguiría estando mal?*
2. **`FX33`:** escribe las unidades en `I+1` en vez de en `I+2` y machaca las decenas.
3. **`DXYN`:** ni envuelve ni recorta las coordenadas, así que puede escribir **fuera del array** de la pantalla. En C++ eso no da un error claro: corrompe memoria en silencio.
4. **Temporizadores:** dice que van a 60 Hz, pero los ata a 20 iteraciones de ~1 ms, es decir, unos 50 Hz como mucho.
5. **No comprueba el tamaño de la ROM** antes de copiarla a memoria.
6. Usa `new` sin `delete`. En C++ moderno, miembros por valor o `std::unique_ptr`.
7. Por defecto imprime una línea por cada instrucción, lo que hace el emulador lentísimo.

Además separa la CPU de la máquina en clases distintas (`TCpu` + `TChip8` con `friend`). Es un diseño válido: si sigues
esta guía, mantén su estructura en lugar de mezclarla con la Parte 2.

## 3.1 Cómo adaptar tmvtech a tu proyecto, paso a paso

**Regla general:** cada `.cpp` nuevo que crees **hay que añadirlo a `add_executable` en `CMakeLists.txt`**.
Si no lo haces, te saldrá `LNK2019` (ver la lección sobre `.h` y `.cpp`).

| Paso tmvtech | Qué crea | Qué adaptar |
|---|---|---|
| 3 · Setup | `Makefile`, `main.cpp` | **Sáltatelo.** Ya tienes CMake y la ventana SDL3. |
| 4 · Logger | `logger.h/.cpp` | Opcional. Si lo haces, que el nivel por defecto **no** sea DEBUG (imprimir cada instrucción lo hace lentísimo). Un singleton con `shared_ptr` es excesivo: vale con funciones libres o con `std::printf`. |
| 5 · Línea de comandos | `cmdLineParser.h/.cpp` | Opcional. Para empezar, `argv[1]` = ruta de la ROM. Si lo haces, comprueba `i + 1 < argc` antes de leer el argumento siguiente (el suyo se sale del array). |
| 7 · `TChip8` | `chip8.h/.cpp` | **← Estás aquí.** `constexpr` en vez de `#define`, `#pragma once`, la fuente en el `.cpp` y llaves en los `for`. |
| 8 · ROM loader | `romLoader.h/.cpp` | Que **devuelva `bool`** en vez de cerrar el programa, y que **compruebe el tamaño** (`<= 4096 - 0x200`). |
| 9 · `TCpu` | `cpu.h/.cpp` | Bien tal cual. `SP` puede ser `uint8_t`. Comprueba que no se desborda la pila (16 niveles). |
| 10 · Instrucciones | `cpu.cpp` | **Usa la tabla 1.7 de esta guía, no su código.** Corrige `8XYE` (paréntesis), `FX33` (`I+2`) y `DXYN` (envolver la coordenada inicial y recortar). Prueba con las ROMs de Timendus. |
| 11 · Display | `display.h`, `displaysdl.h/.cpp` | Pásalo a SDL3 (tabla de arriba). Usa el **renderer** (`SDL_RenderFillRect` con `SDL_FRect`), no `SDL_GetWindowSurface`. Ventana de 640×320, no de 1280×720. **Importante:** su `update()` hace `SDL_PollEvent` y descarta los eventos, con lo que **se come las teclas** del paso 13. Los eventos se procesan en **un solo sitio**. |
| 12 · Timing | `chip8.cpp` (`run`) | Sus "20 iteraciones = 60 Hz" son ~50 Hz. Usa el esquema del paso 7 de la Parte 2: por frame de 60 Hz, ~11 ciclos y un tic de los temporizadores, midiendo el tiempo con `SDL_GetTicksNS()`. |
| 13 · Teclado | `keyboard.h`, `keyboardsdl.h/.cpp` | Pásalo a SDL3: `SDL_EVENT_KEY_DOWN/UP` y `e.key.key`. Aquí es donde se leen los eventos, también el de cerrar la ventana. |
| 14 · Sonido | `sound.h`, `soundsdl.h/.cpp` | La API de audio cambió mucho en SDL3: busca `SDL_OpenAudioDeviceStream`. Su `deinit` llama a `SDL_Quit()` y cierra **todo** SDL; quita eso de ahí. Déjalo para el final. |
| 15 · Montaje | `main.cpp` | Crea los objetos SDL **por valor** o con `std::unique_ptr`, y pásalos con `setDisplay(&display)` y similares. Sin `new` ni `delete` sueltos. |
