import { libPath, entryPath } from './internal/paths';
import app from './app';
import { BrowserWindow } from './browser-window';
import { Menu, MenuItem } from './menu';
import { WebViews, messageNode } from './webview';
import Dialog from './dialog';
import shell from './shell';
import './async-node';

import systemPreferences from './system-preferences';

//The native land doesn't have a mechanism to get __dirname, so we need to tell it from the js land.
const { setLibPath, setNativeExceptionConstructor } = require('./bindings');
setLibPath(libPath);

class NativeException extends Error {
    constructor(name: string, message: string) {
        super(message);
        this.name = name;
    }
}

setNativeExceptionConstructor(NativeException);

export = {
    app,
    BrowserWindow,
    webViews: WebViews,
    webContents: WebViews,
    Menu,
    MenuItem,
    messageNode,
    ipcMain: messageNode,
    systemPreferences,
    dialog: Dialog,
    NativeException,
    shell,
};


process.on('uncaughtException', (error) => {
    if (process.listeners('uncaughtException').length > 1) {
        return;
    }
    const message = error.stack || `${error.name}: ${error.message}`;
    Dialog.showErrorBox('Uncaught exception', message);
    process.exit(1);
});

process.nextTick(() => {
    app['run_']();
    __non_webpack_require__(entryPath);
});
