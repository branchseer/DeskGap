const { spawn } = require('child_process');
const path = require('path');
const fs = require('fs');

module.exports = (distPath, entryPath, args) => {
    let executablePath;
    if (process.platform === 'darwin') {
        executablePath = path.join(distPath, 'DeskGap.app/Contents/MacOS/DeskGap');
    }
    else if (process.platform === 'win32') {
        executablePath = path.join(distPath, 'DeskGap/DeskGap.exe');
    }
    else if (process.platform === 'linux') {
        executablePath = path.join(distPath, 'DeskGap/DeskGap');
    }

    const deskgapProcess = spawn(path.resolve(executablePath), args, {
        stdio: 'inherit',
        windowsHide: false,
        env: {
            ...process.env,
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
