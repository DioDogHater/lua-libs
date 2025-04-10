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