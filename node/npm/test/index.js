const { app } = require('deskgap');
const assert = require('assert');
const fs = require('fs');

app.once('ready', () => {
	console.log('index.js: DeskGap app ready.');
	assert.strictEqual(fs.readFileSync(process.env['DESKGAP_NPM_TEST_VERSION_FILE'], 'utf8'), process.versions.deskgap)
	app.exit();
});
