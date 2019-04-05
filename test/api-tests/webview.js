const { app, BrowserWindow, messageNode } = require('deskgap');
const { expect } = require('chai');
const { createLocalServer } = require('../utils');
const path = require('path');

describe('BrowserWindow#webView', () => {
    const windowAllClosedHandler = () => {};
    
    let win;
    beforeEach(() => {
        win = new BrowserWindow({show:false});
    });
    afterEach(() => {
        win.destroy();
        win = null;
    });

    before(async () => {
        app.on('window-all-closed', windowAllClosedHandler);
        await app.whenReady();
    });
    after(() => {
        app.removeListener('window-all-closed', windowAllClosedHandler);
    });

    it('has an alias: BrowserWindow#webContents', () => {
        expect(win.webView).to.equal(win.webContents);
    });

    describe('webView.loadURL(url)', () => {
        if (process.platform !== 'win32') it('loads the page by requesting the url', async () => {
            let resolve;
            let requested = false;
            const server = await createLocalServer({
                '/index.html': async ctx => {
                    requested = true;
                    resolve();
                }
            });
            win.webView.loadURL(server.url + '/index.html');
            await new Promise(r => resolve = r);
            expect(requested).to.equal(true);
        });
    });

    describe('webView.loadFile(path)', () => {
        it('loads the give file', (done) => {
            win.webView.loadFile(path.resolve(__dirname, '..', 'fixtures', 'files', 'web-view-load-file.html'));
            messageNode.once('hello-web-view-load-file', () => done());
        })
    });
});
