const { runDeskGap } = require('../npm/util');
const path = require('path');

const buildPath = require('../scripts/build-folder');
let distPath = path.resolve(buildPath, 'Release');

runDeskGap(distPath, __dirname, []);
 