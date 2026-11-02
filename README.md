# Emulador CHIP-8 en C++

Emulador de [CHIP-8](https://es.wikipedia.org/wiki/CHIP-8) escrito en C++ con SDL3.

Es un **proyecto personal de aprendizaje**: mi primer proyecto serio en C++, viniendo del mundo
de los datos (SQL, Python, Java). Lo usé para aprender cómo funciona un ordenador por dentro
(memoria, registros, ciclo fetch-decode-execute) y los fundamentos del lenguaje: operaciones con
bits, punteros y referencias, clases, herencia e interfaces, y gestión de memoria.

## Cómo lo he hecho

- **Tutorial:** he seguido la estructura del tutorial de TMVTech
  **[CHIP-8 Emulator in C++](https://www.tmvtech.com/chip8-emulator-in-cpp/)**, escribiendo y
  entendiendo el código paso a paso.
- **Apoyo de IA:** he usado una IA como profesor: para resolver dudas de C++, revisar mi código,
  depurar los fallos y montar el entorno (CMake, SDL3 y GitHub Actions).

El tutorial está pensado para Linux con SDL2, así que lo he adaptado a:

| Tutorial | Este proyecto |
|---|---|
| Linux (Ubuntu) | Windows |
| Makefile | CMake (descarga SDL3 automáticamente) |
| SDL2 | SDL3 |

Además se corrigieron varios fallos del código del tutorial:

- **`8XYE`**: la comprobación del bit alto (`& 0x80 == 1`) siempre daba falso por la precedencia de operadores.
- **`FX33`**: escribía las unidades en `I+1` en vez de `I+2`.
- **`DXYN`**: no envolvía ni recortaba las coordenadas y podía escribir fuera de la pantalla, corrompiendo la memoria (la fuente).
- **`8XY4`–`8XYE`**: la bandera `VF` se escribe después del resultado y `8XY5`/`8XY7` usan `>=`.
- **Teclado y pantalla**: el display leía y descartaba los eventos de SDL, así que el teclado nunca recibía las pulsaciones.

## Compilar

Requisitos: Visual Studio Build Tools (MSVC), CMake 3.24+ y Git.

```bash
cmake -S . -B build
cmake --build build --config Debug
```

La primera configuración descarga y compila SDL3, así que tarda unos minutos.

## Ejecutar

```bash
.\build\Debug\chip8.exe -r roms\2-ibm-logo.ch8 -l 1
```

| Opción | Significado |
|---|---|
| `-r <ruta>` | ROM a cargar |
| `-l <nivel>` | Nivel de log: 0 nada, 1 errores, 2 avisos, 3 info, 4 depuración |

Las ROMs deben ser de **CHIP-8 clásico** (máximo 3584 bytes). Las de SUPER-CHIP o XO-CHIP no funcionan.

## Controles

El teclado hexadecimal del CHIP-8 se mapea en la parte izquierda del teclado:

```
CHIP-8        PC
1 2 3 C       1 2 3 4
4 5 6 D       Q W E R
7 8 9 E       A S D F
A 0 B F       Z X C V
```

`ESC` o la X de la ventana cierran el emulador.

## Estructura

| Archivo | Qué hace |
|---|---|
| `Chip8` | La máquina: memoria, pantalla, pila, temporizadores y el bucle principal |
| `cpu` | Los registros y las 35 instrucciones (fetch, decode, execute) |
| `romLoader` | Carga la ROM en memoria a partir de `0x200` |
| `display.h` / `displaysdl` | Interfaz de pantalla y su implementación con SDL3 |
| `keyboard.h` / `keyboardsdl` | Interfaz de teclado y su implementación con SDL3 |
| `logger`, `cmdLineParser` | Mensajes de depuración y argumentos de la línea de comandos |

## Estado

- ✅ Las 35 instrucciones de CHIP-8, pantalla y teclado.
- ✅ Probado con el logo de IBM y Super Pong.
- ⏳ Sin sonido.
- ⏳ El control de velocidad del bucle principal está pendiente de ajustar.

El repositorio tiene un *quality gate* en GitHub Actions que compila el proyecto y pasa
`cppcheck` en cada push a `desarrollo` y en cada pull request a `calidad` o `master`.
