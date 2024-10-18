# Overview
- board: Schematics and PCB layout
    - fusion: Fusion project archive
    - proto: KiCad prototype project
- CAD
    - 3d-files: .stl files for 3D printing the case
    - components: .STEP files for some components
- src: platform.io project files and deps

# Building

```bash
git clone --recurse-submodules git@github.com:radupascale/smartwatch-licenta.git
```

Afterwards open the `src` directory and build&upload using `platformio` (this project used the VScode extension).