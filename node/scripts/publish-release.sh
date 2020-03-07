#!/usr/bin/env bash
set -e
scriptDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
deskgapVersion=$(<$scriptDir/../VERSION)

for zipFilePath in $DESKGAP_DISTS_DIR/*.zip
do
	zipFilename=$(basename $zipFilePath)
	uploadUrl=https://api.bintray.com/content/patr0nus/DeskGap/releases/$deskgapVersion/$zipFilename
	echo Uploading $zipFilename
	statusCode=$(curl -s -o /dev/null -w "%{http_code}" -T "$zipFilePath" -u$DESKGAP_BINTRAY_USER:$DESKGAP_BINTRAY_KEY $uploadUrl)
	if [[ ! $statusCode == 2* ]]; then
		echo Upload failed: HTTP $statusCode
		exit 1
	fi
done

statusCode=$(curl -s -o /dev/null -w "%{http_code}" -X POST -u$DESKGAP_BINTRAY_USER:$DESKGAP_BINTRAY_KEY https://api.bintray.com/content/patr0nus/DeskGap/releases/$deskgapVersion/publish)
if [[ ! $statusCode == 2* ]]; then
	echo Publish failed: HTTP $statusCode
	exit 1
fi

echo "//registry.npmjs.org/:_authToken=${NPM_TOKEN}" > $HOME/.npmrc
npm publish $DESKGAP_NPM_TARBALL --tag=$DESKGAP_AP_NPM_TAG
