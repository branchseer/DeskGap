## Added
- A standalone cross-platform C/C++ library: [libdeskgap](https://github.com/patr0nus/DeskGap/tree/master/lib), which DeskGap statically links against. Notes:
	- No documentation for now. See the [headers](https://github.com/patr0nus/DeskGap/tree/master/lib/src/include/deskgap) for usage details.
	- C APIs still lack. Most APIs are in C++.
- Additional info in the process object:
	- process.version.deskgap
	- process.resourcesPath

## Changed
- The native module and js are now embedded in the DeskGap executable.
- Build steps have been simplified.

## Fixed
- Windows woundn't go on top on macOS. #36
- A line ending issue on non-Windows platforms. #33
