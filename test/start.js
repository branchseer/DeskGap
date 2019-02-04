const runDeskGap = require('../npm/run');
const path = require('path');
const { spawnDeskGapAsync } = require('./utils');


const buildPath = require('../scripts/build-folder');
let distPath = path.resolve(buildPath, 'Release');


runDeskGap(distPath, __dirname, []);
