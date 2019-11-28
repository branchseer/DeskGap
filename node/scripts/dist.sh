#!/usr/bin/env bash
set -e
scriptDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
deskGapNodeDir=$scriptDir/..

rm -rf dist_build

if [[ "$OSTYPE" == "linux-gnu" ]] || [[ "$OSTYPE" == "darwin"* ]]; then
  cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -S "$deskGapNodeDir" -B dist_build
  cmake --build dist_build
else
  cmake -G "Visual Studio 16 2019" -A Win32 -S "$deskGapNodeDir" -B dist_build
  cmake --build dist_build --config Release
fi

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
