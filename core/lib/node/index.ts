
import { entryPath, NativeException } from './internal/bootstrap';
import app from './app';
import { BrowserWindow } from './browser-window';
import { Menu, MenuItem } from './menu';
import { WebViews, messageNode } from './webview';
import Dialog from './dialog';
import shell from './shell';
import './async-node';
import systemPreferences from './system-preferences';
import os = require('os');



if (process.platform === 'win32') {
    const gte = (release1: number[], release2: number[]): boolean => {
        for (let i = 0; i < release2.length; ++i) {
            const r1 = release1[i];
            const r2 = release2[i];
            if (r1 == null || isNaN(r1) || r1 < r2) {
                return false;
            }
            if (r1 > r2) {
                return true;
            }
        }
        return true;
    }

    const minSupportedRelease = [10, 0, 17763];
    const releaseString = os.release();
    const release = releaseString.split('.').map(n => parseInt(n, 10));
    

    if (!gte(release, minSupportedRelease)) {
        Dialog.showErrorBox(
            'Unsupported OS Version: ' + releaseString,
            "The minimum supported OS version is " + minSupportedRelease.join('.')
        );
        process.exit();
    }
}

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
