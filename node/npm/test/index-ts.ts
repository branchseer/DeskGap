import { app } from 'deskgap';
import assert = require('assert');
import fs = require('fs');

app.once('ready', () => {
	console.log('index-ts.ts: DeskGap app ready.');
	assert.strictEqual(fs.readFileSync(process.env['DESKGAP_NPM_TEST_VERSION_FILE'], 'utf8'), process.versions.deskgap)
	app.exit();
});
