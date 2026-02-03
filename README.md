# Tetris C11 (SDL2) — Arquitetura modular, gameplay puro e testes

Projeto de Tetris em C (C11) para Linux (Ubuntu), com SDL2 como backend de janela/render/input,
HUD via bitmap font interno (sem SDL_ttf), gameplay separado e testável.

## Dependências
- Ubuntu:
```bash
  sudo apt update
  sudo apt install -y build-essential cmake libsdl2-dev
````

## Build e execução

```bash
mkdir -p build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/tetris
```

Build Debug (com logs mais verbosos):

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
./build/tetris
```

## Rodar testes (ctest)

```bash
cmake -S . -B build -DTETRIS_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
ctest --test-dir build --output-on-failure
```

## Controles padrão

* Movimento: Setas ou A/D (esq/dir), S (soft drop)
* Hard drop: Espaço
* Rotação: Z (CCW), X (CW)
* Hold: C ou LShift
* Pause: P ou Esc
* Confirm: Enter
* Back: Backspace

## Arquitetura (resumo)

* `gameplay/` é “puro”: não conhece SDL, só recebe ações (Command) e avança o estado.
* `render/` consome o estado para desenhar (virtual resolution).
* `input/` mapeia teclado para ações e aplica DAS/ARR.
* `core/state` implementa State pattern (Menu/Jogo/Pause/etc.).
* `core/event_bus` implementa Observer para FX/UI reagirem a eventos do gameplay.

## Padrões de projeto usados

* Game Loop: fixed timestep 60Hz (`core/time`)
* State: `State` + `StateStack` (`core/state` + `ui/state_*`)
* Strategy:

  * randomizer: 7-bag vs random (`gameplay/randomizer`)
  * rotação: SRS (`gameplay/srs`)
  * scoring: clássico/modern-lite (`gameplay/scoring`)
  * curva de velocidade por nível (em `gameplay/gameplay`)
* Observer/Event Bus: `EventBus` (`core/event_bus`) para line clear, lock, level up...
* Command: `GameAction` + `InputCommand` (`input/input`)
* Factory: criação de peças e estados (`gameplay/piece`, `ui/state_*`)

## Config (config.ini)

O jogo cria/lê `config.ini` no diretório de execução.
Você pode alterar:

* ghost on/off
* hold on/off
* modo (marathon/sprint/ultra-lite)
* DAS/ARR
* keybinds

Exemplo (gerado automaticamente na primeira execução):

```ini
[video]
fullscreen=0
scale=3

[game]
mode=marathon
ghost=1
hold=1
das_ms=140
arr_ms=25
start_level=1

[keys]
left=A
right=D
down=S
hard_drop=SPACE
rot_cw=X
rot_ccw=Z
hold=C
pause=ESC
confirm=RETURN
back=BACKSPACE
```

## Observações

* HUD usa fonte bitmap interna 5x7: sem assets obrigatórios.
* Áudio é opcional e não incluído por padrão para manter dependências mínimas.
