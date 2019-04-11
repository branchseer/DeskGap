const path = require('path');
const Koa = require('koa');
const { webViews, BrowserWindow } = require('deskgap');
const { spawn } = require('child_process');
const { once } = require('events');


class DeskGapProcessError extends Error {
    constructor(result) {
        super('The DeskGap instance exited with a non-zero status');
        this.result = result;
    }
}

const spawnDeskGapAsync = (entryPath, args) => {
    const spawnedDeskGap = spawn(process.argv0, args, {
        windowsHide: false,
        env: {
            ...process.env,
            'DESKGAP_ENTRY': entryPath
        }
    });

    const stdoutBuffers = [];
    const stderrBuffers = [];
    spawnedDeskGap.stdout.on('data', chunk => stdoutBuffers.push(chunk));
    spawnedDeskGap.stderr.on('data', chunk => stderrBuffers.push(chunk));

    return new Promise((resolve, reject) => {
        spawnedDeskGap.once('error', reject);
        spawnedDeskGap.once('close', (code, signal) => {
            const result = {
                stdout: Buffer.concat(stdoutBuffers).toString('utf8'),
                stderr: Buffer.concat(stderrBuffers).toString('utf8'),
                code, signal
            };
            if (code !== 0) {
                reject(new DeskGapProcessError(result));
            }
            else {
                resolve({
                    stdout: Buffer.concat(stdoutBuffers).toString('utf8'),
                    stderr: Buffer.concat(stderrBuffers).toString('utf8'),
                    code, signal
                });
            }
        });
    });
}

exports.spawnDeskGapAppAsync = (appName, ...args) => {
    return spawnDeskGapAsync(path.join(__dirname, 'fixtures', 'apps', appName), args);
};

exports.createLocalServer = (handlers) => {
    const koa = new Koa();

    koa.use(async (ctx) => {
        const handler = handlers[ctx.path];
        if (handler == null) {
            ctx.status = 404;
            return;
        }
        await handler(ctx);
    });
    
    return new Promise((resolve, reject) => {
        koa.once('error', reject);
        koa.listen(0, 'localhost', function() {
            resolve({
                url: `http://localhost:${this.address().port}`,
                close: () => {
                    this.close();
                },
                whenClose: () => {
                    return new Promise(r => this.on('close', r));
                }
            });
        });
    });
}

const engines = webViews.isEngineAvaliable('winrt') ? ['trident', 'winrt']: [ null ];
exports.withWebView = (it, description, func, loadsBlankPage = false) => {
    for (const engine of engines) {
        it(description + (engine == null ? "": `@${engine}`), async function() {
            const win = new BrowserWindow({
                show: false,
                webPreferences: { engine }
            });
            if (loadsBlankPage) {
                win.loadFile(path.resolve(__dirname, 'fixtures', 'files', 'blank.html'));
                await once(win.webView, 'did-finish-load');
            }
            try {
                return await func.call(this, win);
            }
            finally {
                win.destroy();
            }
        });
    }
};
