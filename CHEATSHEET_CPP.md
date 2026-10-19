# Cheat sheet de C++

> Pensada para alguien que viene de Python, Java, JS y SQL, con ejemplos de lo que aparece en el tutorial.
> Ábrela con `Ctrl+Shift+V` en VS Code.

---

## 1. De tus lenguajes a C++

| Concepto | Python | Java | C++ |
|---|---|---|---|
| Variable | `x = 5` | `int x = 5;` | `int x = 5;` o `auto x = 5;` |
| Constante | `MAX = 10` (convención) | `final int MAX = 10;` | `constexpr int kMax = 10;` |
| Nulo | `None` | `null` | `nullptr` (solo para punteros) |
| Texto | `str` | `String` | `std::string` (`#include <string>`) |
| Lista dinámica | `list` | `ArrayList` | `std::vector<T>` (`#include <vector>`) |
| Array fijo | — | `int[]` | `std::array<T, N>` (`#include <array>`) |
| Diccionario | `dict` | `HashMap` | `std::unordered_map<K, V>` |
| Imprimir | `print(x)` | `System.out.println(x)` | `std::cout << x << '\n';` o `std::printf("%d\n", x);` |
| Importar | `import x` | `import x;` | `#include "x.h"` (copia y pega el archivo) |
| Bloques | Sangría | `{ }` | `{ }`. **La sangría no significa nada.** |
| Memoria | Recolector de basura | Recolector de basura | **Tú decides**: pila, `unique_ptr` o `shared_ptr` |

---

## 2. Tipos

```cpp
#include <cstdint>

uint8_t  a = 255;     // 0..255           (1 byte)  → registros V0-VF, memoria
uint16_t b = 0xFFFF;  // 0..65535         (2 bytes) → PC, I, opcodes
uint32_t c;           // 0..4.294.967.295 (4 bytes)
int      d = -5;      // con signo, normalmente 4 bytes
bool     e = true;
char     f = 'A';     // 1 carácter (comillas simples)
float    g = 1.5f;    // decimal (con f al final)
double   h = 1.5;     // decimal de doble precisión
auto     i = 42;      // el compilador deduce el tipo (int)
```

- `0x1F` es hexadecimal y `0b0001'1111` es binario. Las comillas `'` separan dígitos para que se lean mejor.
- **Desbordamiento:** `uint8_t a = 255; a++;` → `a` vale `0`. En el CHIP-8 eso es lo que queremos.
- **Promoción:** `uint8_t + uint8_t` da un `int`. Si lo guardas en un `uint8_t`, se trunca.
- **Conversión explícita:** `static_cast<uint8_t>(valor)`.

---

## 3. Constantes y macros

```cpp
#define MAX 16              // ❌ Estilo C: sustitución de texto, sin tipo
constexpr int kMax = 16;    // ✅ C++: con tipo y ámbito; se calcula al compilar
const int y = calcular();   // ✅ No cambia, pero se calcula en ejecución
```

---

## 4. Funciones

```cpp
int sumar(int a, int b) { return a + b; }        // por valor: copia los argumentos

void reset(int& x) { x = 0; }                    // por referencia: modifica el original
void mostrar(const std::string& s) { ... }       // por referencia const: no copia y no modifica

bool cargarRom(const std::string& ruta);          // declaración (va en el .h)
```

**Regla rápida para los parámetros:**
- Tipos pequeños (`int`, `uint8_t`, `bool`) → **por valor**.
- Objetos (`std::string`, `std::vector`...) que solo vas a leer → **`const T&`**.
- Objetos que quieres modificar → **`T&`**.

---

## 5. Punteros y referencias (lo que más asusta)

```cpp
int x = 10;

int& ref = x;     // referencia = OTRO NOMBRE para x. No puede ser nula ni cambiar de destino.
ref = 20;         // ahora x == 20

int* ptr = &x;    // puntero = variable que guarda la DIRECCIÓN de x
*ptr = 30;        // * "desreferencia": accede a lo apuntado → x == 30
ptr = nullptr;    // un puntero puede no apuntar a nada
```

| Símbolo | En una declaración | En una expresión |
|---|---|---|
| `&` | `int& r` → es una referencia | `&x` → la dirección de x |
| `*` | `int* p` → es un puntero | `*p` → el valor apuntado |
| `->` | — | `p->metodo()` = `(*p).metodo()` |

