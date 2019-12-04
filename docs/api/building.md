# Build Instructions

## Prerequisites

* A recent version of [Node.js](http://nodejs.org)
* [CMake](https://cmake.org) 3.15+

### macOS
* Xcode 10+

### Windows
* Windows 10 April 2018 Update (version 1809) or later
* The "Desktop development with C++" workload from [Visual Studio 2019](https://docs.microsoft.com/en-us/visualstudio/releases/2019/release-notes)

### Linux

* `gcc-8` and `g++-8`
* libgtk-3-dev version 3.18.9 or later
* libwebkit2gtk-4.0-dev version 2.20.5 later


## Steps
1. Download the npm dependencies: `npm ci`
2. Generate the buildsystem:
    - Windows: `cmake -G "Visual Studio 16 2019" -A Win32 -S node -B build`
    - macOS or Linux: `cmake -G "Unix Makefiles" -S node -B build`
3. Build DeskGap: `cmake --build build`
4. Test:
	- macOS: `node node/test/start.js build`
	- Windows or Linux: `node node/test/start.js build/Release`
