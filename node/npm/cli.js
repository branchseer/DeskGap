#!/usr/bin/env node

const runDeskGap = require('./run');
const path = require('path');

const distPath = process.env.DESKGAP_DIST_PATH || path.join(__dirname, 'dist');

runDeskGap(distPath, process.argv[2], process.argv.slice(3));
