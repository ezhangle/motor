LÖVE is an awesome framework that provides an OpenGL backed 2D game engine programmable in the Lua scripting language.

A lot of great games have been made using LÖVE and the number is steadily growing. The only downside of LÖVE is that it cannot be embedded in websites for people to play. This is what Motor2D is trying to solve: A version of LÖVE that can run on any modern WebGL-enabled browser. How is it done?

Motor2D is a reimplementation of LÖVE's Lua interface in C, which is then compiled to JavaScript using the unbelievable emscripten toolset.

Check the [Wiki](https://github.com/rnlf/motor/wiki) for more information on compatibility and contributing!
