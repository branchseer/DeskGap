const { app, BrowserWindow, dialog } = require('deskgap');

app.once('ready', () => {
    const mainWindow = new BrowserWindow({
        show: false,
        width: 800, height: 600,
    }).once('ready-to-show', () => {
        
        for (const eventName of ['blur', 'focus']) {
            mainWindow.on(eventName, () => { 
                mainWindow.webView.getService('').send('windowEvent', eventName);
            })
        }

        mainWindow.show();
    });

    mainWindow.webView.publishServices({
        "dialog": dialog,
        "browserWindow": mainWindow,
        "": {
            execute(code) {
                return (new Function('browserWindow', code))(mainWindow);
            }
        }
    });

    if (process.platform !== 'win32') {
        mainWindow.webView.setDevToolsEnabled(true);
    }

    mainWindow.loadFile("app.html");
});
