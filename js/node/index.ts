import './process';
import NativeException from './native-exception';
import app from './app';
import { BrowserWindow } from './browser-window';
import { Menu, MenuItem } from './menu';
import { WebViews } from './webview';
import messageNode from './message-node';
import Dialog from './dialog';
import shell from './shell';
import './internal/async-node';
import systemPreferences from './system-preferences';
import { registerModule } from './internal/cjs-intercept';

const deskgap = {
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

export = deskgap;
registerModule(deskgap);

process.on('uncaughtException', (error) => {
    if (process.listeners('uncaughtException').length > 1) {
        return;
    }
    const message = error.stack || `${error.name}: ${error.message}`;
    console.error('Uncaught exception', message);
    Dialog.showErrorBox('Uncaught exception', message);
    process.exit(1);
});

app['run_']();

