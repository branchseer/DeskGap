# DeskGap

> Build desktop apps using Node.js and the system's WebView.

## Supported Platforms

- Mac OS X Yosemite (version 10.10) or later
- Windows 10 October 2018 Update (version 1809) or later

## Getting Started

### Seeing What DeskGap Can Do

#### API Demo
The DeskGap API Demo app shows some of the DeskGap features and APIs with interactive scripts.

|macOS|Windows|Source Code|
|-|-|-|
|[Download](https://deskgap.com/dl/macos) | [Download](https://deskgap.com/dl/win32) | [Link](https://github.com/patr0nus/DeskGap/tree/master/app) |

#### A Real-Life App
To test DeskGap on field, A simple app ”Pym“ was built with it and submitted to the app stores.

|macOS|Windows|Source Code|
|-|-|-|
|[Mac App Store](https://deskgap.com/dl/macos) | [Microsoft Store](https://deskgap.com/dl/win32) | [GitHub](https://github.com/patr0nus/Pym) |

### Writing Your First DeskGap App

#### Create a folder with the structure:
```
hello-deskgap/
├── package.json
├── index.js
└── index.html
```

`package.json` points to the app's entry file and provides the script the start your app:
```js
{
  "name": "hello-deskgap",
  "main": "index.js",
  "scripts": {
    "start": "deskgap ."
  }
}
```

`index.js` is the entry file that creates a window which will render a HTML page:
```js
const { app, BrowserWindow } = require('deskgap');

app.once('ready', () => {
    const win = new BrowserWindow();
    win.loadFile('index.html');
});
```

`index.html` is the page to render:
```html
<!DOCTYPE html>
<html>
  <head><meta charset="utf-8" /><title>Hello DeskGap</title></head>
  <body><h1>Hello DeskGap</h1></body>
</html>
```

#### Install DeskGap:

```
npm install --save-dev deskgap
```

#### Start your application:

```sh
npm start
```
