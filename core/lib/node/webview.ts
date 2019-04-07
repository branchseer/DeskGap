import { EventEmitter, IEventMap } from '../common/events';
import messageNode from './message-node';
import { evaluateJavaScript } from './internal/js-evaluation';
import { entryPath } from './internal/bootstrap';
import path = require('path');
import globals from './internal/globals';

const { WebViewNative } = require('./bindings');

let currentId = 0;

export interface WebViewEvents extends IEventMap {
    'did-start-loading': [];
    'did-finish-load': [];
    'did-fail-load': [number, string];
    'did-stop-loading': [];
    'page-title-updated': [string];
}

export class WebView extends EventEmitter<WebViewEvents> {
    /** @internal */ private id_: number;
    /** @internal */ private native_: any;

    /** @internal */ private asyncNodeObjectsById_ = new Map<number, any>();
    /** @internal */ private asyncNodeValuesByName_ = new Map<string, any>();
    /** @internal */ private isDevToolsEnabled_: boolean = false;

    constructor(callbacks: { onPageTitleUpdated: (title: string) => void, onReadyToShow: () => void }) {
        super();
        this.id_ = currentId;
        currentId++;

        this.native_ = new WebViewNative({
            didFinishLoad: () => {
                if (this.isDestroyed()) return;
                try {
                    this.trigger_('did-finish-load');
                }
                finally {
                    callbacks.onReadyToShow();
                }
            },
            onStringMessage: (stringMessage: string) => {
                if (this.isDestroyed()) return;
                const [channelName, args]: [string, any] = JSON.parse(stringMessage);
                messageNode['trigger_'](channelName, { sender: this }, ...args);
            },
            onPageTitleUpdated: (title: string) => {
                try {
                    if (this.isDestroyed()) return;
                    this.trigger_('page-title-updated', null, title);
                }
                finally {
                    callbacks.onPageTitleUpdated(title);
                }
            }
        });
    }

    get id(): number {
        return this.id_;
    }
    isDestroyed(): boolean {
        return this.native_ == null;
    }
    
    loadFile(filePath: string): void {
        this.native_.loadLocalFile(path.resolve(entryPath, filePath));
    }
    loadURL(url: string): void {
        const errorMessage = this.native_.loadRequest("GET", url, [], null);
        if (errorMessage != null) {
            throw new Error(errorMessage);
        }
    }
    send(channelName: string, ...args: any[]): void {
        if (this.isDestroyed()) return;
        this.native_.executeJavaScript(`window.deskgap.__messageReceived(${JSON.stringify(channelName)}, ${JSON.stringify(args)})`, null);
    }
    executeJavaScript(code: string): Promise<any> {
        return evaluateJavaScript(this.native_, code);
    }

    setDevToolsEnabled(enabled: boolean): void {
        this.native_.setDevToolsEnabled(enabled);
        this.isDevToolsEnabled_ = enabled;
    }
    
    isDevToolsEnabled(): boolean {
        return this.isDevToolsEnabled_ ;
    }
    
    reload(): void {
        this.native_.reload();
    }
}


export const WebViews = {
    getAllWebViews(): WebView[] {
        return Array.from(globals.webViewsById.values());
    },

    /**
     * Alias to [[getAllWebViews]]
     */
    getAllWebContents(): WebView[] {
        return this.getAllWebViews();
    },

    /**
     * Alias to [[getFocusedWebView]]
     */
    getFocusedWebContents(): WebView | null {
        return this.getFocusedWebView();
    },
    getFocusedWebView(): WebView | null {
        
        if (globals.focusedBrowserWindow == null) {
            return null;
        }
        return globals.focusedBrowserWindow.webContents;
    },
    fromId(id: number): WebView | null {
        return globals.webViewsById.get(id) || null;
    }
}
