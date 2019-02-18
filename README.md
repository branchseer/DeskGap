# DeskGap

DeskGap is a framework for building cross-platform desktop apps with web technologies (JavaScript, HTML and CSS).

To enable native capabilities while keeping the size down, DeskGap bundles a [Node.js](https://nodejs.org/) runtime and leaves the HTML rendering to the operating system‘s webview.

## Supported Platforms

- Mac OS X Yosemite (version 10.10) or later
- Windows 10 October 2018 Update (version 1809) or later

Linux support is [work in progress. ![branch status](https://img.shields.io/github/last-commit/patr0nus/DeskGap/gtkwebkit.svg?style=flat)](https://github.com/patr0nus/DeskGap/tree/gtkwebkit)

## Downloads

### Prebuilt Binaries

```sh
npm install --save-dev deskgap
```

### API Demos
The DeskGap API Demos app shows some of the DeskGap features and APIs with interactive scripts.

|macOS|Windows|Source Code|
|-|-|-|
|[![Download](https://api.bintray.com/packages/patr0nus/DeskGap/deskgap-darwin-x64/images/download.svg)](https://deskgap.com/dl/macos) | [![Download](https://api.bintray.com/packages/patr0nus/DeskGap/deskgap-win32-ia32/images/download.svg)](https://deskgap.com/dl/win32) | [GitHub](https://github.com/patr0nus/DeskGap/tree/master/app) |

### Pym: A Real-Life App Built With DeskGap
To test DeskGap on field, [squoosh](https://squoosh.app) is wrapped into a desktop app "Pym" with DeskGap and submitted to the app stores.

|macOS|Windows|Source Code|
|-|-|-|
| [<img alt="Download on Mac App Store" src="https://linkmaker.itunes.apple.com/en-us/badge-lrg.svg?releaseDate=2019-02-12T00:00:00Z&kind=desktopapp&bubble=macos_apps" height="32" />](https://geo.itunes.apple.com/us/app/pym/id1451733095?mt=12&app=apps) | [<img alt="Download on Microsoft Store" src="https://storebadge.azureedge.net/assets/en.png" height="32">](https://www.microsoft.com/store/productId/9PMTMRNBXMPB) | [GitHub](https://github.com/patr0nus/Pym) |

## Getting Started

### Creating a Node.js Package for your app
```
hello-deskgap/
├── package.json
├── index.js
└── index.html
```

`package.json` points to the app's entry file and provides the script that starts your app:
```json
{
  "name": "hello-deskgap",
  "main": "index.js",
  "scripts": {
    "start": "deskgap ."
  }
}
```

`index.js` is the entry file that creates a window which will render an HTML page:
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

### Installing DeskGap

```sh
npm install --save-dev deskgap
```

### Starting Your App

```sh
npm start
```

## Documentation

[Work in Progress](https://deskgap.com/api/)

## FAQ

### What’s the difference between DeskGap and [Electron](https://electronjs.org)? 

DeskGap is designed to be a more lightweight alternative to Electron. It does not bundle [Chromium](https://www.chromium.org/) or any other web engines. Instead, the ability of rendering HTML pages comes from __the webview provided by the operating system__, specifically, [WKWebView](https://developer.apple.com/documentation/webkit/wkwebview) on macOS and [Microsoft.Toolkit.Forms.UI.Controls.WebView](https://docs.microsoft.com/en-us/windows/communitytoolkit/controls/wpf-winforms/webview) on Windows.

DeskGap is at its early stage. __The API is still quite limited__ compared to Electron. Many functionalities are under development and some of them will probably never be possible. See [this](https://deskgap.com/api/) and [this](https://deskgap.com/architecture/#synchronous-and-asynchronous-dispatching) for more information.

### There are already similar attempts ([electrino](https://github.com/pojala/electrino) and [Quark](https://github.com/jscherer92/Quark) for instance) out there. What makes DeskGap different?

With a Node.js runtime bundled, DeskGap comes with support for npm packages and all the battle-tested __native capabilities__ in Node.js such as [`fs`](https://nodejs.org/api/fs.html), [`net`](https://nodejs.org/api/net.html), [`http`](https://nodejs.org/api/http.html). The price is a __larger executable size__ (about 8 MB zipped and 20 MB unzipped).

### Why is the supported version of Windows so high? Any plan of supporting Windows 7?

Older Windows’ __do not have a modern browser engine__, only the one that powers Internet Explorer. Windows 10 1809 is the first version that provides a modern webview with [enough functionalities](https://docs.microsoft.com/en-us/microsoft-edge/dev-guide#win32-webview-updates) for DeskGap to be possible.

To support Windows 7, app developers would have to face compatibility issues coming from as low as IE 8. I personally don’t have enough motivation and interest to do this, but pull requests are always welcome.

If you want to try DeskGap but dropping Windows 7 support is a no-go for your app, consider packaging the app with Electron for the unsupported platform. The DeskGap API is intentionally designed to be like Electron’s. The following code is a good start:

```js
let appEngine;
try {
  appEngine = require('deskgap');
}
catch (e) {
  appEngine = require('electron');
}
const { app, BrowserWindow } = appEngine;
```

### So I can port my Electron app to DeskGap?

Probably no. The DeskGap API is still quite limited. If you start building an app with DeskGap, getting it running on Electron may be easy, but not the other way around.

