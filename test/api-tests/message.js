const { app, BrowserWindow, messageNode } = require('deskgap');
const { expect } = require('chai');
const path = require('path');
const { once } = require('events');

describe('messageUI and messageNode', () => {
    const windowAllClosedHandler = () => {};
    
    let win;
    beforeEach(() => {
        win = new BrowserWindow({show: false});
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

    describe('messageUI.send', () => {
        beforeEach(async () => {
            win.loadFile(path.resolve(__dirname, '..', 'fixtures', 'files', 'blank.html'));
            await once(win.webView, 'did-finish-load');
        });

        it('should send messages from ui to node with e.sender being the webview', async () => {
            const execution = win.webView.executeJavaScript(`window.deskgap.messageUI.send('messageui-send')`);
            const [e, ] = await once(messageNode, 'messageui-send');
            expect(e.sender).to.equal(win.webView);
            await execution;
        });

        it('can send messages with primitive args', async () => {
            const execution = win.webView.executeJavaScript(`window.deskgap.messageUI.send('messageui-send-primitive-args', 3, 'str', null, false)`);
            const [, ...args] = await once(messageNode, 'messageui-send-primitive-args');
            expect(args).to.deep.equal([3, 'str', null, false]);
            await execution;
        });

        it('can send messages with json object args', async () => {
            const execution = win.webView.executeJavaScript(`window.deskgap.messageUI.send('messageui-send-json-args', { a: 1 })`);
            const [, jsonObject] = await once(messageNode, 'messageui-send-json-args');
            expect(jsonObject).to.deep.equal({a: 1});
            await execution;
        });
    });

    describe('messageUI.once', () => {
        beforeEach(async () => {
            win.loadFile(path.resolve(__dirname, '..', 'fixtures', 'files', 'blank.html'));
            await once(win.webView, 'did-finish-load');
            await win.webView.executeJavaScript(`
                window.deskgap.messageUI.once('webview-echo', function() {
                    var args = Array.prototype.slice.call(arguments, 1);
                    window.deskgap.messageUI.send.apply(
                        null,
                        ['webview-echo-reply'].concat(args)
                    );
                })
            `);
        });

        it('should receives messages sent from node', (done) => {
            win.webView.send('webview-echo');

            messageNode.once('webview-echo-reply', (e) => {
                expect(e.sender).to.equal(win.webView);
                done();
            });
        });

        it('can receives messages with primitive args', (done) => {
            win.webView.send('webview-echo', 1, null, 'str', false);

            messageNode.once('webview-echo-reply', (e, ...args) => {
                expect(args).to.deep.equal([1, null, 'str', false]);
                done();
            });
        });

        it('should send messages with json object args', (done) => {
            win.webView.send('webview-echo', { a: 1 });

            messageNode.once('webview-echo-reply', (e, jsonObject) => {
                expect(jsonObject).to.deep.equal({ a: 1 });
                done();
            });
        });
    });

});
