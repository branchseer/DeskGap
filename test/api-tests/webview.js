const { app, BrowserWindow, messageNode } = require('deskgap');
const { expect } = require('chai');
const { createLocalServer } = require('../utils');
const { once } = require('events');
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
        it('loads the page by requesting the url', async function() {
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
        it('loads the given file', (done) => {
            win.webView.loadFile(path.resolve(__dirname, '..', 'fixtures', 'files', 'web-view-load-file.html'));
            messageNode.once('hello-web-view-load-file', () => done());
        })
    });

    describe('webView.executeJavaScript(code)', () => {
        beforeEach(async () => {
            win.loadFile(path.resolve(__dirname, '..', 'fixtures', 'files', 'blank.html'));
            await once(win.webView, 'did-finish-load');
        });
        it('returns a promise of the evaluated value', async () => {
            const result = await win.webView.executeJavaScript("{ answer: 40+2 }");
            expect(result).to.deep.equal({ answer: 42 });
        });

        it('returns a promise that resolves to the result of the evaluated promise', async () => {
            const result = await win.webView.executeJavaScript("Promise.resolve({ answer: 40+2 })");
            expect(result).to.deep.equal({ answer: 42 });
        });

        it('returns a promise that rejects when there is a syntax error', async () => {
            const resultPromise = win.webView.executeJavaScript("nonsense");
            await expect(resultPromise).to.eventually.rejected;
        });

        it('returns a promise that rejects when the code throws an error synchronously', async () => {
            const resultPromise = win.webView.executeJavaScript("(function(){ throw new Error(); })()");
            await expect(resultPromise).to.eventually.rejected;
        });

        it('returns a promise that rejects when the code is evaluated to a promise that rejects', async () => {
            const resultPromise = win.webView.executeJavaScript("Promise.reject(new Error())");
            await expect(resultPromise).to.eventually.rejected;
        });
    });
});
