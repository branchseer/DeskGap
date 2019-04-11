import { EventEmitter, IEventMap } from '../common/events';
import messageNode from './message-node';
import { evaluateJavaScript } from './internal/js-evaluation';
import { entryPath } from './internal/bootstrap';
import path = require('path');
import globals from './internal/globals';
import { platform } from 'os';

const { WebViewNative } = require('./bindings');
const isWinRTEngineAvailable = process.platform === 'win32' && WebViewNative.isWinRTEngineAvailable();
type Engine = 'winrt' | 'trident';

let defaultEngine: Engine | null = null;
if (process.platform === 'win32') {
    defaultEngine = isWinRTEngineAvailable ? 'winrt': 'trident';
}

const engineCodeByName: Record<Engine, number> = {
    'trident': 0,
    'winrt': 1
};

export interface WebViewEvents extends IEventMap {
    'did-start-loading': [];
    'did-finish-load': [];
    'did-fail-load': [number, string];
    'did-stop-loading': [];
    'page-title-updated': [string];
}

export interface WebPreferences {
    engine: Engine | null;
}

let currentId = 0;
export class WebView extends EventEmitter<WebViewEvents> {
    /** @internal */ private id_: number;
    /** @internal */ private native_: any;
    /** @internal */ private engine_: Engine | null;

    /** @internal */ private asyncNodeObjectsById_ = new Map<number, any>();
    /** @internal */ private asyncNodeValuesByName_ = new Map<string, any>();
    /** @internal */ private isDevToolsEnabled_: boolean = false;

    constructor(callbacks: { onPageTitleUpdated: (title: string) => void, onReadyToShow: () => void }, preferences: WebPreferences) {
        super();
        this.id_ = currentId;
        currentId++;

        this.engine_ = preferences.engine || defaultEngine;

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
        }, this.engine_ == null ? null : engineCodeByName[this.engine_]);
    }

    get id(): number {
        return this.id_;
    }

    get engine(): Engine | null {
        return this.engine_;
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
    },

    setDefaultEngine(engine: Engine): void {
        defaultEngine = engine;
    },

    getDefaultEngine(): Engine | null {
        return defaultEngine;
    },
    
    isEngineAvailable(engine: Engine): boolean {
        if (process.platform !== 'win32') {
            return false;
        }
        if (engine === 'trident') {
            return true;
        }
        if (engine === 'winrt') {
            return WebViewNative.isWinRTEngineAvailable();
        }
        return false;
    }
}
