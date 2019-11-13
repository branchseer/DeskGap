import { Menu } from './menu';
import { EventEmitter, IEventMap } from '../common/events';
declare const pathNameValues: {
    'appData': number;
    'temp': number;
    'desktop': number;
    'documents': number;
    'downloads': number;
    'music': number;
    'pictures': number;
    'videos': number;
    'home': number;
    'userData': number;
};
export declare type PathName = keyof typeof pathNameValues;
export interface AppEvents extends IEventMap {
    'ready': [];
    'window-all-closed': [];
    'will-quit': [];
    'before-quit': [];
    'quit': [number];
}
export declare class App extends EventEmitter<AppEvents> {
    private isReady_;
    private triggersWindowAllClosed_;
    private whenReady_;
    private native_;
    private menu_;
    private menuNativeId_;
    constructor();
    private run_;
    private notifyWindowAllClosed_;
    quit(): void;
    exit(code?: number): void;
    whenReady(): Promise<void>;
    isReady(): boolean;
    getName(): string;
    setName(value: string): void;
    getVersion(): string | null;
    setVersion(version: string): void;
    private pathCache_;
    setPath(name: PathName, value: string): void;
    getPath(name: PathName): string;
    setMenu(menu: Menu | null): void;
    private actuallySetTheMenu_;
    getMenu(): Menu | null;
}
declare const app: App;
export default app;
