const { app, BrowserWindow } = require('deskgap');
const chai = require('chai');

const { expect } = chai;

describe('BrowserWindow module', () => {
    const windowAllClosedHandler = () => {};
    
    before(async () => {
        app.on('window-all-closed', windowAllClosedHandler);
        await app.whenReady();
    });
    after(() => {
        app.removeListener('window-all-closed', windowAllClosedHandler);
    });

    describe('win.close()', () => {
        it('destroys the window if not prevented', () => {
            const win = new BrowserWindow({ show: false });
            expect(win.isDestroyed()).to.equal(false);
            win.close();
            expect(win.isDestroyed()).to.equal(true);
        });
        it('can be prevented in a close event', () => {
            const win = new BrowserWindow({ show: false });
            win.once('close', e => e.preventDefault());
            win.close();
            expect(win.isDestroyed()).to.equal(false);
            win.close();
            expect(win.isDestroyed()).to.equal(true);
        });
    });
    describe('win.destroy()', () => {
        it('destroys the window without emitting a close event', () => {
            const win = new BrowserWindow({ show: false });
            win.on('close', () => { throw new Error(); });
            expect(win.isDestroyed()).to.equal(false);
            win.destroy();
            expect(win.isDestroyed()).to.equal(true);
        });
        it('should not crash when destroying windows with pending events', () => {
            const win = new BrowserWindow({ show: false });
            win.loadHTMLString(`<script type="text/javascript">
                window.deskgap.messageUI.send('a-channel');
            </script>`);
            win.destroy();
            expect(win.isDestroyed()).to.equal(true);
        })
    });
});
