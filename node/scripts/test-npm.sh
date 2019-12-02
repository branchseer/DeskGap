#!/usr/bin/env bash
set -e
scriptDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
echo $scriptDir$

rm -rf ./npm_test && cp -r $scriptDir/../npm/test ./npm_test
cd npm_test
echo "Installing denpendencies of npm_test"
npm install
echo "Installing DeskGap to npm_test"
npm install ../npm.tgz
npm test