**Cuándo usar cada uno:** referencias siempre que puedas, y punteros cuando algo pueda ser "nada" (`nullptr`) o cambiar de destino.

---

## 6. Memoria: pila, heap y smart pointers

```cpp
TChip8 emu;                                    // ✅ en la PILA: se destruye solo al salir del bloque { }

TCpu* cpu = new TCpu();                        // ⚠️ en el HEAP: TÚ tienes que hacer delete
delete cpu;                                    //    si se te olvida → fuga de memoria

auto cpu = std::make_unique<TCpu>();           // ✅ unique_ptr: un único dueño; se borra solo
auto log = std::make_shared<TLogger>();        // ✅ shared_ptr: varios dueños; se borra al soltarlo el último
```

`#include <memory>` para `unique_ptr` y `shared_ptr`.

**Regla del C++ moderno:** si escribes `new` o `delete`, para y piensa si no vale con una variable normal o con un `unique_ptr`.

**RAII** (idea clave de C++): el constructor adquiere los recursos y el destructor los libera. Si el objeto muere, sus recursos se liberan solos.

---

## 7. Clases

```cpp
// --- Persona.h ---
#pragma once
#include <string>

class Persona {
public:                                         // accesible desde fuera
    Persona(const std::string& nombre, int edad);   // constructor
    ~Persona();                                 // destructor (casi nunca hace falta)

    void cumplir();                             // modifica el objeto
    int edad() const;                           // const = no modifica el objeto

private:                                        // solo accesible desde dentro
    std::string m_nombre;                       // m_ = convención para "miembro"
    int m_edad = 0;                             // valor inicial por defecto
};

// --- Persona.cpp ---
#include "Persona.h"

Persona::Persona(const std::string& nombre, int edad)
    : m_nombre(nombre), m_edad(edad)            // lista de inicialización (preferible a asignar en el cuerpo)
{}

void Persona::cumplir() { m_edad++; }
int Persona::edad() const { return m_edad; }
```

- `Clase::metodo` → "el método `metodo` de la clase `Clase`" (se usa en el `.cpp`).
- `class` es privado por defecto y `struct` es público por defecto. Por lo demás, son iguales.
- `friend class X;` → la clase X puede tocar las partes `private`. Úsalo con moderación.
- `class X;` sin cuerpo es una **declaración adelantada**: "existe una clase X". Sirve para usar `X*` sin incluir su `.h`.

---

## 8. Herencia e interfaces (patrón del tutorial: `TDisplay` → `TDisplaySDL`)

```cpp
// Interfaz (clase abstracta) = la "interface" de Java
class TDisplay {
public:
    virtual ~TDisplay() = default;              // ⚠️ obligatorio en una clase base con virtuales
    virtual void draw() = 0;                    // = 0 → "virtual puro": las hijas DEBEN implementarlo
};

// Implementación concreta
class TDisplaySDL : public TDisplay {
public:
    void draw() override;                       // override → el compilador comprueba que existe en la base
};

// Uso polimórfico
TDisplaySDL pantalla;
TDisplay* d = &pantalla;
d->draw();                                      // llama a TDisplaySDL::draw()
```

| Java | C++ |
|---|---|
| `interface X { void f(); }` | `class X { public: virtual void f() = 0; };` |
| `class A implements X` | `class A : public X` |
| `@Override` | `override` |

---

## 9. Control de flujo

```cpp
if (a > b) { ... } else if (a == b) { ... } else { ... }

for (int i = 0; i < 16; i++) { ... }            // clásico
for (auto& valor : miArray) { valor = 0; }      // range-for (como el "for x in" de Python); & para modificar

while (ejecutando) { ... }

switch (opcode & 0xF000) {                      // como un CASE WHEN de SQL
    case 0x1000: saltar(); break;               // ⚠️ sin break, "se cae" al siguiente case
    case 0x6000: cargar(); break;
    default:     error();  break;
}
```

⚠️ **Pon siempre llaves**, aunque haya una sola línea. Sin ellas, el `for` o el `if` solo abarca la siguiente sentencia, y la sangría no cuenta.

---

## 10. Ámbito y "sombra" (shadowing)

