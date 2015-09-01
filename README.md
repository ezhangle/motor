## Overview

LÖVE is an awesome framework that provides an OpenGL backed 2D game engine programmable in the Lua scripting language.

A lot of great games have been made using LÖVE and the number is steadily growing. The only downside of LÖVE is that it cannot be embedded in websites for people to play. This is what Motor2D is trying to solve: A version of LÖVE that can run on any modern WebGL-enabled browser. How is it done?

Motor2D is a reimplementation of LÖVE's Lua interface in C, which is then compiled to JavaScript using the unbelievable emscripten toolset.

Check the [Wiki](https://github.com/rnlf/motor/wiki) for more information on compatibility and contributing!

## Installation

### Dependencies
The build system is currently a pretty messy Python3 script, so you need Python for building. Besides that, there are only a few dependencies, most stuff is included in the repository.

#### Web 
You need emscripten (I am currently using 1.34.4, slightly older versions should work, too) and Python 3. Your emscripten installation should support the Closure compiler and thus requires a Java runtime. The Closure compiler is also needed for building the loader.

#### Native
There is also a native build (great for debugging), which requires Python for the build system and additionally the development libraries for SDL2, OpenAL, OpenGL and GLEW. So far, I have only tested it with clang 3.6, but since all of the code is C11 (and 99.9% is C99), any up-to-date compiler should be able to compile motor.

### Building
The build script build.py can be used to compile Motor for the Web and native. Just make a new directory (I suggest 'build' for web and whatever you want for native builds) and call `build.py build`. This automatically builds the web version (if emscripten can be found). If you want to build the native version for debugging (or because you like how small it is, compared to LÖVE), you can add the switch --native: `build.py build --native`.

After building the engine itself, the web version also requires a loader script, which can be built (in the same directory) with `build.py buildloader`.

## Running
At the moment, the native version offers much less flexibility as for example LÖVE does. You change to the directory with your (unpacked) game (i.e. lua files and all that) and run the motor2d executable.

The web version is more sophisticated: It comes with a development server, like web frameworks such as Jekyll, Hyde or Django do. At the moment, you add the directory that contains the git source to your PATH. If you have built motor into a subdirectory 'build', then the 'motor' script will search there for your engine, loader and HTML files.

You change your directory to the one that contains your game's lua files and run `motor serve`. You can connect to the server via your browser, it will listen on *:8000.

The other option is to put `motor2dloader.js`, `motor2d.js` and your .love file (which at the moment must be called `game.love`) into a directory on a server and load an appropriate HTML file (motor2d.html can be used as a template).


## Contributing

If you want to contribute to the project, feel free to work on one of the topics mentioned in the following list. I want to do a lot of the other stuff myself.

Please try to replicate the code style of the existing code, I might refuse to pull your changes otherwise.

* love.physics.
* Additional sound file formats.
* Clean up the build system (the current build.py is a real mess!). I want to add user-defined code (which can be compiled and linked into the final motor executable). This also requires configurability for the different modules. One user might not need all audio file formats, so she should be able to select the formats she wants. On second though, please contact me before you start working on that thing, I have some very specific ideas about it.
* Additions to the development server.
* General bug- und incompatibility-fixes
