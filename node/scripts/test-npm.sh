#!/usr/bin/env bash
set -e
scriptDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
echo $scriptDir$

npmTGZFile=$1

rm -rf ./npm_test && cp -r $scriptDir/../npm/test ./npm_test
ls npm_test
npm --prefix npm_test install
npm --prefix npm_test install "$npmTGZFile"
npm --prefix npm_test run test-js
npm --prefix npm_test run test-ts
