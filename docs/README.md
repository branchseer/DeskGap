# DeskGap

DeskGap is a framework for building cross-platform desktop apps with web technologies (JavaScript, HTML and CSS).

To enable native capabilities while keeping the size down, DeskGap bundles a [Node.js](https://nodejs.org/) runtime and leaves the HTML rendering to the operating system‘s webview.

[![Build Status](https://dev.azure.com/patr0nus/DeskGap/_apis/build/status/patr0nus.DeskGap?branchName=master)](https://dev.azure.com/patr0nus/DeskGap/_build/latest?definitionId=6&branchName=master) [![Build Status](https://travis-ci.com/patr0nus/DeskGap.svg?branch=master)](https://travis-ci.com/patr0nus/DeskGap)

## Supported Platforms

<table>
  <thead>
    <tr>
      <th></th>
      <th>macOS</th>
      <th colspan="2">Windows</th>
      <th>Linux</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Version</td>
      <td>10.11+</td>
      <td>7 SP1<sup>1</sup> - 10 version 1803</td>
      <td>10 version 1809+</td>
      <td>Tested on Ubuntu 16.04 LTS</td>
    </tr>
    <tr>
      <td>Rendering Engine</td>
      <td>WebKit</td>
      <td>Trident</td>
      <td>EdgeHTML<sup>2</sup></td>
      <td>WebKit</td>
    </tr>
  </tbody>
</table>

1. Internet Explorer 11 is required for Node.js interop to work
2. Trident is also available if specified explicitly.


## Downloads

### Prebuilt Binaries

```sh
npm install --save-dev deskgap
```

### API Demos
The DeskGap API Demos app shows some of the DeskGap features and APIs with interactive scripts.

|macOS|Windows|Linux|Source Code|
|-|-|-|-|
|[![Download](https://api.bintray.com/packages/patr0nus/DeskGap/deskgap-darwin-x64/images/download.svg)](https://deskgap.com/dl/macos) | [![Download](https://api.bintray.com/packages/patr0nus/DeskGap/deskgap-win32-ia32/images/download.svg)](https://deskgap.com/dl/win32) | [![Download](https://api.bintray.com/packages/patr0nus/DeskGap/deskgap-linux-x64/images/download.svg)](https://deskgap.com/dl/linux) |[GitHub](https://github.com/patr0nus/DeskGap/tree/master/app) |

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
<head>
  <meta charset="utf-8" />
  <meta http-equiv="X-UA-Compatible" content="ie=edge" />
  <title>Hello DeskGap</title>
</head>
<body>
  <h1>Hello DeskGap</h1>
</body>
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

DeskGap is designed to be a more lightweight alternative to Electron. It does not bundle [Chromium](https://www.chromium.org/) or any other web engines. Instead, the ability of rendering HTML pages comes from __the webview provided by the operating system__, specifically, [WKWebView](https://developer.apple.com/documentation/webkit/wkwebview) on macOS, [IWebBrowser2](https://docs.microsoft.com/en-us/previous-versions/windows/internet-explorer/ie-developer/platform-apis/aa752127(v%3Dvs.85)) or [WebViewControl](https://docs.microsoft.com/en-us/uwp/api/windows.web.ui.interop.webviewcontrol) (if available) on Windows, and [WebKitWebView](https://webkitgtk.org/reference/webkitgtk/stable/webkitgtk-webkitwebview.html) on Linux.

DeskGap is at its early stage. __The API is still quite limited__ compared to Electron. Many functionalities are under development and some of them will probably never be possible. See [this](https://deskgap.com/api/) and [this](https://deskgap.com/architecture/#synchronous-and-asynchronous-dispatching) for more information.

### There are already similar attempts ([electrino](https://github.com/pojala/electrino) and [Quark](https://github.com/jscherer92/Quark) for instance) out there. What makes DeskGap different?

With a Node.js runtime bundled, DeskGap comes with support for npm packages and all the battle-tested __native capabilities__ in Node.js such as [`fs`](https://nodejs.org/api/fs.html), [`net`](https://nodejs.org/api/net.html), [`http`](https://nodejs.org/api/http.html). The price is a __larger executable size__ (about 8 MB zipped and 20 MB unzipped).

### Can I port my Electron app to DeskGap without much modification?

Probably no. The DeskGap API is still quite limited. If you start building an app with DeskGap, getting it running on Electron may be easy, but not the other way around.
