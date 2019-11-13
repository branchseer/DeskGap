import { NativeException } from './internal/bootstrap';
import { BrowserWindow } from './browser-window';
import { Menu, MenuItem } from './menu';
import Dialog from './dialog';
import './internal/async-node';
declare const _default: {
    app: import("./app").App;
    BrowserWindow: typeof BrowserWindow;
    webViews: {
        getAllWebViews(): import("./webview").WebView[];
        getAllWebContents(): import("./webview").WebView[];
        getFocusedWebContents(): import("./webview").WebView | null;
        getFocusedWebView(): import("./webview").WebView | null;
        fromId(id: number): import("./webview").WebView | null;
        setDefaultEngine(engine: "winrt" | "trident"): void;
        getDefaultEngine(): "winrt" | "trident" | null;
        isEngineAvailable(engine: "winrt" | "trident"): boolean;
    };
    webContents: {
        getAllWebViews(): import("./webview").WebView[];
        getAllWebContents(): import("./webview").WebView[];
        getFocusedWebContents(): import("./webview").WebView | null;
        getFocusedWebView(): import("./webview").WebView | null;
        fromId(id: number): import("./webview").WebView | null;
        setDefaultEngine(engine: "winrt" | "trident"): void;
        getDefaultEngine(): "winrt" | "trident" | null;
        isEngineAvailable(engine: "winrt" | "trident"): boolean;
    };
    Menu: typeof Menu;
    MenuItem: typeof MenuItem;
    messageNode: import("../common/events").EventEmitter<any, import("./webview").WebView>;
    ipcMain: import("../common/events").EventEmitter<any, import("./webview").WebView>;
    systemPreferences: import("./system-preferences").SystemPreference;
    dialog: typeof Dialog;
    NativeException: typeof NativeException;
    shell: {
        openExternal: (url: string) => boolean;
    };
};
export = _default;
