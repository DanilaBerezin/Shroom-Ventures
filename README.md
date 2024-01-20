# Shroom Ventures
A fun 2D platformer I will be developing and my girlfriend (Gabby) will be doing art for. Current WIP, basically nothing is implemented at this stage. Still trying to figure out raylib and game dev in general.

## Todos
1) ~~First finish camera~~
2) ~~Create config.mk that also supports compiling on msys~~
3) ~~Finish character movement control (sprint, crouch etc)~~
4) ~~Implement a dash mechanic instead of a sprint mechanic~~
5) ~~Implement a fixed time step instead of a variable one~~
5) ~~Figure out a pause screen~~
6) ~~Figure out a static background~~
7) ~~Figure out background soundtrack~~
8) Figure out sound effects for
9) Figure out animations
10) Create a startup menu
11) Figure out basic ai
12) Load a map (maybe? see how it goes)
13) ~~Reinstall raylib and link against archive instead of shared library~~
14) ~~Make makefile configurable and not dependent on my setup~~

## Build instructions
Currently, I support building on MSYS2 with the mingw64 environment on Windows, and natively on linux. This project does depend on **Raylib** which is a beautiful project and you should please take a moment to [check them out](https://github.com/raysan5/raylib/). Before building Shroom Ventures from source, follow the instructions on Raylibs github repo to build and install the Raylib library on your system.

I am trying to make the project build as flexible as possible while maintaining stable defaults. Check out `config.mk` to see how you can configure the build and checkout `makefile` to see what the available make targets are.

### Building For Linux 
To build on linux first install the raylib dependency as mentioned earlier, then you can simply do:
    
    make release

### Building For Windows
Building for Windows is only supported by using MSYS2's mingw64 environment. Please also take some time to [check them out](https://github.com/msys2/).

To build on Windows you first have to install raylib following the installation instructions on their github. The recommendation is to use `pacman` to install the raylib package that's already been packaged to the MSYS2 repos. After installing raylib you have two options: either pass `PLATFORM` argument to make and set it equal to `MSYS` like so:
    
    make release PLATFORM=MSYS

Or modify the value of `PLATFORM` in the `config.mk` file and set it to `MSYS` and then run
    
    make release

### Credits

Gabby Falkowski - All the drawn art is made by her
Background music - Epic background orchestral music. Fairytale World Emily. Hip-Hop ver. by White_Records. [link to music](https://pixabay.com/music/beats-epic-background-orchestral-music-fairytale-world-emily-hip-hop-ver-185382/)
