const nodeVersion = require('../npm/node-version');
const path = require('path');
const fs = require('fs');

const archive = require('archiver')('zip', {
    zlib: { level: require('zlib').constants.Z_BEST_COMPRESSION }
}).once('warning', (err) => {
    throw err;
});

archive.glob('out/Release/*.a', {
    cwd: path.resolve(__dirname, '..', 'deps', 'node'),
    ignore: ['gtest', 'v8_nosnapshot']
});

archive.pipe(fs.createWriteStream(path.resolve(__dirname, '..', 'deps', 'node.zip')));
archive.finalize();
