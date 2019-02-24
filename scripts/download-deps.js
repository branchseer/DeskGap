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
    if (process.platform === 'win32') {
        const nugetPackage = 'webview_nuget.zip';

        await downloadFile(
            'https://www.nuget.org/api/v2/package/Microsoft.Toolkit.Forms.UI.Controls.WebView/5.1.0',
            nugetPackage
        );

        await decompress(nugetPackage, '.', {
            filter: file => file.path === 'lib/net462/Microsoft.Toolkit.Forms.UI.Controls.WebView.dll',
            map: file => {
                file.path = 'Microsoft.Toolkit.Forms.UI.Controls.WebView.dll'
                return file;
            }
        });

        fs.unlinkSync(nugetPackage);
    }

    const nodeSourceURL = `https://nodejs.org/download/release/${nodeVersion}/node-${nodeVersion}.tar.gz`;
    const nodeSourcePackage = 'node.tar.gz';
    await downloadFile(nodeSourceURL, nodeSourcePackage);
    console.log("Extracting...");
    await decompress(nodeSourcePackage, '.');
    fs.renameSync(`node-${nodeVersion}`, "node");
    fs.unlinkSync(nodeSourcePackage);
})();
