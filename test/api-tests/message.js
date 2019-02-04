const { app, BrowserWindow, messageNode } = require('deskgap');
const { expect } = require('chai');

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
        it('should send messages from ui to node with e.sender being the webview', (done) => {
            win.loadHTMLString(`<script type='text/javascript'>
                window.deskgap.messageUI.send('messageui-send');
            </script>`);
            messageNode.once('messageui-send', (e) => {
                expect(e.sender).to.equal(win.webView);
                done();
            });
        });

        it('can send messages with primitive args', (done) => {
            win.loadHTMLString(`<script type='text/javascript'>
                window.deskgap.messageUI.send('messageui-send-primitive-args', 3, 'str', null, false);
            </script>`);
            messageNode.once('messageui-send-primitive-args', (e, ...args) => {
                expect(args).to.deep.equal([3, 'str', null, false]);
                done();
            });
        });

        it('can send messages with json object args', (done) => {
            win.loadHTMLString(`<script type='text/javascript'>
                window.deskgap.messageUI.send('messageui-send-json-args', { a: 1 });
            </script>`);
            messageNode.once('messageui-send-json-args', (e, jsonObject) => {
                expect(jsonObject).to.deep.equal({a: 1});
                done();
            });
        });
    });

    describe('messageUI.once', () => {
        beforeEach(() => {
            win.loadHTMLString(`<script type='text/javascript'>
                window.deskgap.messageUI.once('webview-echo', function() {
                    var args = Array.prototype.slice.call(arguments, 1);
                    window.deskgap.messageUI.send.apply(
                        null,
                        ['webview-echo-reply'].concat(args)
                    );
                });
            </script>`);
        });

        it('should receives messages sent from node', (done) => {
            win.webView.once('did-finish-load', () => {
                win.webView.send('webview-echo');
            });

            messageNode.once('webview-echo-reply', (e) => {
                expect(e.sender).to.equal(win.webView);
                done();
            });
        });

        it('can receives messages with primitive args', (done) => {
            win.webView.once('did-finish-load', () => {
                win.webView.send('webview-echo', 1, null, 'str', false);
            });

            messageNode.once('webview-echo-reply', (e, ...args) => {
                expect(args).to.deep.equal([1, null, 'str', false]);
                done();
            });
        });

        it('should send messages with json object args', (done) => {
            win.webView.once('did-finish-load', () => {
                win.webView.send('webview-echo', { a: 1 });
            });

            messageNode.once('webview-echo-reply', (e, jsonObject) => {
                expect(jsonObject).to.deep.equal({ a: 1 });
                done();
            });
        });
    });

});
