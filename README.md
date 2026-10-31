# microcosm

WIP

## Setup

1. Grab a prebuilt SDL3 (the **MinGW dev** package on Windows) from
   <https://github.com/libsdl-org/SDL/releases> and unpack it anywhere.
2. Copy `local.cmake.example` to `local.cmake` and point `SDL3_DIR` at the
   package's `cmake` directory:

   ```cmake
   set(SDL3_DIR "C:/path/to/SDL3-<version>/cmake" CACHE PATH "")
   ```

## Build and run

```bash
./make.sh          # Debug (default)
./make.sh Release  # or any CMAKE_BUILD_TYPE
./run.sh           # builds, then runs build/Debug/microcosm.exe
./run.sh Release   # same, Release config
```