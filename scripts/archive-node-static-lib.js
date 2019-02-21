const nodeVersion = require('../npm/node-version');
const path = require('path');
const fs = require('fs');

const archive = require('archiver')('zip', {
    zlib: { level: require('zlib').constants.Z_BEST_COMPRESSION }
}).once('warning', (err) => {
    throw err;
});

const nodeSrcFolder = path.resolve(__dirname, '..', 'deps', 'node');

switch (process.platform) {
case 'darwin':
	archive.glob('out/Release/*.a', {
	    cwd: nodeSrcFolder,
	});
	break;
case 'linux':
	archive.glob('out/Release/obj.target/**/@(*.a|*.o)', {
	    cwd: nodeSrcFolder,
	});
	break;
default:
	console.error(`Unsupported platform: ${process.platform}`);
	process.exit(1);
}

archive.pipe(fs.createWriteStream(path.resolve(__dirname, '..', 'deps', 'node.zip')));
archive.finalize();
