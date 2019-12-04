#!/usr/bin/env bash
set -e
scriptDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
deskgapVersion=$(<$scriptDir/../VERSION)

for zipFilePath in $DESKGAP_DISTS_DIR/*.zip
do
	zipFilename=$(basename $zipFilePath)
	echo Uploading $zipFilename
	curl -T "$zipFilePath" -u$DESKGAP_BINTRAY_USER:$DESKGAP_BINTRAY_KEY https://api.bintray.com/content/patr0nus/DeskGap/releases/$deskgapVersion/$zipFilename
done
curl -X POST -u$DESKGAP_BINTRAY_USER:$DESKGAP_BINTRAY_KEY https://api.bintray.com/content/patr0nus/DeskGap/releases/$deskgapVersion/publish
npm publish $DESKGAP_NPM_TARBALL
