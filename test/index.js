const Mocha = require('mocha');
const fs = require('fs');
const path = require('path');
const chai = require('chai');
chai.use(require('chai-as-promised'));

const mocha = new Mocha({
    timeout: 5000
});

const testDir = path.join(__dirname, 'api-tests');

const testFiles = fs.readdirSync(testDir)
    .filter(filename => filename.endsWith('.js') && filename.includes('app'))
    .map(filename => path.join(testDir, filename));

for (const file of testFiles) {
    mocha.addFile(file);
}

mocha.run(failures => process.exit(failures ? 1: 0));
