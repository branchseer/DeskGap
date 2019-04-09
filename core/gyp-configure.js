const nodeVersion = require('../npm/node-version');
const arch = process.platform === 'win32' ? 'ia32': 'x64';
const { execSync } = require('child_process');

execSync(
    `node-gyp configure --target=${nodeVersion} --arch=${arch}`,
    { stdio: 'inherit', env: process.env }
);
