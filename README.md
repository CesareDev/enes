# enes

NES emulator project written in C following this [guide](https://bugzmanov.github.io/nes_ebook/)

## Requirements

- `cmake`
- `raylib`
    - In the project there are already the compiled version of the library under `enes/dep/`
    - For now there are no roms available

## Installation

Clone the reposistory 
    - `git clone https://github.com/CesareDev/enes.git`
Launch the script file to generate the `Makefile`
    - `cd enes && ./generate.sh`
Go into the generated `build` directory then go into one of the two target directory `debug` or `release`
    - `cd build/release`
Navigate into the `enes` folder and run `make`
    - `cd enes && make`
Run the executable
    - `./enes`

## Mention

- [raylib](https://github.com/raysan5/raylib)
