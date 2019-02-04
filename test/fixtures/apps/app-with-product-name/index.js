const { app } = require('deskgap');

process.stdout.write(app.getName());
app.quit();
