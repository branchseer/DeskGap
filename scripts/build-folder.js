const path = require('path');
const os = require('os');

module.exports = path.resolve(__dirname, '..', 'build');
if (process.platform === 'win32') {
    module.exports = path.join(os.homedir(), 'deskgap-build');
}
