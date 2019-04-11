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
        withWebView(it, 'loads the given file', async (win) => {
            win.webView.loadFile(path.resolve(__dirname, '..', 'fixtures', 'files', 'web-view-load-file.html'));
            await once(messageNode, 'hello-web-view-load-file');
        })
    });

    describe('webView.executeJavaScript(code)', () => {
        withWebView(it, 'returns a promise of the evaluated value', async (win) => {
            const result = await win.webView.executeJavaScript("{ answer: 40+2 }");
            expect(result).to.deep.equal({ answer: 42 });
        }, true);

        withWebView(it, 'returns a promise that resolves to the result of the evaluated promise', async (win) => {
            const result = await win.webView.executeJavaScript("Promise.resolve({ answer: 40+2 })");
            expect(result).to.deep.equal({ answer: 42 });
        }, true);

        withWebView(it, 'returns a promise that rejects when there is a syntax error', async (win) => {
            const resultPromise = win.webView.executeJavaScript("nonsense");
            await expect(resultPromise).to.eventually.rejected;
        }, true);

        withWebView(it, 'returns a promise that rejects when the code throws an error synchronously', async (win) => {
            const resultPromise = win.webView.executeJavaScript("(function(){ throw new Error(); })()");
            await expect(resultPromise).to.eventually.rejected;
        }, true);

        withWebView(it, 'returns a promise that rejects when the code is evaluated to a promise that rejects', async (win) => {
            const resultPromise = win.webView.executeJavaScript("Promise.reject(new Error())");
            await expect(resultPromise).to.eventually.rejected;
        }, true);
    });
});
