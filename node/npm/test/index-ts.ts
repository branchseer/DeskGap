import { app } from 'deskgap';

app.once('ready', () => {
	console.log('index-ts.ts: DeskGap app ready. Exiting...');
	app.exit();
});