```cpp
int x = 1;
if (true) {
    int x = 2;      // ⚠️ es OTRA variable que "tapa" a la de fuera
    x = 3;          // modifica la de dentro
}
// aquí x sigue valiendo 1
```

El compilador lo permite, pero suele ser un despiste. Si quieres usar la de fuera, no la vuelvas a declarar.

---

## 11. Enums

```cpp
enum class ELogLevel { NONE, ERROR, WARN, INFO, DEBUG };   // ✅ con ámbito: ELogLevel::ERROR
ELogLevel nivel = ELogLevel::INFO;

enum Color { ROJO, VERDE };                                 // ❌ estilo antiguo: los nombres "se escapan" fuera
```

---

## 12. `static` (tiene tres significados según dónde esté)

```cpp
class TLogger {
    static std::shared_ptr<TLogger> s_instancia;   // 1. Miembro compartido por TODA la clase, no por cada objeto
public:
    static std::shared_ptr<TLogger> getInstance(); // 2. Método que se llama sin objeto: TLogger::getInstance()
};

static int contador = 0;   // 3. A nivel de archivo: solo visible dentro de este .cpp
```

---

## 13. Ficheros y tiempo

```cpp
#include <fstream>
std::ifstream f(ruta, std::ios::binary | std::ios::ate);   // ate = abre con el cursor al final
if (!f) { /* no se pudo abrir */ }
auto tamano = f.tellg();                                   // posición = tamaño del fichero
f.seekg(0);                                                // vuelve al principio
f.read(reinterpret_cast<char*>(destino), tamano);

#include <chrono>
#include <thread>
auto t0 = std::chrono::steady_clock::now();
std::this_thread::sleep_for(std::chrono::milliseconds(1));
auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
              std::chrono::steady_clock::now() - t0).count();
```

---

## 14. Formatos de `printf`

| Formato | Para qué | Ejemplo |
|---|---|---|
| `%d` | Entero | `printf("%d", 42)` |
| `%u` | Entero sin signo | |
| `%X` / `%04X` | Hexadecimal (rellenado a 4 dígitos) | `printf("%04X", 0xD12)` → `0D12` |
| `%s` | Texto C (`const char*`) | `printf("%s", str.c_str())` |
| `%f` | Decimal | |

---

## 15. Errores típicos en MSVC (en español)

| Mensaje | Quién | Causa habitual |
|---|---|---|
| `identificador no declarado` / `C2065` | Compilador | Falta un `#include`, o hay una errata en el nombre |
| `no es miembro de` / `C2039` | Compilador | El método no está declarado en el `.h`, o le falta `std::` |
| `falta ';'` / `C2143` | Compilador | Falta un `;`, **a menudo al final de una `class { ... };`** |
| `no se puede convertir de A a B` / `C2440` | Compilador | Tipos incompatibles. ¿Falta `*`, `&` o un `static_cast`? |
| `no se puede crear una instancia de una clase abstracta` / `C2259` | Compilador | No has implementado algún método `= 0` |
| `símbolo externo sin resolver` / `LNK2019` | Linker | El `.cpp` no está en `add_executable`, o no coinciden la firma o el nombre |
| `ya está definido` / `LNK2005` | Linker | Has definido una función en un `.h` incluido desde varios `.cpp` |
| `No se puede abrir el archivo de inclusión` / `C1083` | Compilador | Ruta o nombre del `.h` mal escrito (¡ojo con mayúsculas y minúsculas!) |

**Consejo:** lee **siempre el primer error** de la lista. Los siguientes suelen ser consecuencia del primero.

---

## 16. Depurar en VS Code

| Acción | Atajo |
|---|---|
| Compilar | `F7` |
| Ejecutar con el depurador | `Ctrl+F5` (CMake: Debug) |
| Ejecutar sin el depurador | `Shift+F5` |
| Poner un breakpoint | Clic a la izquierda del número de línea, o `F9` |
| Siguiente línea (sin entrar en las funciones) | `F10` |
| Entrar en la función | `F11` |
| Continuar hasta el siguiente breakpoint | `F5` |

Con el programa parado en un breakpoint puedes ver el valor de cada variable en el panel izquierdo.
Para ver un número en hexadecimal, en el panel *Inspección* escribe `variable,x`.
