# API Reference

!!! info "Work in Progress"
    For now most of the APIs listed below are linked to the Electron documentation pages respectfully.   
    Please be aware that they work similarly, but probably not exactly the same.

## [app](https://electronjs.org/docs/api/app)

### Events

* [`ready`](https://electronjs.org/docs/api/app#event-ready)
* [`window-all-closed`](https://electronjs.org/docs/api/app#event-window-all-closed)
* [`before-quit`](https://electronjs.org/docs/api/app#event-before-quit)
* [`will-quit`](https://electronjs.org/docs/api/app#event-will-quit)
* [`quit`](https://electronjs.org/docs/api/app#event-quit)

### Methods

* [`quit()`](https://electronjs.org/docs/api/app#appquit)
* [`exit([exitCode])`](https://electronjs.org/docs/api/app#appexitexitcode)
* [`isReady()`](https://electronjs.org/docs/api/app#appisready)
* [`whenReady()`](https://electronjs.org/docs/api/app#appwhenready)
* [`getPath(name)`](https://electronjs.org/docs/api/app#appgetpathname) (supported names: `home`, `appData`, `temp`, `desktop`, `documents`, `downloads`, `music`, `pictures`, `videos`, `home`, `userData`)
* [`setPath(name, path)`](https://electronjs.org/docs/api/app#appsetpathname-path)
* [`getVersion()`](https://electronjs.org/docs/api/app#appgetversion)
* [`getName()`](https://electronjs.org/docs/api/app#appgetname)
* [`setName(name)`](https://electronjs.org/docs/api/app#appsetnamename)

## [BrowserWindow](https://electronjs.org/docs/api/browser-window)
### [`new BrowserWindow(options)`](https://electronjs.org/docs/api/browser-window#new-browserwindowoptions)

Supported `options` fields:

* `width`, `height`
* `x`, `y`
* `center`
* `minWidth`, `minHeight`, `maxWidth`, `maxHeight`
* `resizable`, `minimizable`, `maximizable`, `closable`
* `icon` (path string only)
* `frame`
* `title`
* `show`
* `vibrancy`
* `menu`
* `titleBarStyle` (supported values: `default`, `hidden`, `hiddenInset`)

### Instance Events

* [`ready-to-show`](https://electronjs.org/docs/api/browser-window#using-ready-to-show-event)
* [`page-title-updated`](https://electronjs.org/docs/api/browser-window#event-page-title-updated)
* [`blur`](https://electronjs.org/docs/api/browser-window#event-blur)
* [`focus`](https://electronjs.org/docs/api/browser-window#event-focus)
* [`move`](https://electronjs.org/docs/api/browser-window#event-move)
* [`resize`](https://electronjs.org/docs/api/browser-window#event-resize)
* [`close`](https://electronjs.org/docs/api/browser-window#event-close)
* [`closed`](https://electronjs.org/docs/api/browser-window#event-closed)

### Instance Methods

* [`destroy()`](https://electronjs.org/docs/api/browser-window#windestroy)
* [`close()`](https://electronjs.org/docs/api/browser-window#windestroy)
* [`minimize()`](https://electronjs.org/docs/api/browser-window#winminimize)
* [`isDestroyed()`](https://electronjs.org/docs/api/browser-window#winisdestroyed)
* [`show()`](https://electronjs.org/docs/api/browser-window#winshow)
* [`setSize(width, height[, animate])`](https://electronjs.org/docs/api/browser-window#winsetsizewidth-height-animate)
* [`getSize()`](https://electronjs.org/docs/api/browser-window#wingetsize)
* [`setMinimumSize(width, height)`](https://electronjs.org/docs/api/browser-window#winsetminimumsizewidth-height)
* [`setMaximumSize(width, height)`](https://electronjs.org/docs/api/browser-window#winsetmaximumsizewidth-height)
* [`setMenu(menu)`](https://electronjs.org/docs/api/browser-window#winsetmenumenu-linux-windows)
* [`setIcon(icon)`](https://electronjs.org/docs/api/browser-window#winseticonicon-windows-linux)
* [`setPosition(x, y[, animate])`](https://electronjs.org/docs/api/browser-window#winsetpositionx-y-animate)
* [`getPosition()`](https://electronjs.org/docs/api/browser-window#wingetposition)
* [`setTitle(title)`](https://electronjs.org/docs/api/browser-window#winsettitletitle)
* [`getTitle()`](https://electronjs.org/docs/api/browser-window#wingettitle)
* [`loadFile(filePath)`](https://electronjs.org/docs/api/browser-window#winloadfilefilepath-options) (not supporting the `options` parameter)
* [`loadURL(url)`](https://electronjs.org/docs/api/browser-window#winloadurlurl-options) (not supporting the `options` parameter)

### Instance Properties

* `webView` (alias: [`webContents`](https://electronjs.org/docs/api/browser-window#winwebcontents))
* [`id`](https://electronjs.org/docs/api/browser-window#winid)


### Static Methods

* [`getAllWindows()`](https://electronjs.org/docs/api/browser-window#browserwindowgetallwindows)
* [`getFocusedWindow()`](https://electronjs.org/docs/api/browser-window#browserwindowgetfocusedwindow)
* `fromWebView(webView)` (alias: [`fromWebContents(webContents)`](https://electronjs.org/docs/api/browser-window#browserwindowfromwebcontentswebcontents))
* [`fromId(id)`](https://electronjs.org/docs/api/browser-window#browserwindowfromidid)

## `WebView` (alias: [WebContents](https://electronjs.org/docs/api/web-contents#class-webcontents))

### Instance Methods

* [`isDestroyed()`](https://electronjs.org/docs/api/web-contents#contentsisdestroyed)
* [`loadFile(filePath)`](https://electronjs.org/docs/api/web-contents#contentsloadfilefilepath-options) (not supporting the `options` parameter)
* [`loadURL(url)`](https://electronjs.org/docs/api/web-contents#contentsloadurlurl-options) (not supporting the `options` parameter)
* [`reload()`](https://electronjs.org/docs/api/web-contents#contentsreload)
* [`send(channel[, arg1][, arg2][, ...])`](https://electronjs.org/docs/api/web-contents#contentssendchannel-arg1-arg2-)

### Instance Properties

* [`id`](https://electronjs.org/docs/api/web-contents#contentsid)

## `webViews` (alias: [`webContents`](https://electronjs.org/docs/api/web-contents))

### Methods

* `getAllWebViews()` (alias: [`getAllWebContents()`](https://electronjs.org/docs/api/web-contents#webcontentsgetallwebcontents))
* `getFocusedWebView()` (alias: [`getFocusedWebContents()`](https://electronjs.org/docs/api/web-contents#webcontentsgetfocusedwebcontents))
* [`fromId(id)`](https://electronjs.org/docs/api/web-contents#webcontentsfromidid)


## `messageUI` (alias: [`ipcRenderer`](https://electronjs.org/docs/api/ipc-renderer))

### Methods
* [`on(channel, listener)`](https://electronjs.org/docs/api/ipc-renderer#ipcrendereronchannel-listener)
* [`send(channel[, arg1][, arg2][, ...])`](https://electronjs.org/docs/api/ipc-renderer#ipcrenderersendchannel-arg1-arg2-)

## `messageNode` (alias: [`ipcMain`](https://electronjs.org/docs/api/ipc-main))

### Methods

* [`on(channel, listener)`](https://electronjs.org/docs/api/ipc-main#ipcmainonchannel-listener)

## [`dialog`](https://electronjs.org/docs/api/dialog)

### Methods

* [`showErrorBox(title, content)`](https://electronjs.org/docs/api/dialog#dialogshowerrorboxtitle-content)
* [`showOpenDialog(browserWindow, options, callback)`](https://electronjs.org/docs/api/dialog#dialogshowopendialogbrowserwindow-options-callback)
* [`showSaveDialog(browserWindow, options, callback)`](https://electronjs.org/docs/api/dialog#dialogshowsavedialogbrowserwindow-options-callback)

## [`shell`](https://electronjs.org/docs/api/shell)

### Methods

* [`shell.openExternal(url)`](https://electronjs.org/docs/api/shell#shellopenexternalurl-options-callback) (not supporting `options` and `callback`)

## [`systemPreferences`](https://electronjs.org/docs/api/system-preferences)

### Methods

* [`isDarkMode()`](https://electronjs.org/docs/api/system-preferences) (available on macOS and Windows)

### Events

* `dark-mode-toggled` (available on macOS and Windows): will be emitted when the user turns on or turns off the system‘s dark mode. 
