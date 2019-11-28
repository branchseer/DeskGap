#!/usr/bin/env bash
set -e
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

cd $DIR/..

if [[ "$OSTYPE" == "linux-gnu" ]]; then
    CMakeGenerator="Unix Makefiles";
elif [[ "$OSTYPE" == "darwin"* ]]; then
    CMakeGenerator="Unix Makefiles";
else
    CMakeGenerator="Visual Studio 16 2019";
fi

rm -rf dist_build
cmake -G "$CMakeGenerator" -DCMAKE_BUILD_TYPE=Release -S . -B dist_build
cmake --build dist_build --config Release

cd dist_build
ls
mkdir dist
if [[ "$OSTYPE" == "linux-gnu" ]]; then
  mkdir dist/DeskGap
  cp DeskGap dist/DeskGap
  cp -r resources dist/DeskGap
  strip -x dist/DeskGap/DeskGap
elif [[ "$OSTYPE" == "darwin"* ]]; then
  cp -r DeskGap.app dist
  strip -x dist/DeskGap.app/Contents/MacOS/DeskGap
else
  mkdir dist/DeskGap
  cp Release/deskgap_winrt.dll dist/DeskGap
  cp Release/DeskGap.exe dist/DeskGap
  cp -r Release/resources dist/DeskGap
fi
