# Libraries
There are currently 6 different libraries implemented into this library "package".
They are inside the `/src/libs/` folder, and they are all C header files. They all
have `/src/lua_wrap.h` as a dependency, as `../lua_wrap.h`. If you change the location
of these files, there might be problems with includes. The header file that brings them
all together is `/src/lua_wrap_stdlibs.h`, which is included in `/src/main.c`. If you
want to remove the SDL2 wrapper library implementation (`/src/libs/sdl.h`), please go in
`/src/lua_wrap_stdlibs.h` and remove the line `#define LUA_STDLIBS_SDL`. If you do so,
please also remove the line `target_link_libraries(lua-test SDL2)` in `CMakeLists.txt`.
Other libraries will not need any additional linking, just remember to link the lua
library version installed on your computer.

# Dependencies
For every compilation in this project you will need a version of the Lua library
installed on your computer. Thankfully, you will only need to setup the linking part
since the project already has the Lua include files. For my setup, I used the system wide
install of lua5.4 on linux. For other versions or setups, please tweak the compilation settings
and stuff.
For the SDL2 wrapper library `/src/libs/sdl.h`, you will need to install and
setup SDL2. I used the SDL2 system wide install on linux, but if you do not have
the same setup, please tweak `CMakeLists.txt` and/or the compilation commands to
accomodate your setup.

# How to compile into a standalone app
To compile the standalone app, just compile using `cmake`.
```
git clone https://github.com/DioDogHater/lua-libs
cd lua-libs
cmake -S . -B ./build
cmake --build ./build
```
The executable will be `./build/lua-test/`.

# How to compile into a shared library
First, create a C file inside of `/src/` with this as its contents:
`#include "lua_wrap_stdlibs.h"`.
That's it! No more lines needed. Now, you need to compile the C file
into a shared library. Beware that the `./libs/sdl.h` library uses the SDL2
library so please link it accordingly.