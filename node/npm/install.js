const path = require('path');
const fs = require('fs');
const { downloadFile, sha256OfPath } = require('./util');
const decompress = require('decompress');
const fse = require('fs-extra');

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
else if (process.platform === 'linux') {
    deskGapPlatform = 'linux-x64';
}

if (deskGapPlatform == null) {
    console.error('DeskGap doesn’t support your platform: ' + process.platform);
    process.exit(1);
}

const distZipFile = require('./dist_files/' + deskGapPlatform);
const distZipFilePath = path.join(__dirname, distZipFile.filename);

(async () => {
    try {
        fs.unlinkSync(distZipFilePath);
    } catch (e) { }

    await downloadFile(distZipFile.filename, distZipFilePath);
    const sha256 = await sha256OfPath(distZipFilePath);
    if (sha256 !== distZipFile.sha256) {
        console.error(`SHA256 mismatch (${sha256} !== ${distZipFile.sha256})`);
        process.exit(1);
    }

    try {
        await fse.remove('dist');
    }
    catch (e) { }

    await decompress(distZipFilePath, 'dist');
    fs.unlinkSync(distZipFilePath);
})().catch(e => {
    console.error(e.stack || e.toString());
    process.exit(1);
});
