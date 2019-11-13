import { BrowserWindow } from './browser-window';
import { Role } from './internal/menu/roles';
export declare type MenuItemType = 'normal' | 'separator' | 'submenu' | 'checkbox';
export declare const MenuTypeCode: {
    main: number;
    context: number;
    submenu: number;
};
export interface MenuItemConstructorOptions {
    role: Role;
    submenu: Array<Partial<MenuItemConstructorOptions> | null> | Menu;
    type: MenuItemType;
    label: string;
    enabled: boolean;
    checked: boolean;
    click: (item: MenuItem, window: BrowserWindow) => void;
    accelerator: string;
}
export interface IMenuPopupOptions {
    window?: BrowserWindow;
    x?: number;
    y?: number;
    positioningItem?: number;
}
export declare class Menu {
    private natives_;
    items: MenuItem[];
    private nativeCallbacks_;
    append(menuItem: MenuItem): void;
    popup(optionsOrWindow?: BrowserWindow | IMenuPopupOptions): void;
    static setApplicationMenu(menu: Menu | null): void;
    static getApplicationMenu(): Menu | null;
    static buildFromTemplate(template: Array<Partial<MenuItemConstructorOptions> | null>): Menu;
    private createNative_;
    private destroyNative_;
}
export declare class MenuItem {
    private label_;
    private enabled_;
    private type_;
    click: (item: MenuItem, window: BrowserWindow | null) => void;
    private submenu_;
    private natives_;
    private checked_;
    private accelerator_;
    private role_;
    constructor(options?: Partial<MenuItemConstructorOptions>);
    readonly label: string;
    readonly submenu: Menu | null;
    enabled: boolean;
    checked: boolean;
    readonly accelerator: string;
    private createNative_;
    private destroyNative_;
}
