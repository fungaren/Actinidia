# Actinidia

![logo](Actinidia/logo.png)

Using Lua to write your games running on Windows10/Linux/Android.

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

## Build instructions

This program support both Windows and Linux. Only support x64 systems.

### Windows

Visual Studio 2019 is required.

### Linux

g++-8 or higher is required to support C++17 filesystem:

```bash
sudo apt-get install g++ cmake
```

Install dependent libraries:

```bash
# Debian
sudo apt-get install -y libgtk-3-dev zlib1g-dev libpng-dev liblua5.3-dev libjpeg62-turbo-dev
# Ubuntu
sudo apt-get install -y libgtk-3-dev zlib1g-dev libpng-dev liblua5.3-dev libjpeg62-dev
```

Clone this repository and build:

```bash
git clone https://github.com/mooction/actinidia
cd actinidia/Actinidia
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
sudo make install
```

If you want to build a deb package, execute:

```bash
make package
```

## How to play

We provided some games in [example](example). You can try these games before writing your own games.

**Windows**

1. Just drag any resource file (`*.res`) onto the program to launch it.
2. If no resource file specified, the program will launch `game.res` under current directory.
3. If no such file in the directory, the program will launch scripts in the `game` folder.

> Install [Microsoft Visual C++ Redistributable for Visual Studio 2015, 2017 and 2019](https://aka.ms/vs/16/release/vc_redist.x64.exe) for any missing DLL.

**Linux**

1. Install dependencies:

```bash
# Debian
sudo apt-get install -y libgtk-3-0 zlib1g libpng16-16 liblua5.3-0 libjpeg62-turbo
# Ubuntu
sudo apt-get install -y libgtk-3-0 zlib1g libpng16-16 liblua5.3-0 libjpeg62
```

2. Install deb package:

```bash
sudo dpkg -i actinidia_1.0.0_amd64.deb
```

3. Launch your resource pack:

```bash
actinidia ./your_game.res
```

## How to build games

* Edit scripts in `lua/`, **DO NOT** modify `main.lua` and `core.lua`.
* Use `Tools.exe` generate `*.res` file. Note that the root folder MUST be named as `game`.

> A script debugger is on the way. You can now use `lua.exe` to check syntax and use interface `SaveSetting(key,value)` to observe an variable.

## Snapshots

* [RPG](https://moooc.oss-cn-shenzhen.aliyuncs.com/blog/actinidia_prev1.png)
* [RPG](https://moooc.oss-cn-shenzhen.aliyuncs.com/blog/actinidia_prev2.png)
* [RPG](https://moooc.oss-cn-shenzhen.aliyuncs.com/blog/actinidia_prev3.png)
* [FlappyBird](https://moooc.oss-cn-shenzhen.aliyuncs.com/blog/flappybird-1.png)
* [FlappyBird](https://moooc.oss-cn-shenzhen.aliyuncs.com/blog/flappybird-2.png)
* [FlappyBird](https://moooc.oss-cn-shenzhen.aliyuncs.com/blog/flappybird-3.png)
