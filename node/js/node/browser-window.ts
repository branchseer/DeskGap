import { bulkUISync } from './internal/dispatch';
import app from './app';
import { EventEmitter, IEventMap } from './internal/events';
import globals from './internal/globals';
import { Menu, MenuTypeCode } from './menu';
import { WebView, WebPreferences } from './webview';

const { BrowserWindowNative } = require('./bindings');

const TitleBarStyleCode = {
    default: 0,
    hidden: 1,
    hiddeninset: 2
} as {
    [index: string]: number
};

const vibrancyLayoutAttributes = new Set(['left', 'right', 'top', 'bottom', 'width', 'height']);
export type VibrancyMaterial = 'appearance-based' | 'light' | 'dark' | 'medium-light' | 'ultra-dark' | 'titlebar' | 'selection' |
    'menu' | 'popover' | 'sidebar' | //10.11+
    'header-view' | 'sheet' | 'window-background' | 'hud-window' | 'full-screen-ui' | 'tool-tip' | 'content-background' | 'under-window-background' | 'under-page-background' //10.14+

export type VibrancyBlendingMode = 'behind-window' | 'within-window';

export type VibrancyState = 'follows-window' | 'active' | 'inactive';

export type TitleBarStyle = 'default' | 'hidden' | 'hiddenInset';

export interface Vibrancy {
    material?: VibrancyMaterial;
    blending?: VibrancyBlendingMode;
    state?: VibrancyState;
    left?: string | number;
    right?: string | number;
    top?: string | number;
    bottom?: string | number;
}

export interface IBrowserWindowConstructorOptions {
    width: number, height: number,
    x: number, y: number,
    center: boolean,
    title: string,
    icon: string | null,
    show: boolean,
    titleBarStyle: TitleBarStyle,
    maximizable: boolean,
    minimizable: boolean,
    resizable: boolean,
    frame: boolean,
    closable: boolean,
    vibrancies: Vibrancy[],
    vibrancy: VibrancyMaterial,
    maxHeight: number, maxWidth: number,
    minHeight: number, minWidth: number,
    menu: Menu | null,
    webPreferences: Partial<WebPreferences>
};

export interface BrowserWindowEvents extends IEventMap {
    'blur': [],
    'focus': [],
    'resize': [],
    'close': [],
    'move': [],
    'page-title-updated': [string],
    'ready-to-show': [],
    'closed': []
}

export class BrowserWindow extends EventEmitter<BrowserWindowEvents> {
    /** @internal */ private id_: number;
    /** @internal */ private hasBeenShown_ = false;
    /** @internal */ private webview_: WebView;
    /** @internal */ private native_: any;
    /** @internal */ private title_: string;
    /** @internal */ private titleBarStyle_: TitleBarStyle;
    /** @internal */ private minimumSize_: [number, number];
    /** @internal */ private maximumSize_: [number, number];
    /** @internal */ private menu_: Menu | null = null;
    /** @internal */ private menuNativeId_: number | null = null;

