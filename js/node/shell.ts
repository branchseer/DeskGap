const { shellNative } = require('./bindings');

export default {
    openExternal: (url: string): boolean => shellNative.openExternal(url)
};
