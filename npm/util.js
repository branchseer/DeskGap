const { spawn } = require('child_process');
const path = require('path');
const nugget = require('nugget');
const crypto = require('crypto');
const fs = require('fs');

exports.downloadFile = (url, target) => new Promise((resolve, reject) => {
    nugget(url, { target }, (errors) => {
        if (errors) {
            reject(errors[0]);
        }
        else {
            resolve();
        }
    });
});

exports.runDeskGap = (distPath, entryPath, args) => {
    let executablePath;
    if (process.platform === 'darwin') {
        executablePath = path.join(distPath, 'DeskGap.app/Contents/MacOS/DeskGap');
    }
    else if (process.platform === 'win32') {
        executablePath = path.join(distPath, 'DeskGap/DeskGap.exe');
    }

    const deskgapProcess = spawn(executablePath, args, {
        stdio: 'inherit',
        windowsHide: false,
        env: {
            'DESKGAP_ENTRY': path.resolve(entryPath)
        }
    });
    
    for (const signal of ['SIGINT', 'SIGTERM']) {
        process.on(signal, () => {
            if (!deskgapProcess.killed) {
                deskgapProcess.kill(signal);
            }
        })
    }

    deskgapProcess.once('close', (code) => process.exit(code));
};

exports.sha256OfPath = (path) => {
    return new Promise((resolve, reject) => {
        fs.createReadStream(path)
            .once('error', reject)
            .pipe(crypto.createHash('sha256'))
            .on('readable', function () {
                const data = this.read();
                if (data) {
                    resolve(data.toString('hex'));
                }
            });
    });
};
