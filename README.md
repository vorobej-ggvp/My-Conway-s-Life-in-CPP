# Conway's Game of Life (with SFML 3.1)

A clean, feature-rich C++ implementation of John Conway's cellular automaton using the SFML 3.1 library.

## Key Features
*   **Full Game of Life Engine:** Complete cell update cycle with stagnation detection — the simulation auto-pauses on total extinction or when the field loops (checked against the last 4 frames).
*   **Stamp Library:** 5 built-in structures — *Glider*, *Gosper Gun*, *Simkin Gun*, *35P12H6V0*, *Noah's Ark* — selectable via keys **1–5**, with repeat press or **0** reverting to the pencil tool.
*   **Stamp Rotation & Preview:** Rotate any stamp with **R** (90°/180°/270°) and see a semi-transparent ghost preview showing exact placement before clicking.
*   **Brush Drawing:** Hold and drag the left mouse button to paint or erase cells smoothly across the grid.
*   **Mouse Wheel Zoom:** Zoom in/out freely for working with large structures or fine details.
*   **Fullscreen Mode:** Toggle borderless fullscreen with **F11** — system cursor stays visible and UI stays intact.
*   **Adaptive Square Grid:** On any window resize or fullscreen toggle, the grid auto-centers and maintains perfect square proportions without stretching cells.
*   **Separate Camera Views:** Game world and UI live in independent coordinate systems — zoom never breaks buttons.
*   **Optimized Rendering:** Grid lines drawn once via `sf::VertexArray`, keeping CPU usage low.
*   **Bottom UI Panel:** Dedicated dark panel with a context-aware **Start/Pause/Resume** button, a **Clear** button (resets field, history, and score), and a live **X, Y** cursor coordinate display.
*   **Live Stats:** Alive cell count and score tracked in real-time, displayed directly in the window title.
*   **Out-of-Bounds Safety:** Boundary checks on all draw and stamp operations to prevent crashes.

## Prerequisites & Build
*   **Language:** C++17 or higher.
*   **Library:** SFML 3.1+.
*   **IDE:** Visual Studio 2022 (recommended).

**Setup:**
1. Clone the repository.
2. Link SFML 3.1 in your Project Properties.
3. Ensure the font file `font.ttf` (Artifakt Element Regular.ttf renamed) is located in the same directory as your executable (`.exe`).

## Controls

### Mouse
*   **Left Click / Drag (Grid):** Toggle or paint/erase cells (available only while paused).
*   **Mouse Wheel:** Zoom in/out on the grid.
*   **Left Click (UI Button):** Start, Pause, Resume, or Clear the simulation.

### Keyboard
| Key | Action |
|-----|--------|
| `Space` | Start / Pause simulation |
| `F11` | Toggle borderless fullscreen |
| `Escape` | Quit the program |
| `1` – `5` | Select stamp (Glider, Gosper Gun, Simkin Gun, 35P12H6V0, Noah's Ark) |
| `0` | Revert to pencil tool |
| `R` | Rotate selected stamp (90° steps) |2. Link SFML 3.1 in your Project Properties.
3. Ensure the font file `font.ttf`, which is Artifakt Element Regular.ttf renamed, is located in the same directory as your executable (`.exe`).

## Controls
*   **Left Click (Grid):** Toggle cell life (available only while paused).
*   **Main Button:** Start simulation, Pause, or Resume from the current state.
*   **Visual Grid:** Helps with precise placement of complex patterns (Gliders, Oscillators, etc.).
