#!/usr/bin/env bash
set -e
scriptDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

deskgapVersion=$(<$scriptDir/../VERSION)

rm -rf ./npm && cp -r $scriptDir/../npm .
cp $scriptDir/../../docs/README.md ./npm
mkdir npm/dist_files

rm -rf dist_zips && mkdir dist_zips
for distFolder in ./dists/*; do
  distName=`basename $distFolder`
  zipFilename=deskgap-v$deskgapVersion-$distName.zip
  zipFilePath="$PWD/dist_zips/$zipFilename"
  fileInfoJSONPath="$PWD/npm/dist_files/$distName.json"
  pushd $distFolder
  zip -r "$zipFilePath" ./*  -x "*.DS_Store" > /dev/null
  sha256output=( `cmake -E sha256sum "$zipFilePath"` )
  echo \{\"filename\":\"$zipFilename\",\"sha256\":\"${sha256output[0]}\"\} > "$fileInfoJSONPath"
  popd
done
