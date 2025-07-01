# PentaGN

This project demonstrates a simple C++ application using SDL3.
It shows two windows side by side:

- The left window displays a pentagram with five LED circles.
- The right window acts as a control panel where LEDs can be selected and their color and brightness changed.

## Requirements

The application builds against SDL3. The SDL source is provided as a
git submodule under `vendored/SDL`. Clone the repository with
submodules enabled:

```bash
git clone --recurse-submodules <repo-url>
```

If you already cloned without submodules, run `git submodule update --init`.
No system SDL installation is required.

## Build and run the application

```bash
cmake -B build -S .
cmake --build build
./build/penta
```

Two windows will appear: the LED display on the left and the control panel on the right.
