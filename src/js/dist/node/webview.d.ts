import { EventEmitter, IEventMap } from '../common/events';
declare type Engine = 'winrt' | 'trident';
export interface WebViewEvents extends IEventMap {
    'did-finish-load': [];
    'page-title-updated': [string];
}
export interface WebPreferences {
    engine: Engine | null;
}
export declare class WebView extends EventEmitter<WebViewEvents> {
    private id_;
    private native_;
    private engine_;
    private asyncNodeObjectsById_;
    private asyncNodeValuesByName_;
    private isDevToolsEnabled_;
    constructor(callbacks: {
        onPageTitleUpdated: (title: string) => void;
        onReadyToShow: () => void;
    }, preferences: WebPreferences);
    readonly id: number;
    readonly engine: Engine | null;
    isDestroyed(): boolean;
    loadFile(filePath: string): void;
    loadURL(url: string): void;
    send(channelName: string, ...args: any[]): void;
    executeJavaScript(code: string): Promise<any>;
    setDevToolsEnabled(enabled: boolean): void;
    isDevToolsEnabled(): boolean;
    reload(): void;
}
export declare const WebViews: {
    getAllWebViews(): WebView[];
    getAllWebContents(): WebView[];
    getFocusedWebContents(): WebView | null;
    getFocusedWebView(): WebView | null;
    fromId(id: number): WebView | null;
    setDefaultEngine(engine: Engine): void;
    getDefaultEngine(): "winrt" | "trident" | null;
    isEngineAvailable(engine: Engine): boolean;
};
export {};
