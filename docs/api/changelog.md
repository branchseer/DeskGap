# Changelog

## [0.1.1] - 2019-4-12

### Added
- 🎉Windows 7 support🎉 [Learn more about web engines on Windows]().
- Per-monitor DPI scaling support on Windows.
- Event `'did-finish-load'` to [`WebView`](https://deskgap.com/api/#webview-alias-webcontents).
- Method `ExecuteJavaScript` to [`WebView`](https://deskgap.com/api/#webview-alias-webcontents).

### Changed
- Rewrite the Windows implementation with the native C/C++ (was C++/CLI).
- Upgrade Node.js to v11.14.0.

### Removed
- Method `LoadHTMLString` of [`WebView`](https://deskgap.com/api/#webview-alias-webcontents).

## [0.0.9] - 2019-02-25

### Added
- 🎉Linux support🎉

### Changed
- Bump the version of Microsoft.Toolkit.Forms.UI.Controls.WebView to 5.1.0


## [0.0.7] - 2019-02-13

### Changed
- Private methods & properties removed in d.ts files.

### Fixed
- Better support for unicode argv in Windows.
- Loading local files no longer requires write access.


## [0.0.6] - 2019-02-08

- Fixed an issue where setting titleBarStyle on a browser window changed its size.
- Fixed an issue where requests for non-existing files on Windows failed silently with 200 status.
- Fixed an issue where extensions of defaultPath in file dialogs could be lost on macOS.
