#!/usr/bin/env bash
set -e
scriptDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
echo $scriptDir$

npmTGZFile=$1
if [ ! -f "$npmTGZFile" ]; then
    echo "The npmTGZFile file does not exist: $npmTGZFile"
    exit 1
fi

rm -rf ./npm_test && cp -r $scriptDir/../npm/test ./npm_test
ls npm_test
cat npm_test/package.json
echo "Installing denpendencies of npm_test"
npm --prefix npm_test install
echo "Installing DeskGap to npm_test"
npm --prefix npm_test install "$npmTGZFile"
npm --prefix npm_test run test-js
npm --prefix npm_test run test-ts
