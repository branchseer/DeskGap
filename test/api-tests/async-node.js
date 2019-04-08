const { app, BrowserWindow, messageNode } = require('deskgap');
const path = require('path');
const { once } = require('events');
const { expect } = require('chai');

describe('asyncNode', () => {
    const windowAllClosedHandler = () => {};
    
    let win;
    beforeEach(() => {
        win = new BrowserWindow({ show: false });
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

    describe('asyncNode.require(moduleName)', () => {
        beforeEach(async () => {
            win.loadFile(path.resolve(__dirname, '..', 'fixtures', 'files', 'blank.html'));
            await once(win.webView, 'did-finish-load');
        });

        it('can require built-in modules', async () => {
            win.webView.executeJavaScript(`
                deskgap.asyncNode.require('os').then(function (os) {
                    return os.invoke('platform').value();
                }).then(function (result) {
                    deskgap.messageUI.send('async-node-result', result);
                })
            `);
            const [, result] = await once(messageNode, 'async-node-result');
            expect(result).to.equal(require('os').platform());
        });

        it('should resolve relative paths to the entry of the app', async () => {
            win.webView.executeJavaScript(`
                deskgap.asyncNode.require('./fixtures/modules/async-node-simple-module').then(function(m) {
                    return m.value();
                }).then(function (result) {
                    deskgap.messageUI.send('async-node-result', result);
                })
            `);
            const [, result] = await once(messageNode, 'async-node-result');
            expect(result).to.equal('hello asyncNode');
        });
    });
});
