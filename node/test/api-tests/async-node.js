const { app, messageNode } = require('deskgap');
const { once } = require('events');
const { expect } = require('chai');
const { withWebView } = require('../utils');

describe('asyncNode', () => {
    const windowAllClosedHandler = () => {};

    before(async () => {
        app.on('window-all-closed', windowAllClosedHandler);
        await app.whenReady();
    });
    after(() => {
        app.removeListener('window-all-closed', windowAllClosedHandler);
    });

    describe('asyncNode.require(moduleName)', () => {
        withWebView(it, 'can require built-in modules', async function (win) {
            // TODO: This test takes toooooo much time (sometimes over 10s) on win2012r2
            this.timeout(20 * 1000);
            const onceAsyncNodeResult = once(messageNode, 'async-node-result');
            const execution = win.webView.executeJavaScript(`
                deskgap.asyncNode.require('os').then(function (os) {
                    return os.invoke('platform').value();
                }).then(function (result) {
                    deskgap.messageUI.send('async-node-result', result);
                })
            `);
            const [, result] = await onceAsyncNodeResult;
            expect(result).to.equal(require('os').platform());
            await execution;
        }, true);

        withWebView(it, 'should resolve relative paths to the entry of the app', async (win) => {
            const onceAsyncNodeResult = once(messageNode, 'async-node-result');
            const execution = win.webView.executeJavaScript(`
                deskgap.asyncNode.require('./fixtures/modules/async-node-simple-module').then(function(m) {
                    return m.value();
                }).then(function (result) {
                    deskgap.messageUI.send('async-node-result', result);
                })
            `);
            const [, result] = await onceAsyncNodeResult;
            expect(result).to.equal('hello asyncNode');
            await execution;
        }, true);
    });
});
