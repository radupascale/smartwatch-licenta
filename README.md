# Intro

<p align="center">
<img src="https://github.com/user-attachments/assets/a926d2e6-5f9e-4f0d-a9ef-c2ae39191c80" width="500" height="250">
</p>

This repository contains all the files necessary to build the smartwatch I developed for my thesis at the Computer Science Faculty of UPB.
I plan to add step-by-step building instructions in the future, but for now I'll leave only an overview of the directory structure.

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
