const path = require('path');
const os = require('os');
const { execSync } = require('child_process');

const nodeSourcePath = path.resolve(__dirname, '..', 'deps', 'node');

process.chdir(nodeSourcePath);

let buildCommands;
if (process.platform === 'darwin' || process.platform === 'linux') {
    buildCommands = [
        './configure --without-intl --enable-static',
        `make -j${Math.max(os.cpus().length - 1, 1)}`
    ];
}
else if (process.platform === 'win32') {
    buildCommands = ['vcbuild.bat x86 without-intl static'];
}

for (const cmd of buildCommands) {
    execSync(cmd, { stdio: 'inherit' });
}
