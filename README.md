# Shroom Ventures
This is just a hobby 2D platformer I've been developing in my spare time to learn more about game dev. Gotta give a quick shoutout to my fiance gabby has been doing all of the finalized artwork. Everything you see is a massive work in progress since this is my first serious attempt at game dev and I'm doing it all in C with minimal dependencies.

## Todos
1) ~~First finish camera~~
2) ~~Create config.mk that also supports compiling on msys~~
3) ~~Finish character movement control (sprint, crouch etc)~~
4) ~~Implement a dash mechanic instead of a sprint mechanic~~
5) ~~Implement a fixed time step instead of a variable one~~
5) ~~Figure out a pause screen~~
6) ~~Figure out a static background~~
7) ~~Figure out background soundtrack~~
8) ~~Figure out sound effects for player~~
9) ~~Refactor day tomorrow:~~
    - ~~Move background and background music into map~~
    - ~~Give map and player initializer functions~~
    - ~~Give player draw functions~~
    - ~~Give background play sound function~~
9) ~~Figure out animations~~
10) ~~Redesign collision logic~~
11) ~~Refactor the code to get rid of the stupid design choice I made to make current
	every component of current state an explicit function of previous state and 
	current inputs~~
12) ~~Finish implementing a fullscreen functionality~~
13) Refactor player state
14) Add double jump, slide, pull-up jump movements
15) Figure out basic ai
16) Load a map (maybe? see how it goes)
17) Create a startup menu
18) ~~Reinstall raylib and link statically instead of shared library~~
19) ~~Make makefile configurable and not dependent on my setup~~

## Build instructions
Currently, I support building on MSYS2 with the mingw64 environment on Windows, and natively on linux. This project does depend on **Raylib** which provides most of the graphics and sound APIs I use for this game. Their work is amazing and I'd recomment taking the time to [check them out](https://github.com/raysan5/raylib/). Before building Shroom Ventures from source, follow the instructions on Raylibs github repo to build and install the Raylib library on your system.

I am trying to make the project build as flexible as possible while maintaining stable defaults. Read `config.mk` to see how you can configure the build and read `makefile` to see what the available make targets are.

### Building For Linux 
To build on linux first install raylib through your system pacakge manager. If you choose to install raylib from source or if your system package manager installs raylib in a location other than "/usr/local", then you should modify the `RAYLIB_INSTALL` path to the appropriate path for your raylib installation in `config.mk`. After installing raylib and configuring `config.mk` appropriately, you can simply do:
    
    make release

### Building For Windows
Building for Windows is only supported by using MSYS2's mingw64 environment. Once again, a really awesome project and I would highly recommend [checking them out](https://github.com/msys2/).

To build on Windows you first have to install Raylib following the installation instructions for installing on MSYS on Raylib's github. After installing raylib you have two options: either pass `PLATFORM` argument to make and set it equal to `MSYS` like so:
    
    make release PLATFORM=MSYS

Or modify the value of `PLATFORM` in the `config.mk` file and set it to `MSYS` and then run
    
    make release

### Credits

Gabby Falkowski - All the drawn art is made by her

Background music - Red Baron - flying fighter plane video game soundtrack 1942 sopwith by melodyayresgriffiths [link](https://pixabay.com/music/video-games-red-baron-flying-fighter-plane-video-game-soundtrack-1942-sopwith-159522/)

Player jump sound effect - JumpGroan002.wav by TaraMG [link](https://freesound.org/people/TaraMG/sounds/386043/)

Player dash sound effect - Swing Woosh Weapon 1 by floraphonic [link](https://pixabay.com/sound-effects/swing-whoosh-weapon-1-189819/)
