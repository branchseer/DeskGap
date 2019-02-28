const path = require('path');
const fs = require('fs');
const { downloadFile } = require('./utils');
const decompress = require('decompress');

const nodeVersion = require('../npm/node-version')

const depsFolder = path.resolve(__dirname, '..', 'deps');
fs.mkdirSync(depsFolder, { recursive: true });
process.chdir(depsFolder);

process.on('unhandledRejection', e => { throw e });
(async () => {
    const nodeSourceURL = `https://nodejs.org/download/release/${nodeVersion}/node-${nodeVersion}.tar.gz`;
    const nodeSourcePackage = 'node.tar.gz';
    await downloadFile(nodeSourceURL, nodeSourcePackage);
    console.log("Extracting...");
    await decompress(nodeSourcePackage, '.');
    fs.renameSync(`node-${nodeVersion}`, "node");
    fs.unlinkSync(nodeSourcePackage);
})();
