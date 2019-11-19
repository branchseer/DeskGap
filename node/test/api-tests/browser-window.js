const { app, BrowserWindow } = require('deskgap');
const chai = require('chai');
const path = require('path');

const { expect } = chai;

const mac = process.platform === 'darwin';

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
            win.loadFile(path.resolve(__dirname, '..', 'fixtures', 'files', 'blank.html'));
            win.destroy();
            expect(win.isDestroyed()).to.equal(true);
        })
    });
    describe('win.setMenu(menu)', () => {
        it('should not throw when it is called after the window has been shown', function () {
            if (mac) return this.skip();
            const win = new BrowserWindow();
            win.setMenu(null);
            win.destroy();
        });
        it('should not throw when it is called before the window has been shown', function () {
            if (mac) return this.skip();
            const win = new BrowserWindow({ show: false });
            win.setMenu(null);
            win.show();
            win.destroy();
        });
    });
    describe('win.setTitleBarStyle(style)', () => {
        before(function() {
            if (process.platform !== 'darwin') this.skip();
        });

        it('should not change the frame of the window', () => {
            const win = new BrowserWindow({ show: false });
            const size = win.getSize();
            const position = win.getPosition();

            for (const style of ['hidden', 'hiddenInset', 'default']) {
                win.setTitleBarStyle(style);
                expect(win.getSize()).to.deep.equal(size);
                expect(win.getPosition()).to.deep.equal(position);
            }
        });
    });
});
