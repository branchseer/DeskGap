import path = require('path');

//The host program passed [argv[0], deskgap lib path, entry path, ...restOfArgv] to node::Start()
export const entryPath = process.argv.splice(1, 2)[1];

//Resolves from libPath/dist/node/paths.js
export const libPath = path.resolve(__dirname, '..', '..');
