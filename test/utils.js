const path = require('path');
const Koa = require('koa');
const { spawn } = require('child_process');

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
