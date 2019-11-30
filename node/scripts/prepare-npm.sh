#!/usr/bin/env bash
set -e
scriptDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
deskgapVersion=$(<$scriptDir/../VERSION)

$distsDir=$1
if [ ! -d "$distsDir" ]; then
    echo "The distsDir folder does not exist: $distsDir"
    exit 1
fi

echo "Preparing the npm package..."
rm -rf ./npm && cp -r $scriptDir/../npm .
cp $scriptDir/../../docs/README.md ./npm
node -e "fs.writeFileSync('./npm/package.json', JSON.stringify(Object.assign(require('./npm/package.json'), { version: '$deskgapVersion'})))"
npx --no-install tsc -p $scriptDir/../js/tsconfig-d-ts.json --outDir ./npm/types

echo "Zipping binaries..."
mkdir npm/dist_files
rm -rf dist_zips && mkdir dist_zips
for distFolder in $distsDir/*; do
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

echo "Packing the npm package..."
npmPackFileName=`npm pack ./npm`
mv $npmPackFileName npm.tgz
