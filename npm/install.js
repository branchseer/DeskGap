const path = require('path');
const fs = require('fs');
const { downloadFile, sha256OfPath } = require('./util');
const decompress = require('decompress');

process.on('unhandledRejection', (error) => {
    throw error;
});

let deskGapPlatform = null;
if (process.platform === 'darwin') {
    deskGapPlatform = 'darwin-x64';
}
else if (process.platform === 'win32') {
    deskGapPlatform = 'win32-ia32';
}

if (deskGapPlatform == null) {
    console.error('DeskGap doesn’t support your platform: ' + process.platform);
    process.exit(1);
}


const distFiles = require('./dist_files');
const distZipFile = distFiles[deskGapPlatform];
const distZipFilePath = path.join(__dirname, distZipFile.filename);
const downloadURL = "https://dl.bintray.com/patr0nus/DeskGap/" + distZipFile.filename;

(async () => {
    await downloadFile(downloadURL, distZipFilePath);
    const sha256 = await sha256OfPath(distZipFilePath);
    if (sha256 !== distZipFile.sha256) {
        console.error(`SHA256 mismatch (${sha256} !== ${distZipFile.sha256})`);
        process.exit(1);
    }
    await decompress(distZipFilePath, 'dist');
    fs.unlinkSync(distZipFilePath);
})();
