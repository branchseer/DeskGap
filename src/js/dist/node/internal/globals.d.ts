import { BrowserWindow } from '../browser-window';
import { WebView } from '../webview';
declare const _default: {
    browserWindowsById: Map<number, BrowserWindow>;
    webViewsById: Map<number, WebView>;
    focusedBrowserWindow: BrowserWindow | null;
};
export default _default;
