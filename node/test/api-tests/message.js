const { app, messageNode } = require('deskgap');
const { expect } = require('chai');
const { once } = require('events');
const { withWebView } = require('../utils');


describe('messageUI and messageNode', () => {
    const windowAllClosedHandler = () => {};

    before(async () => {
        app.on('window-all-closed', windowAllClosedHandler);
        await app.whenReady();
    });
    after(() => {
        app.removeListener('window-all-closed', windowAllClosedHandler);
    });

    describe('messageUI.send', () => {

        withWebView(it, 'should send messages from ui to node with e.sender being the webview', async (win) => {
            const execution = win.webView.executeJavaScript(`window.deskgap.messageUI.send('messageui-send')`);
            const [e, ] = await once(messageNode, 'messageui-send');
            expect(e.sender).to.equal(win.webView);
            await execution;
        }, true);

        withWebView(it, 'can send messages with primitive args', async (win) => {
            const execution = win.webView.executeJavaScript(`window.deskgap.messageUI.send('messageui-send-primitive-args', 3, 'str', null, false)`);
            const [, ...args] = await once(messageNode, 'messageui-send-primitive-args');
            expect(args).to.deep.equal([3, 'str', null, false]);
            await execution;
        }, true);

        withWebView(it, 'can send messages with json object args', async (win) => {
            const execution = win.webView.executeJavaScript(`window.deskgap.messageUI.send('messageui-send-json-args', { a: 1 })`);
            const [, jsonObject] = await once(messageNode, 'messageui-send-json-args');
            expect(jsonObject).to.deep.equal({a: 1});
            await execution;
        }, true);
    });

    describe('messageUI.once', () => {
        const registerEchoHandler = async (win) => {
            await win.webView.executeJavaScript(`
                window.deskgap.messageUI.once('webview-echo', function() {
                    var args = Array.prototype.slice.call(arguments, 1);
                    window.deskgap.messageUI.send.apply(
                        null,
                        ['webview-echo-reply'].concat(args)
                    );
                })
            `);
        }

        withWebView(it, 'should receives messages sent from node', async (win) => {
            await registerEchoHandler(win);
            win.webView.send('webview-echo');
            const [e, ] = await once(messageNode, 'webview-echo-reply');
            expect(e.sender).to.equal(win.webView);
        }, true);

        withWebView(it, 'can receives messages with primitive args', async (win) => {
            await registerEchoHandler(win);
            win.webView.send('webview-echo', 1, null, 'str', false);
            const [, ...args] = await once(messageNode, 'webview-echo-reply');
            expect(args).to.deep.equal([1, null, 'str', false]);
        }, true);

        withWebView(it, 'should send messages with json object args', async (win) => {
            await registerEchoHandler(win);
            win.webView.send('webview-echo', { a: 1 });
            const [, jsonObject] = await once(messageNode, 'webview-echo-reply');
            expect(jsonObject).to.deep.equal({ a: 1 });
        }, true);
    });
});
