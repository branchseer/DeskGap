import path = require('path');
import fs = require('fs');
const { appNative } = require('../bindings');

let appPath = path.join(process.resourcesPath, 'app');
const envEntry = process.env['DESKGAP_ENTRY'];
if (envEntry != null && fs.existsSync(path.join(appPath, 'DESKGAP_DEFAULT_APP'))) {
    appPath = envEntry;
}
export default appPath;
