const { app, BrowserWindow, messageNode } = require('deskgap');
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
        it('can require built-in modules', (done) => {
            win.loadHTMLString(`<script type='text/javascript'>
                deskgap.asyncNode.require('os').then(function (os) {
                    return os.invoke('platform').value()
                }).then(function (result) {
                    deskgap.messageUI.send('async-node-result', result);
                });
            </script>`);
            messageNode.once('async-node-result', (e, result) => {
                expect(result).to.equal(require('os').platform());
                done();
            });
        });

        it('should resolve relative paths to the entry of the app', (done) => {
            win.loadHTMLString(`<script type='text/javascript'>
                deskgap.asyncNode.require('./fixtures/modules/async-node-simple-module').then(function(m) {
                    return m.value();
                }).then(function (result) {
                    deskgap.messageUI.send('async-node-result', result);
                });
            </script>`);
            messageNode.once('async-node-result', (e, result) => {
                expect(result).to.equal('hello asyncNode');
                done();
            });
        });
    });
});
