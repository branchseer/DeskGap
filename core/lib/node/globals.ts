import { BrowserWindow } from './browser-window';
import { WebView } from './webview';

export default {
    browserWindowsById: new Map<number, BrowserWindow>(),
    webViewsById: new Map<number, WebView>(),
    focusedBrowserWindow: <BrowserWindow | null>null
};
