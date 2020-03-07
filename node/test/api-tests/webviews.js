const { webViews, BrowserWindow } = require('deskgap');
const { expect } = require('chai');
const { once } = require('events');
const isElevated = require('is-elevated');
const os = require('os');
const path = require('path');

const win = process.platform === 'win32';
const win1809 = win && (() => {
    const gte = (release1, release2) => {
        for (let i = 0; i < release2.length; ++i) {
            const r1 = release1[i];
            const r2 = release2[i];
            if (r1 == null || isNaN(r1) || r1 < r2) {
                return false;
            }
            if (r1 > r2) {
                return true;
            }
        }
        return true;
    }
    const releaseString = os.release();
    const release = releaseString.split('.').map(n => parseInt(n, 10));
    return gte(release, [10, 0, 17763]);
});

describe('webViews', () => {
    describe('webViews.isEngineAvailable(engine)', () => {
        let isAdmin;
        before(async () => {
            isAdmin = await isElevated();
        });
        it('should return false if the os is not Windows', function () {
            if (win) return this.skip();
            for (const engine of ["winrt", "trident"]) {
                expect(webViews.isEngineAvailable(engine)).to.be.false;
            }
        });

        it('should return true for "trident" on Windows', function() {
            if (!win) return this.skip();
            expect(webViews.isEngineAvailable('trident')).to.be.true;
        });

        it('should return true for "winrt" if the Windows version is 10.0.17763 or higher and the user is not Administrator', function() {
            if (!win1809 || isAdmin) return this.skip();
            expect(webViews.isEngineAvailable('winrt')).to.be.true;
        });
            
            
        it('should return false for "winrt" if the Windows version is lower that 10.0.17763 or the user is Administrator', function() {
            if (win && (!win1809 || isAdmin)) {
                expect(webViews.isEngineAvailable('winrt')).to.be.false;
            }
            else {
                this.skip();
            }
        });
    });

    describe('webViews.getDefaultEngine()', () => {
        it('should return null if the os is not Windows', function () {
            if (win) return this.skip();
            expect(webViews.getDefaultEngine()).to.be.null;
        });

        it('should initially return "winrt" on Windows if the WinRT engine is supported', function () {
            if (!webViews.isEngineAvailable('winrt')) return this.skip();
            expect(webViews.getDefaultEngine()).to.equal('winrt');
        });

        it('should initially return "trident" on Windows if the WinRT engine is not supported', function () {
            if (!win || webViews.isEngineAvailable('winrt')) return this.skip();
            expect(webViews.getDefaultEngine()).to.equal("trident");
        });
    });

    describe('webViews.setDefaultEngine(engine)', () => {
        let initialEngine = null;
        beforeEach(() => {
            initialEngine = webViews.getDefaultEngine();
        });
        afterEach(() => {
            webViews.setDefaultEngine(initialEngine);
        });

        it('should change the return value of getDefaultEngine()', () => {
            for (const engine of ["winrt", "trident"]) {
                webViews.setDefaultEngine(engine);
                expect(webViews.getDefaultEngine()).to.equal(engine);
            }
        });

        it('should change the engine of webviews created afterwards to Trident if "trident" is passed', async function() {
            if (!webViews.isEngineAvailable('trident')) return this.skip();
            webViews.setDefaultEngine('trident');
            const window = new BrowserWindow({ show: false });
            window.loadFile(path.resolve(__dirname, '..', 'fixtures', 'files', 'web-view-ua-service.html'));
            await once(window.webView, 'did-finish-load');
            const userAgent = await window.webView.getService('').call('ua');
            expect(userAgent).to.include('Trident');
        });

        it('should change the engine of webviews created afterwards to a WebKit-like one if "winrt" is passed', async function() {
            if (!webViews.isEngineAvailable('winrt')) return this.skip();

            webViews.setDefaultEngine('winrt');
            const window = new BrowserWindow({ show: false });
            window.loadFile(path.resolve(__dirname, '..', 'fixtures', 'files', 'web-view-ua-service.html'));
            await once(window.webView, 'did-finish-load');
            const userAgent = await window.webView.getService('').call('ua');
            expect(userAgent).to.include('WebKit');
        });
        
    });

});
