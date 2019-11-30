const { app } = require('deskgap');

app.once('ready', () => {
	console.log('index.js: DeskGap app ready. Exiting...');
	app.exit();
});
