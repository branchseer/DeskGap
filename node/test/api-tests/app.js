const { app } = require('deskgap');
const chai = require('chai');
const { spawnDeskGapAppAsync } = require('../utils');

const { expect } = chai;

describe('process', () => {
    describe('process.argv', () => {
        it('returns an array containing the command line arguments passed to the app', async () => {
            const result = await spawnDeskGapAppAsync('arbitrary-code', `
                process.stdout.write(process.argv[2] + process.argv[3]);
                process.exit();
            `, 'hello', '你好');
            expect(result.stdout).to.equal('hello你好');
        });
    });
});

describe('app module', () => {
    describe('app.getVersion', () => {
        it('returns the version field of package.json', () => {
            expect(app.getVersion()).to.equal('0.0.1')
        })
    });
    describe('app.setVersion(version)', () => {
        it('overrides the version', () => {
            expect(app.getVersion()).to.equal('0.0.1')
            app.setVersion('test-version')

            expect(app.getVersion()).to.equal('test-version')
            app.setVersion('0.0.1')
        })
    });

    describe('app.getName()', () => {
        it('returns the name field of package.json if productName does not exists', () => {
            expect(app.getName()).to.equal('DeskGap Test');
        });
        it('returns the productName field of package.json if both name and productName exists', async () => {
            const spawnResult = await spawnDeskGapAppAsync('app-with-product-name');
            expect(spawnResult.stdout).to.equal('package.productName');
        });
    });

    describe('app.setName(name)', () => {
        it('overrides the name', () => {
            expect(app.getName()).to.equal('DeskGap Test')
            app.setName('test-name')
        
            expect(app.getName()).to.equal('test-name')
            app.setName('DeskGap Test')
        })
    });

    describe('app.exit(code)', () => {
        it('emits a process exit event with the code', async () => {
            const error = await spawnDeskGapAppAsync('arbitrary-code', `
                const { app } = require('deskgap');
                app.on('ready', () => {
                    app.exit(123);
                });
                process.on('exit', code => {
                    process.stdout.write('Exit event with code: ' + code);
                });
            `).catch(e => e);
            expect(error).to.be.an.instanceof(Error);
            expect(error.result.code).to.equal(123);
            expect(error.result.stdout).to.equal('Exit event with code: 123');
        });

        it('emits a quit event but the before-quit and will-quit events will not be emitted', async () => {
            const result = await spawnDeskGapAppAsync('arbitrary-code', `
                const { app } = require('deskgap');
                app.on('quit', () => process.stdout.write('quit'));
                app.on('before-quit', () => process.stdout.write('before-quit'));
                app.on('will-quit', () => process.stdout.write('will-quit'));
                app.on('ready', () => {
                    app.exit();
                });
            `);
            expect(result.stdout).to.equal('quit');
        });

        it('closes all windows without asking', async () => {
            const result = await spawnDeskGapAppAsync('arbitrary-code', `
                const { app, BrowserWindow } = require('deskgap');
                const windows = [];

                const createWindow = () => {
                    const window = new BrowserWindow({ show: false });
                    window.on('close', (e) => {
                        process.stdout.write('Please do not close');
                    });
                    windows.push(window);
                };

                app.once('ready', () => {
                    for (let i = 1; i <= 5; i++) {
                        createWindow()
                    }
                    app.exit();
                });
            `);
            expect(result.stdout).to.equal("");
        });
    });

    describe('app.whenReady', () => {
        it('returns a Promise', () => {
          expect(app.whenReady()).to.be.instanceof(Promise);
        });
    
        it('becomes fulfilled if the app is already ready', () => {
          expect(app.isReady()).to.equal(true);
          return expect(app.whenReady()).to.be.eventually.fulfilled;
        });
    });

    describe('window-all-closed event', () => {
        it('not having any subscriber will cause the app quitting when all window closed', async () => {
            const result = await spawnDeskGapAppAsync('arbitrary-code', `
                const { app, BrowserWindow } = require('deskgap');
                const windows = [];
                app.once('ready', () => {
                    windows.push(new BrowserWindow({ show: false}));
                    windows.push(new BrowserWindow({ show: false}));
                    windows.forEach(w => w.close());
                    process.stdout.write('after all window closed');
                });
            `);
            expect(result.stdout).to.equal('');
        });
        it('prevents app to be closed automatically when all windows closed if there is any subscriber', async () => {
            const result = await spawnDeskGapAppAsync('arbitrary-code', `
                const { app, BrowserWindow } = require('deskgap');
                const windows = [];
                app.once('ready', () => {
                    windows.push(new BrowserWindow({ show: false}));
                    windows.push(new BrowserWindow({ show: false}));
                    app.once('window-all-closed', () => {
                        process.stdout.write('emitted');
                    });
                    windows.forEach(w => w.close());
                    process.stdout.write(' prevented');
                    new BrowserWindow({ show: false }).close();
                    process.stdout.write('quitted');
                });
            `);
            expect(result.stdout).to.equal('emitted prevented');
        });
    });

    describe('app.quit()', () => {
        it('emits a before-quit event, windows’ close events, a will-quit event and a quit event', async () => {
            const result = await spawnDeskGapAppAsync('arbitrary-code', `
                const { app, BrowserWindow } = require('deskgap');
                const windows = [];
                app.once('ready', () => {
                    app.once('before-quit', () => process.stdout.write('0'));
                    for (let i = 1; i <= 2; i++) {
                        const window = new BrowserWindow({ show: false });
                        window.on('close', () => process.stdout.write(i.toString()));
                    }
                    app.once('will-quit', () => process.stdout.write('3'));
                    app.once('quit', () => process.stdout.write('4'));

                    app.quit();
                });
            `);
            expect(result.stdout).to.equal("01234");
        })

        it('does not try to close windows if prevented in before-quit', async () => {
            const result = await spawnDeskGapAppAsync('arbitrary-code', `
                const { app, BrowserWindow } = require('deskgap');
                const windows = [];
                app.once('ready', () => {
                    app.once('before-quit', (e) => {
                        process.stdout.write('preventing close,');
                        e.preventDefault();
                    });
                    windows.push(new BrowserWindow({ show: false }).on('close', (e) => {
                        process.stdout.write('closing window');
                    }));
                    app.quit();
                    app.quit();
                });
            `);
            expect(result.stdout).to.equal('preventing close,closing window');
        });

        it('does not quit the app if prevented by one of windows', async () => {
            const result = await spawnDeskGapAppAsync('arbitrary-code', `
                const { app, BrowserWindow } = require('deskgap');
                const windows = [];
                app.once('ready', () => {
                    windows.push(new BrowserWindow({ show: false }));
                    windows.push(new BrowserWindow({ show: false }).once('close', (e) => {
                        process.stdout.write('preventing close');
                        e.preventDefault();
                    }));
                    app.quit();
                    app.quit();
                });
            `);
            expect(result.stdout).to.equal('preventing close');
        });

        it('closes all windows but not quit the app if prevented in will-quit', async () => {
            const result = await spawnDeskGapAppAsync('arbitrary-code', `
                const { app, BrowserWindow } = require('deskgap');
                const windows = [];
                app.once('ready', () => {
                    windows.push(new BrowserWindow({ show: false }).once('close', (e) => {
                        process.stdout.write('closing window,');
                    }));
                    app.once('will-quit', e => {
                        process.stdout.write('preventing close');
                        e.preventDefault();
                    });
                    app.quit();
                    app.quit();
                });
            `);
            expect(result.stdout).to.equal('closing window,preventing close');
        });

        it('cannot be prevented in quit events', async () => {
            const result = await spawnDeskGapAppAsync('arbitrary-code', `
                const { app, BrowserWindow } = require('deskgap');
                app.once('ready', () => {
                    app.on('quit', e => {
                        e.preventDefault();
                    });
                    app.quit();
                });
            `);
            expect(result.stdout).to.equal('');
        });
    });
});
