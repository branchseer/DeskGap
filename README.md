# DeskGap

DeskGap is a framework for building cross-platform desktop apps with web technologies (JavaScript, HTML and CSS).

To enable native capabilities while keeping the size down, DeskGap bundles a [Node.js](https://nodejs.org/) runtime but leave the HTML rendering to the operating system‘s webview.

## Supported Platforms

- Mac OS X Yosemite (version 10.10) or later
- Windows 10 October 2018 Update (version 1809) or later

## Downloads

### Prebuilt Binaries

```sh
npm install --save-dev deskgap
```

### API Demos
The DeskGap API Demos app shows some of the DeskGap features and APIs with interactive scripts.

|macOS|Windows|Source Code|
|-|-|-|
|[Download](https://deskgap.com/dl/macos) | [Microsoft Store](https://deskgap.com/dl/win32) | [Link](https://github.com/patr0nus/DeskGap/tree/master/app) |

### Pym: A Real-Life App Built With DeskGap
To test DeskGap on field, a simple app ”Pym“ was built based on it and submitted to the app stores.

|macOS|Windows|Source Code|
|-|-|-|
|In Review | In Review | [GitHub](https://github.com/patr0nus/Pym) |

## Getting Started

### Creating the Node.js Package
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

### Installing DeskGap

```
npm install --save-dev deskgap
```

### Starting Your App

```sh
npm start
```

## FAQ

### What’s the difference between DeskGap and [Electron](https://electronjs.org)? 

DeskGap is designed to be a lightweight alternative to Electron. It does not bundle [Chromium](https://www.chromium.org/) or any other web engines. Instead, the ability of rendering HTML pages comes from the webview provided by the operating system, specifically, [WKWebView](https://developer.apple.com/documentation/webkit/wkwebview) on macOS and [Microsoft.Toolkit.Forms.UI.Controls.WebView](https://docs.microsoft.com/en-us/windows/communitytoolkit/controls/wpf-winforms/webview) on Windows.

DeskGap is at its early stage. The API is still quite limited compared to Electron. Many functionalities are under development and some of them will probably never be possible. See [this](https://deskgap.com/api/) for more information.

### There are already similar attempts ([electrino](https://github.com/pojala/electrino) and [Quark](https://github.com/jscherer92/Quark) for instance) out there. What makes DeskGap different?

Bundling a Node.js runtime, DeskGap is not that lightweight compared to them. I think this is the sweet spot: most criticism around Electron is about Chromium, which is the cause of size bloating and memory hunger. The Node.js part is actually fine: Node.js enables the cross-platform native capabilities like filesystem access, and the price is a few dozens of megabytes in file size and a reasonable resource consumption.

### Why is the supported version of Windows so high? Any plan of supporting Windows 7 and Linux?

Older Windows’ do not have a modern browser engine, only the one that powers Internet Explorer. Windows 10 1809 is the first version that provides a modern webview with [enough functionalities](https://docs.microsoft.com/en-us/microsoft-edge/dev-guide#win32-webview-updates) for DeskGap. To support Windows 7, app developers would have to face compatibility issues coming from as low as IE 8. I personally don’t have enough motivation and interest to do this, but pull requests are always welcome.

Linux support would be great but I have little knowledge of Linux app development. For now I am looking at [Qt WebEngine](https://doc.qt.io/qt-5/qtwebengine-index.html). Any advice & help is appreciated.

If you want to try DeskGap but dropping Windows 7 (or Linux) support is a no-go for your app, consider packaging the app with Electron for the unsupported platform. The DeskGap API is intentionally designed to be like Electron’s. The following code is a good start:

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

Probably no. The DeskGap API is still quite limited. If you start building an app with DeskGap, getting it running on Electron could be easy, but not the other way around.

