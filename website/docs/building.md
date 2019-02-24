# Build Instructions

## Prerequisites
* [Node.js building prerequisites](https://github.com/nodejs/node/blob/master/BUILDING.md)
* The Node.js that shares the same version and architecture with DeskGap‘s Node.js
    * Version: See `npm/node-version.js`
    * Architecture: `x64` on macOS and `ia32` on Windows
* Visual Studio 2017 Installation with the following components checked:
    * [x] Desktop development with C++
        * [x] C++/CLI support
    * [x] .NET desktop development


## Steps

1. Run `npm ci --ignore-scripts` in `core`, `scripts`, `npm`, and `test`.
2. Download Node.js and the other dependencies: `node scripts/download-deps.js`
3. Build Node.js as a static library (this may take a while): `node scripts/build-node.js`
4. Build the native addon of DeskGap: run `npm run configure-native && npm run build-native` in `core`
5. Compile the TypeScript library: run `npm run build-lib-node && npm run build-lib-ui` in `core`
6. Configure the CMake for building the executable
    1. `mkdir build && cd build`
    2. `cmake -G Xcode ..` on macOS, `cmake -G "Visual Studio 15 2017" ..` on Windows, `cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..` on Linux.
7. Build the executable and copy the resources: `cmake --build . --target ALL_BUILD --config Release`