# Actinidia

Using Lua to write your games running on Windows10/Android. Only x64 system are supported.

## Parts

### [Actinidia](Actinidia)

You can drag any resource file (`*.res`) onto the program to launch the game. If no resource file specified, the program will run scripts in the `game.res` file of current directory. If no such file in the directory, the program will be under *direct mode* and launch scripts in the `game` folder.

### [GameHost](GameHost)

*Not implemented*

Run game scripts.

- Windows 10 - UWP app based on DirectX 11. [Microsoft Store](https://www.microsoft.com/zh-cn/p/xaml-controls-gallery/9msvh128x2zt)
- Android - See [*ActinidiaOnAndroid*](https://github.com/mooction/ActinidiaOnAndroid)

### [Tools](Tools)

- Pack resources to `.res` file. Note that file name start with a dot (`.`) will be skipped.
- Unpack a `.res` file
- Concatenate some images to a long image
- Generate a large image with characters in a specified fontface

## How to build games

See [*ActinidiaGames*](https://github.com/mooction/ActinidiaGames).

## Build instructions

This program support both Windows and Linux. Only support x64 systems.

### Windows

Visual Studio 2019 is required.

### Linux 

g++-8 is required to support C++17 filesystem:

```bash
sudo apt-get install g++-8
```

Install dependent libraries:

```bash
sudo apt-get install -y libgtk-3-dev zlib1g-dev libpng-dev libjpeg-dev liblua5.3-dev
```

Clone this repository and build:

```bash
git clone https://github.com/mooction/actinidia
cd actinidia
make
```
