# DeskGap

> Build desktop apps using Node.js and the system's WebView.

## Getting Started

Create a folder for your DeskGap application with the structure like this:

```
your-app/
├── package.json
├── index.js
└── index.html
```

Install DeskGap into your application:

```
npm install --save-dev deskgap
```

Add a `start` script in your `package.json`:
```js
{
  ...
  "scripts": {
    "start": "deskgap ."
  }
  ...
}
```

Add something cool in `index.html`, and add this in `index.js`:

```js
const { app, BrowserWindow } = require('deskgap');

app.once('ready', () => {
    const win = new BrowserWindow();
    win.loadFile('index.html');
});
```

Start your application:

```sh
npm start
```
