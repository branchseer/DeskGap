const { app, BrowserWindow, MenuItem, Menu, ipcMain, systemPreferences, dialog } = require('deskgap');

ipcMain.on('hello-to-node', (e, message) => {
    e.sender.send('hello-from-node', "Hello, " + message);
});

let mainWindow;

app.once('ready', () => {
    mainWindow = new BrowserWindow({
        show: false,
        width: 800, height: 600,
    }).once('ready-to-show', () => {
        mainWindow.show();
    });

    if (process.platform !== 'win32') {
        mainWindow.webView.setDevToolsEnabled(true);
    }

    for (const eventName of ['blur', 'focus']) {
        mainWindow.on(eventName, () => { mainWindow.webView.send('window-' + eventName) })
    }

    mainWindow.loadFile("app.html");

    mainWindow.on('closed', () => {
        mainWindow = null;
    });
});