    constructor(options: Partial<IBrowserWindowConstructorOptions> = {}) {
        super();

        let defaultMenu: Menu | null = null;
        if (process.platform !== 'darwin' && !options.hasOwnProperty('menu')) {
            defaultMenu = app.getMenu()
        }
        
        const fullOptions: IBrowserWindowConstructorOptions = Object.assign({
            width: 800, height: 600,
            x: 0, y: 0,
            center: (typeof options.x !== 'number') && (typeof options.y !== 'number'),
            title: "Untitled Window",
            icon: null,
            show: true,
            titleBarStyle: 'default',
            maximizable: true,
            minimizable: true,
            resizable: true,
            frame: true,
            closable: true,
            vibrancies: null,
            vibrancy: null,
            maxHeight: 0,
            maxWidth: 0,
            minHeight: 0,
            minWidth: 0,
            menu: defaultMenu,
            webPreferences: { }
        }, options);

        bulkUISync(() => {
            this.webview_ = new WebView({
                onPageTitleUpdated: (title: string) => {
                    if (this.isDestroyed()) return;
                    this.trigger_('page-title-updated', {
                        defaultAction: () => this.setTitle(title)
                    }, title);
                },
                onReadyToShow: () => {
                    if (this.isDestroyed()) return;
                    if (!this.hasBeenShown_) {
                        this.trigger_('ready-to-show');
                    }
                }
            }, Object.assign({ engine: null }, fullOptions.webPreferences));

            this.native_ = new BrowserWindowNative(this.webview_['native_'],  {
                onBlur: () => {
                    if (this.isDestroyed()) return;
                    if (globals.focusedBrowserWindow === this) {
                        globals.focusedBrowserWindow = null;
                    };
                    this.trigger_('blur');
                },
                onFocus: () => {
                    if (this.isDestroyed()) return;
                    globals.focusedBrowserWindow = this;
                    this.trigger_('focus');
                },
                onResize: () => {
                    if (this.isDestroyed()) return;
                    this.trigger_('resize')
                },
                onMove: () => {
                    if (this.isDestroyed()) return;
                    this.trigger_('move')
                },
                onClose: () => {
                    if (this.isDestroyed()) return;
                    this.trigger_('close', { defaultAction: () => this.destroy() })
                }
            });

            this.native_.setMaximizable(fullOptions.maximizable);
            this.native_.setMinimizable(fullOptions.minimizable);
            this.native_.setResizable(fullOptions.resizable);
            this.native_.setHasFrame(fullOptions.frame);
            this.native_.setClosable(fullOptions.closable);

            if (process.platform !== 'darwin') {
                this.setMenu(fullOptions.menu);
                this.setIcon(fullOptions.icon);
            }

            if (process.platform === 'darwin' && fullOptions.frame) {
                this.setTitleBarStyle(fullOptions.titleBarStyle);
            }

            this.setTitle(fullOptions.title);
            this.setMaximumSize(fullOptions.maxWidth, fullOptions.maxHeight);
            this.setMinimumSize(fullOptions.minWidth, fullOptions.minHeight);
            this.setSize(fullOptions.width, fullOptions.height, false);
            if (fullOptions.center) {
                this.native_.center();
            }
            else {
                this.native_.setPosition(fullOptions.x, fullOptions.y,false);
            }

            if (process.platform === 'darwin') {
                if (fullOptions.vibrancies) {
                    this.setVibrancies(fullOptions.vibrancies);
                }
                else {
                    this.setVibrancy(fullOptions.vibrancy);
                }
            }

            if (fullOptions.show) {
                this.show();
            }
        });

        this.id_ = this.webview_.id;

        globals.browserWindowsById.set(this.id_, this);
        globals.webViewsById.set(this.webview_.id, this.webview_);

    }
    get id() {
        return this.id_;
    }

    setTitleBarStyle(style: TitleBarStyle): void {
        this.titleBarStyle_ = style;
        
        this.native_.setTitleBarStyle(TitleBarStyleCode[style.toLowerCase()] || TitleBarStyleCode.default);
    }
    show() {
        if (!this.hasBeenShown_) {
            if (process.platform !== 'darwin') {
                this.actuallySetTheMenu_();
            }
            this.hasBeenShown_ = true;
        }
        this.native_.show();
    }
    setSize(width: number, height: number, animate: boolean = false) {
        this.native_.setSize(width, height, animate);
    }
    setMaximumSize(width: number, height: number) {
        this.native_.setMaximumSize(width, height);
    }
    setMinimumSize(width: number, height: number) {
        this.native_.setMinimumSize(width, height);
    }

    setVibrancy(material: VibrancyMaterial | null) {
        if (material != null) {
            this.setVibrancies([{ material, left: 0, right: 0, top: 0, bottom: 0 }]);
        }
        else {
            this.setVibrancies([]);
        }
    }
    
