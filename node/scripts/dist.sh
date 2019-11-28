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
  cp DeskGap dist
  cp -r resources dist
  strip -x dist/DeskGap
elif [[ "$OSTYPE" == "darwin"* ]]; then
  cp -r DeskGap.app dist
  strip -x dist/DeskGap.app/Contents/MacOS/DeskGap
else
  cp *.dll dist
  cp DeskGap.exe dist
  cp -r resources dist
fi
