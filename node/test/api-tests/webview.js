const { app, BrowserWindow, messageNode } = require('deskgap');
const { expect } = require('chai');
const { createLocalServer, withWebView } = require('../utils');
const { once } = require('events');
const path = require('path');

describe('BrowserWindow#webView', () => {
    const windowAllClosedHandler = () => {};

    before(async () => {
        app.on('window-all-closed', windowAllClosedHandler);
        await app.whenReady();
    });
    after(() => {
        app.removeListener('window-all-closed', windowAllClosedHandler);
    });

    it('has an alias: BrowserWindow#webContents', () => {
        const win = new BrowserWindow({show:false});
        expect(win.webView).to.equal(win.webContents);
        win.destroy();
    });

    describe('webView.loadURL(url)', () => {
        withWebView(it, 'loads the page by requesting the url', async function(win) {
            if (win.webView.engine === 'winrt') return this.skip();
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
            server.close();
        });
    });

    describe('webView.loadFile(path)', () => {
        withWebView(it, 'loads the given file', (win) => new Promise(resolve =>{
            win.webView.loadFile(path.resolve(__dirname, '..', 'fixtures', 'files', 'web-view-load-file.html'));
            win.webView.publishServices({
                'dgtest': { loaded() { resolve() } }
            })
        }))
    });

    describe.only('webView.getService(services).call(...)', () => {
        withWebView(it, 'calls services published on the browser side', async (win) => {
            win.webView.loadFile(path.resolve(__dirname, '..', 'fixtures', 'files', 'web-view-side-services.html'));
            await once(win.webView, 'did-finish-load');
            const resultFromBrowser = await win.webView.getService('aBrowserService').call('getJSONObject');
            expect(resultFromBrowser).to.eql({ answer: 42 });
        });
    });
});