    setVibrancies(vibrancies: Vibrancy[]) {
        this.native_.setVibrancies((vibrancies || []).filter(v => v != null).map(v => [
            v.material || 'appearance-based',
            v.blending || (v.material === 'titlebar' ? 'within-window': 'behind-window'),
            v.state || 'follows-window',
            (() => {
                const layoutEntries = Object.entries(v).filter(entry => vibrancyLayoutAttributes.has(entry[0]));
                if (layoutEntries.length != 4) {
                    throw new Error("There must be exactly 4 layout attributes in a vibrancy");
                }
                return layoutEntries.map(([key, value]) => {
                    let numValue = value;
                    let isPoint = true;
                    if (typeof value === 'string') {
                        if (value.endsWith('%')) {
                            isPoint = false;
                            value = value.substring(0, value.length - 1);
                        }
                        numValue = parseFloat(value);
                    }
                    return [key, numValue, isPoint];
                })
            })()
        ]));
    }
    setPosition(x: number, y: number, animate: boolean = false): void {
        this.native_.setPosition(x, y, animate);
    }
    setTitle(title: string): void {
        this.title_ = title;
        this.native_.setTitle(title);
    }
    getTitle(): string {
        return this.title_;
    }
    center(): void {
        this.native_.center();
    }
    setMenu(menu: Menu | null) {
        if (this.menuNativeId_ != null) {
            bulkUISync(() => {
                this.menu_!['destroyNative_'](this.menuNativeId_!);
            })
        }
        this.menu_ = menu;

        if (this.hasBeenShown_) {
            this.actuallySetTheMenu_();
        }
    }

    /** @internal */ 
    private actuallySetTheMenu_() {
        bulkUISync(() => {
            if (this.menu_ == null) {
                this.native_.setMenu(null);
                this.menuNativeId_ = null;
            }
            else {
                const [menuNativeId, nativeMenu] = this.menu_['createNative_'](MenuTypeCode.main, this);
                this.native_.setMenu(nativeMenu);
                this.menuNativeId_ = menuNativeId;
            }
        });
    }
    getMenu(): Menu | null {
        return this.menu_;
    }
    setIcon(path: string | null) {
        this.native_.setIcon(path);
    }
    getPosition(): [number, number] {
        return this.native_.getPosition();
    }
    getSize(): [number, number] {
        return this.native_.getSize();
    }
    destroy(): void {
        bulkUISync(() => {
            if (this.menuNativeId_ != null) {
                this.menu_!['destroyNative_'](this.menuNativeId_);
                this.menu_ = null;
            }
            this.webview_['native_'].destroy();
            this.native_.destroy();
        });
        this.webview_['native_'] = null;
        this.native_ = null;

        if (globals.focusedBrowserWindow === this) {
            globals.focusedBrowserWindow = null;
        };
        globals.browserWindowsById.delete(this.id_);
        globals.webViewsById.delete(this.id_);

        try {
            this.trigger_('closed');
        }
        finally {
            this.removeAllListeners();

            if (globals.browserWindowsById.size === 0) {
                app['notifyWindowAllClosed_']();
            }
        }
    }
    isDestroyed(): boolean {
        return this.native_ == null;
    }

    minimize(): void {
        this.native_.minimize();
    }
    
    close(): void { 
        this.trigger_('close', { defaultAction: () => this.destroy() });
    }

    get webView(): WebView {
        return this.webview_;
    }

    get webContents(): WebView {
        return this.webView;
    }

    loadFile(filePath: string): void {
        this.webview_.loadFile(filePath);
    }
    loadURL(url: string): void {
        this.webview_.loadURL(url);
    }
    reload(): void {
        this.webview_.reload();
    }
    static getAllWindows(): BrowserWindow[] {
        return Array.from(globals.browserWindowsById.values());
    }
    static getFocusedWindow(): BrowserWindow | null {
        return globals.focusedBrowserWindow;
    }
    static fromWebView(webview: WebView): BrowserWindow {
        return globals.browserWindowsById.get(webview.id)!;
    }
    static fromWebContents(webview: WebView): BrowserWindow {
        return this.fromWebView(webview);
    }
    static fromId(id: number): BrowserWindow | null {
        return globals.browserWindowsById.get(id) || null;
    }
}
