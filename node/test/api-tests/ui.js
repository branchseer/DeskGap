const { app } = require('deskgap');
const { expect } = require('chai');
const { withWebView } = require('../utils');
const { once } = require('events');
const path = require('path');

describe('UI Thread', () => {
    const windowAllClosedHandler = () => {};

    before(async () => {
        app.on('window-all-closed', windowAllClosedHandler);
        await app.whenReady();
    });
    after(() => {
        app.removeListener('window-all-closed', windowAllClosedHandler);
    });

    describe("location.reload()", function () {
        withWebView(it, 'should reload the page', async (win) => {
           win.webView.executeJavaScript("location.reload()");
           await once(win.webView, 'did-finish-load');
        }, true);
    });
});
