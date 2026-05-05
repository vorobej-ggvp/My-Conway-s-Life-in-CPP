# Conway's Game of Life (with SFML 3.1)

A clean simple C++ implementation of John Conway’s cellular automaton using the SFML 3.1 library.

## Key Features
*   **Interactive Sandbox:** Draw or toggle cells using the mouse in real-time.
*   **Intelligent Auto-Stop:** The simulation automatically pauses upon total extinction or when reaching a stagnant state (infinite cycles like squares or blinkers).
*   **Dynamic UI:** Integrated Start/Pause/Resume button and a live "Alive Cells" counter in the window title.
*   **Performance Optimized:** Uses a clock-based timer instead of `sf::sleep` to ensure smooth UI responsiveness even on lower-end hardware.

## Prerequisites & Build
*   **Language:** C++17 or higher.
*   **Library:** SFML 3.1+.
*   **IDE:** Visual Studio 2026 (recommended).

**Setup:**
1. Clone the repository.
2. Link SFML 3.1 in your Project Properties.
3. Ensure the font file `font.ttf`, which is Artifakt Element Regular.ttf renamed, is located in the same directory as your executable (`.exe`).

## Controls
*   **Left Click (Grid):** Toggle cell life (available only while paused).
*   **Main Button:** Start simulation, Pause, or Resume from the current state.
*   **Visual Grid:** Helps with precise placement of complex patterns (Gliders, Oscillators, etc.).
