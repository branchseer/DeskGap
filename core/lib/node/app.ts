import { Menu, MenuTypeCode } from './menu';
import defaultMenuTemplate from './internal/menu/default-template';
import appInfo from './internal/app-info'

import globals from './globals';
import { EventEmitter, IEventMap } from '../common/events';
import { bulkUISync } from './internal/dispatch';

import path = require('path');

const { AppNative } = require('./bindings');

const pathNameValues = {
    'appData': 0,
    'temp': 1,
    'desktop': 2,
    'documents': 3,
    'downloads': 4,
    'music': 5,
    'pictures': 6,
    'videos': 7,
    'home': 8,
    'userData': -1,
};

export type PathName = keyof typeof pathNameValues;

export interface AppEvents extends IEventMap {
    /**
     * Emitted when DeskGap has finished basic startup.
     * 
     * On Windows and Linux, the will-finish-launching event is the same as the `ready` event; on macOS,
     * this event represents the `applicationWillFinishLaunching` notification of `NSApplication`. 
     */
    'will-finish-launching': [],

    /**
     * Emitted when DeskGap has finished initializing.
     */
    'ready': [],

    /**
     * Emitted when all windows have been closed.
     * 
     * If you do not subscribe to this event and all windows are closed, the default behavior is to quit the app;
     * however, if you subscribe, you control whether the app quits or not.
     * If the user pressed `Cmd + Q`, or the developer called `app.quit()`,
     * DeskGap will first try to close all the windows and then emit the `will-quit` event,
     * and in this case the `window-all-closed` event would not be emitted.
     */
    'window-all-closed': [],
    'will-quit': [],
    'before-quit': [],

    /**
     * Emitted when the application is quitting.
     * @param 0 [[IEventObject]]
     * @param 1 The exit code
     */
    'quit': [number]}

/** 
 * Control your application's event lifecycle.
 * 
 * Thread: Node
 */

export class App extends EventEmitter<AppEvents> {

    /** @internal */ private isReady_ = false;
    /** @internal */ private triggersWindowAllClosed_ = true;
    /** @internal */ private whenReady_: Promise<void>;
    /** @internal */ private native_: any;
    /** @internal */ private menu_: Menu | null = Menu.buildFromTemplate(defaultMenuTemplate);
    /** @internal */ private menuNativeId_: number | null = null;

    constructor() {
        super();

        this.whenReady_ = new Promise((resolve) =>{
            this.native_ = new AppNative({
                onWillFinishLaunching: () => {
                    try {
                        this.trigger_('will-finish-launching');
                    }
                    finally {
                        this.removeAllListeners('will-finish-launching');
                        resolve();
                    }
                },

                onReady: () => {
                    this.isReady_ = true;
                    if (process.platform === 'darwin') {
                        this.actuallySetTheMenu_();
                    }

                    try {
                        this.trigger_('ready');
                    }
                    finally {
                        this.removeAllListeners('ready');
                        resolve();
                    }
                },
        
                //The native land will prevent the quit triggered by user interaction and call this,
                //which actually exits the app by default and can be prevented by event handlers.
                beforeQuit: () => {
                    this.quit();
                }
            });
        });
    }

    /** @internal */ 
    private run_() {
        this.native_.run();
    }

    /** @internal */ 
    private notifyWindowAllClosed_() {
        if (this.triggersWindowAllClosed_) {
            if (!this.trigger_('window-all-closed')) {
                this.quit();
            }
        }
    }

    quit() {
        this.trigger_('before-quit', {
            defaultAction: () => {
                this.triggersWindowAllClosed_ = false;
                try {
                    for (const browserWindow of globals.browserWindowsById.values()) {
                        browserWindow.close();
                        if (!browserWindow.isDestroyed()) {
                            return;
                        }
                    }
                    this.trigger_('will-quit', {
                        defaultAction: () => {
                            this.exit(0);
                        }
                    });
                }
                finally {
                    this.triggersWindowAllClosed_ = true;
                }
            }
        });
    }
    exit(code: number = 0): void {
        this.trigger_('quit', null, code);
        process.on('exit', () => {
            this.native_.exit(code);
        });
        process.exit(code);
    }

    whenReady(): Promise<void> {
        return this.whenReady_;
    }

    isReady(): boolean {
        return this.isReady_;
    }
    getName(): string {
        return appInfo.name;
    }
    setName(value: string): void {
        appInfo.name = value;
    }

    getVersion(): string | null {
        return appInfo.version;
    }
    setVersion(version: string): void {
        appInfo.version = version;
    }

    /** @internal */ private pathCache_ = new Map<PathName, string>();
    setPath(name: PathName, value: string): void {
        this.pathCache_.set(name, value);
    }
    getPath(name: PathName): string {
        let result = this.pathCache_.get(name);
        if (result == null) {
            if (name === 'userData') {
                result = path.join(this.getPath('appData'), this.getName());
            }
            else {
                const pathNameValue = pathNameValues[name];
                if (pathNameValue == null) {
                    throw new TypeError(`Invalid path name: ${name}`);
                }
                result = this.native_.getPath(pathNameValue);
            }
            this.pathCache_.set(name, result as string);
        }
        return result as string;
    }

    setMenu(menu: Menu | null) {
        this.menu_ = menu;
        if (process.platform === 'darwin') {
            if (this.isReady_) {
                this.actuallySetTheMenu_();
            }
        }
        else {
            bulkUISync(() => {
                for (const browserWindow of globals.browserWindowsById.values()) {
                    browserWindow.setMenu(this.menu_);
                }
            });
        }
    }

    /** @internal */
    private actuallySetTheMenu_() {
        if (this.menu_ == null) return;
        bulkUISync(() => {
            const oldMenuNativeId = this.menuNativeId_;
            const oldMenu = this.menu_;

            if (this.menu_ == null) {
                this.native_.setMenu(null);
                this.menuNativeId_ = null;
            }
            else {
                const [menuNativeId, nativeMenu] = this.menu_['createNative_'](MenuTypeCode.main);
                this.native_.setMenu(nativeMenu);
                this.menuNativeId_ = menuNativeId;
            }

            if (oldMenuNativeId != null) {
                oldMenu!['destroyNative_'](oldMenuNativeId);
            }
        });
    }

    getMenu(): Menu | null {
        return this.menu_;
    }
}

const app = new App();

Menu.setApplicationMenu = (menu) => app.setMenu(menu);
Menu.getApplicationMenu = () => app.getMenu();

export default app;
