const path = require('path');
const fs = require('fs');
const fse = require('fs-extra');
const { downloadFile } = require('./utils');
const decompress = require('decompress');

const nodeVersion = require('../npm/node-version')
const arch = process.platform === 'win32' ? 'x86': 'x64';

const depsFolder = path.resolve(__dirname, '..', 'deps');
fs.mkdirSync(depsFolder, { recursive: true });
process.chdir(depsFolder);

process.on('unhandledRejection', e => { throw e });
(async () => {
    const libnodeDownloadURL = `https://github.com/patr0nus/libnode/releases/download/libnode-${nodeVersion}/libnode-${nodeVersion}-${process.platform}-${arch}-nointl.zip`;
    const libnodeZipName = 'node.zip';
    await downloadFile(libnodeDownloadURL, libnodeZipName);

    console.log("Extracting...");
    try { await fse.remove('node'); } catch(e) { }
    await decompress(libnodeZipName, 'node');
    
    fs.unlinkSync(libnodeZipName);
})();
