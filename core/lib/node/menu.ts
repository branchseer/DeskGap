import { bulkUISync } from './internal/dispatch';
import { parseAcceleratorToTokens } from './accelerator';
import globals from './globals';
import { BrowserWindow } from './browser-window'
import roleDefaults, { Role } from './internal/menu/roles';

const { MenuItemNative, MenuNative } = require('./bindings');

export type MenuItemType = 'normal' | 'separator' | 'submenu' | 'checkbox';
const MenuItemTypeCode = {
    normal: 0, separator: 1, submenu: 2, checkbox: 3
};

type MenuType = 'main' | 'context' | 'submenu';
export const MenuTypeCode = {
    main: 0, context: 1, submenu: 2
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
    window?: BrowserWindow; x?: number; y?: number; positioningItem?: number;
}
export class Menu {
    private native_: any = null;
    public items: MenuItem[] = [];
    private createdNativeTypeCode_: number | null = null;
    private nativeCallbacks_ = {};
    
    append(menuItem: MenuItem) {
        this.items.push(menuItem);
    }
    popup(optionsOrWindow?: BrowserWindow | IMenuPopupOptions) {
        const options: IMenuPopupOptions | undefined = (optionsOrWindow instanceof BrowserWindow) ? { window: optionsOrWindow } : optionsOrWindow;

        const fullOptions: IMenuPopupOptions = Object.assign({
            window: globals.focusedBrowserWindow,
            positioningItem: -1
        }, options);

        let location: [number, number] | null = null;
        if ((typeof fullOptions.x === 'number') && (typeof fullOptions.y === 'number')) {
            location = [fullOptions.x, fullOptions.y];
        }


        if (fullOptions.window == null) {
            return;
        }

        bulkUISync(() => {
            if (fullOptions.window == null) {
                throw new Error('No window specified for Menu#popup, and there is no focused window');
            }
            this.createNative_(MenuTypeCode.context);
            fullOptions.window['native_'].popupMenu(this.native_, location, fullOptions.positioningItem);
        });
    }
    static setApplicationMenu(menu: Menu | null) {
        throw new Error('This method should have been overridden in the "app" module');
    }
    static getApplicationMenu(): Menu | null {
        throw new Error('This method should have been overridden in the "app" module');
    }
    static buildFromTemplate(template: Array<Partial<MenuItemConstructorOptions> | null>) {
        const newMenu = new Menu();
        for (const itemConstructorOptions of template) {
            if (itemConstructorOptions != null) {
                newMenu.append(new MenuItem(itemConstructorOptions));
            }
        }
        return newMenu;
    }
    private createNative_(type: number) {
        if (type === this.createdNativeTypeCode_) {
            return;
        }
        if (this.createdNativeTypeCode_ != null) {
            throw new Error("A Menu can't be used as both a main menu and a context menu");
        }
        this.createdNativeTypeCode_ = type;
        this.native_ = new MenuNative(type, this.nativeCallbacks_);
        for (const item of this.items) {
            (<any>item).createNative_();
            this.native_.append((<any>item).native_);
        }
    }
};

export class MenuItem {
    private label_: string;
    private enabled_: boolean;
    private type_: number;
    public click: (item: MenuItem, window: BrowserWindow | null) => void;
    private submenu_: Menu | null;
    private native_: any;
    private checked_: boolean;
    private accelerator_: string;
    private role_: string; 

    constructor(options: Partial<MenuItemConstructorOptions> = {}) {
        if (options.role != null) {
            const lowerCasedRole = options.role.toLowerCase() as Role;
            options = Object.assign({}, roleDefaults[lowerCasedRole], options);
            options.role = lowerCasedRole;
        }

        const fullOptions: MenuItemConstructorOptions = Object.assign({
            label: '',
            type: (options.submenu != null) ? 'submenu': 'normal',
            checked: false,
            submenu: null,
            click: null,
            enabled: true,
            accelerator: '',
            role: ''
        }, options);

        if (fullOptions.submenu != null && !(fullOptions.submenu instanceof Menu)) {
            fullOptions.submenu = Menu.buildFromTemplate(fullOptions.submenu);
        }

        if (fullOptions.submenu == null && fullOptions.type === 'submenu') {
            fullOptions.submenu = new Menu();
        }

        this.label_ = fullOptions.label;
        this.enabled_ = fullOptions.enabled;
        this.submenu_ = fullOptions.submenu as (Menu | null);
        this.type_ = MenuItemTypeCode[fullOptions.type];
        this.click = fullOptions.click;
        this.native_ = null;
        this.checked_ = fullOptions.checked;
        this.accelerator_ = fullOptions.accelerator;
        this.role_ = fullOptions.role;   
    }
    
    get label(): string {
        return this.label_;
    }
    get submenu(): Menu | null {
        return this.submenu_;
    }
    get enabled(): boolean {
        return this.enabled_;
    }
    set enabled(value: boolean) {
        if (this.native_ != null) {
            this.native_.setEnabled(value);
        }
        this.enabled_ = value;
    }
    get checked(): boolean {
        return this.checked_;
    }
    set checked(value: boolean) {
        this.checked_ = value
        if (this.native_ != null) {
            this.native_.setChecked(value);
        }
    }
    get accelerator(): string {
        return this.accelerator_;
    }
    private createNative_() {
        let nativeSubmenu = null;
        if (this.submenu_ != null) {
            this.submenu_['createNative_'](MenuItemTypeCode.submenu);
            nativeSubmenu = this.submenu_['native_'];
        }
        this.native_ = new MenuItemNative(this.role_, this.type_, nativeSubmenu, {
            onClick: () => {
                if (this.type_ === MenuItemTypeCode.checkbox) {
                    this.checked = !this.checked;
                }
                if (this.click != null) {
                    this.click(this, globals.focusedBrowserWindow);
                }
            }
        });
        this.native_.setEnabled(this.enabled_);
        this.native_.setLabel(this.label_);
        this.native_.setChecked(this.checked_);

        const acceleratorTokens = parseAcceleratorToTokens(this.accelerator_);

        this.native_.setAccelerator(acceleratorTokens);
    }
};

