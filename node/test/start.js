const runDeskGap = require('../npm/run');
const path = require('path');

const distPath = process.argv[2];

if (distPath == null) {
    console.error("Missing [deskgap-dist-path]");
    console.error('Usage: node start.js [deskgap-dist-path]');
    process.exit(1);
}
else {
    runDeskGap(distPath, __dirname, []);
}
