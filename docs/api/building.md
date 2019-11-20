# Build Instructions

## Prerequisites

* A recent version of [Node.js](http://nodejs.org)
* [node-gyp](https://github.com/nodejs/node-gyp#installation)

### macOS
* Xcode 10 or later
* [CMake](https://cmake.org) 3.10 or later

### Windows
* Windows 10 April 2018 Update (version 1803) or later
* The "Desktop development with C++" workload from [Visual Studio 2017](https://docs.microsoft.com/en-us/visualstudio/releasenotes/vs2017-relnotes) (the free “Community” version is OK)

### Linux
* `gcc-8` and `g++-8`
* [CMake](https://cmake.org) 3.10 or later


## Steps

### Download the Dependencies
1. Download the npm dependencies: execute `npm ci --ignore-scripts` in the following locations:
    * `core/`
    * `scripts/`
    * `npm/`
    * `test/`
2. Download the prebuilt Node.js static library: `node scripts/download-deps.js`. (Alternatively, [build Node.js from source](http://github.com/patr0nus/libnode). Note that it may take a while)

### Configure
1. Configure node-gyp: `cd core && npm run configure-native`
2. Configure cmake:
    1. `mkdir build && cd build`
    2. Generate the project:
        * macOS: `cmake -G Xcode ..`  
        * Windows: `cmake -G "Visual Studio 15 2017" ..`  
        * Linux: `cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..`
   
### Compile
1. Compile TypeScript:
    1. `cd core/`
    2. `npm run build-lib-node`
    3. `npm run build-lib-ui`
2. Build the native bindings: `cd core && npm run build-native`
3. Build the executable and copy the resources:
    1. `cd build/`
    2. Build all the CMake targets:
        * macOS or Windows: `cmake --build . --target ALL_BUILD --config Release`
        * Linux: `make`
