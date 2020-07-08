const { shellNative } = require('./bindings');
const { resolve } = require('path');

export default {
    openExternal: (url: string): boolean => shellNative.openExternal(url),
    showItemInFolder: (path: string) => shellNative.showItemInFolder(resolve(path)),
};
