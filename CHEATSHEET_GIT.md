# Cheat sheet de Git y GitHub

> Repositorio: https://github.com/Telemaco50/chip8 (privado)
> Ábrela con `Ctrl+Shift+V` en VS Code.

---

## 1. Conceptos en 30 segundos

| Concepto | Qué es | Analogía con datos |
|---|---|---|
| **Repositorio** | La carpeta del proyecto + todo su historial | Una base de datos con el histórico completo |
| **Commit** | Una "foto" del proyecto en un momento dado, con un mensaje | Un `COMMIT` de una transacción |
| **Rama (branch)** | Una línea de trabajo independiente | Un entorno: DEV / QA / PROD |
| **Merge** | Llevar los cambios de una rama a otra | Promocionar de DEV a QA |
| **Remoto (`origin`)** | La copia del repositorio en GitHub | El servidor central |
| **Push** | Subir tus commits a GitHub | |
| **Pull** | Bajar de GitHub los commits que no tienes | |

**Las tres zonas de Git:**
```
Carpeta de trabajo  ──git add──►  Staging (preparado)  ──git commit──►  Historial local  ──git push──►  GitHub
```

---

## 2. Tus ramas

```
desarrollo  ──►  calidad  ──►  master
  (DEV)            (QA)        (PROD)
```

| Rama | Para qué | Qué entra |
|---|---|---|
| `desarrollo` | Trabajo diario | Todo, aunque esté a medias |
| `calidad` | Probar | Lo que compila y crees que funciona → pásale las ROMs de test |
| `master` | Versión estable | Solo lo que ha pasado las pruebas en `calidad` |

**Regla:** trabaja **siempre** en `desarrollo`. Nunca hagas commits directamente en `calidad` ni en `master`: a esas ramas solo se llega con un merge.

---

## 3. El día a día (el 90% de lo que harás)

```bash
git status                       # ¿en qué rama estoy? ¿qué ha cambiado?
git switch desarrollo            # cambiar a la rama desarrollo
git add .                        # preparar TODOS los cambios
git add src/cpu.cpp              # ...o solo un archivo
git commit -m "Implementar opcodes 6XNN y 7XNN"
git push                         # subir a GitHub
```

**Rutina recomendada:** cada vez que termines un paso del tutorial y **compile**, haz un commit. Si después lo rompes todo, podrás volver atrás.

---

## 4. Promocionar: desarrollo → calidad → master

```bash
# 1. De desarrollo a calidad
git switch calidad
git merge desarrollo
git push

# 2. Probar en calidad (compilar + ROMs de test). Si todo va bien:

# 3. De calidad a master
git switch master
git merge calidad
git push

# 4. Volver a trabajar
git switch desarrollo
```

---

## 5. Ver el historial y los cambios

```bash
git log --oneline --graph --all  # historial de todas las ramas, en formato compacto
git diff                         # qué has cambiado y todavía no has preparado
git diff --staged                # qué has preparado (add) y todavía no has commiteado
git branch                       # ramas locales (* = la actual)
git branch -a                    # también las de GitHub
```

---

## 6. Deshacer cosas

| Situación | Comando | ¿Peligroso? |
|---|---|---|
| He modificado un archivo y quiero dejarlo como estaba en el último commit | `git restore src/cpu.cpp` | ⚠️ Pierdes los cambios de ese archivo |
| He hecho `add` de algo por error | `git restore --staged archivo` | No, solo lo quita del staging |
| Me he equivocado en el mensaje del último commit (y **no** he hecho push) | `git commit --amend -m "nuevo mensaje"` | No |
| Quiero deshacer un commit que **ya** está en GitHub | `git revert <hash>` | No: crea un commit nuevo que lo deshace |
| Quiero guardar mis cambios un momento para cambiar de rama | `git stash` … `git stash pop` | No |
| Quiero volver a como estaba hace X commits, borrando todo lo posterior | `git reset --hard <hash>` | 🔴 **Sí. Destruye trabajo. Pregúntame antes.** |

El `<hash>` son los 7 caracteres que ves en `git log --oneline` (por ejemplo `9df1944`).

---

## 7. Buenos mensajes de commit

```
✅ "Implementar instrucción DXYN con recorte de bordes"
✅ "Corregir precedencia de operadores en 8XYE"
✅ "Añadir cargador de ROMs con comprobación de tamaño"

❌ "cambios"
❌ "arreglos"
❌ "asdf"
```

Fórmula: **un verbo en infinitivo + qué**. Si lo lees dentro de 3 meses, tienes que entender qué se hizo.

---

## 8. `.gitignore`

Tu proyecto ya tiene uno. Excluye:
- `build/`: todo lo que genera CMake, incluido SDL descargado (cientos de MB).
- `.vscode/` y `.vs/`: configuración local del editor.
- `*.exe`, `*.obj`, `*.pdb`: archivos compilados.

**Regla:** a Git se sube el **código fuente**, nunca lo que se genera a partir de él.

¿Y las ROMs (`.ch8`)? Las de test de Timendus y las de dominio público se pueden subir. ROMs comerciales, no.

---

## 9. Git desde VS Code (sin terminal)

| Acción | Dónde |
|---|---|
| Ver los cambios | Icono de **Control de código fuente** en la barra izquierda (`Ctrl+Shift+G`) |
| Preparar (add) | El botón **+** junto a cada archivo |
| Commit | Escribe el mensaje arriba y pulsa **✓ Confirmar** |
| Push / Pull | El botón **Sincronizar cambios**, o los `↑↓` de la barra inferior |
| Cambiar de rama | Haz clic en el nombre de la rama, **abajo a la izquierda** |
| Ver qué ha cambiado en un archivo | Haz clic en el archivo en el panel de Control de código fuente |

---

## 10. GitHub CLI (`gh`)

```bash
gh repo view --web               # abrir el repositorio en el navegador
gh repo view                     # ver la info del repositorio en la terminal
gh browse src/cpu.cpp            # abrir un archivo concreto en GitHub
```

---

## 11. Avisos que verás y no son errores

| Mensaje | Significado |
|---|---|
| `LF will be replaced by CRLF` | Windows y Linux marcan el final de línea de forma distinta. Git lo convierte solo. **Ignóralo.** |
| `Your branch is ahead of 'origin/desarrollo' by 2 commits` | Tienes 2 commits sin subir → `git push` |
| `Your branch is behind ...` | Hay cambios en GitHub que no tienes → `git pull` |
| `Already up to date.` | No había nada que fusionar |

**Un "error" que sí tienes que atender:**
- `CONFLICT (content): Merge conflict in archivo` → la misma línea se modificó en las dos ramas. Abre el archivo en VS Code: te marcará las dos versiones con botones para elegir. Elige, guarda, y después haz `git add` + `git commit`.
  Con tu flujo (siempre de `desarrollo` hacia arriba) casi nunca te pasará.
