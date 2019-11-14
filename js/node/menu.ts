import { bulkUISync } from './internal/dispatch';
import { parseAcceleratorToTokens } from './accelerator';
import globals from './internal/globals';
import { BrowserWindow } from './browser-window'
import roleDefaults, { Role } from './internal/menu/roles';

interface IMenuItemNative {
    setEnabled(enabled: boolean): void;
    setLabel(label: string): void;
    setChecked(checked: boolean): void;
    setAccelerator(acceleratorTokens: string[]): void;
    destroy(): void;
}

interface IMenuNative {
    append(item: IMenuItemNative): void;
    destroy(): void;
}

const { MenuItemNative, MenuNative } = require('./bindings') as {
    MenuItemNative: { new(role: string, typeCode: number, submenu: IMenuNative | null, onClick: () => void): IMenuItemNative };
    MenuNative: { new(typeCode: number, callbacks: {}): IMenuNative }
}

export type MenuItemType = 'normal' | 'separator' | 'submenu' | 'checkbox';

/** @internal */ 
const MenuItemTypeCode = {
    normal: 0, separator: 1, submenu: 2, checkbox: 3
};

/** @internal */ 
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

let lastNativeId: number = 0;

export class Menu {
    /** @internal */ private natives_ = new Map<number, IMenuNative>();
    public items: MenuItem[] = [];

    /** @internal */ private nativeCallbacks_ = {};
    
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

        let nativeId: number = 0;
        bulkUISync(() => {
            if (fullOptions.window == null) {
                throw new Error('No window specified for Menu#popup, and there is no focused window');
            }
            const result = this.createNative_(MenuTypeCode.context, fullOptions.window);
            nativeId = result[0];
            const native = result[1];
            fullOptions.window['native_'].popupMenu(native, location, fullOptions.positioningItem, () => {
                bulkUISync(() => {
                    this.destroyNative_(nativeId);
                });
            });    
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

    /** @internal */ 
    private createNative_(type: number, window: BrowserWindow | null, theNativeId?: number): [number, IMenuNative] {
        const nativeId = theNativeId || ++lastNativeId;
        //console.log(nativeId);

        const native = new MenuNative(type, this.nativeCallbacks_);
        this.natives_.set(nativeId, native);
        
        for (const item of this.items) {
            const nativeMenuItem = item['createNative_'](nativeId, window);
            native.append(nativeMenuItem);
        }

        return [nativeId, native];
    }
    /** @internal */
    private destroyNative_(nativeId: number): void {
        for (const item of this.items) {
            item['destroyNative_'](nativeId);
        }
        const native = this.natives_.get(nativeId)!;
        native.destroy();
        this.natives_.delete(nativeId);
    }

};

export class MenuItem {
    /** @internal */ private label_: string;
    /** @internal */ private enabled_: boolean;
    /** @internal */ private type_: number;
    public click: (item: MenuItem, window: BrowserWindow | null) => void;
    /** @internal */ private submenu_: Menu | null;
    /** @internal */ private natives_ = new Map<number, IMenuItemNative>();
    /** @internal */ private checked_: boolean;
    /** @internal */ private accelerator_: string;
    /** @internal */ private role_: string;

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
        bulkUISync(() => {
            for (const native of this.natives_.values()) {
                native.setEnabled(value);
            };
        });
        this.enabled_ = value;
    }
    get checked(): boolean {
        return this.checked_;
    }
    set checked(value: boolean) {
        bulkUISync(() => {
            for (const native of this.natives_.values()) {
                native.setChecked(value);
            }
        });
        this.checked_ = value
    }
    get accelerator(): string {
        return this.accelerator_;
    }
    
    /** @internal */ 
    private createNative_(nativeId: number, window: BrowserWindow | null): IMenuItemNative {

        let nativeSubmenu: IMenuNative | null = null;
        if (this.submenu_ != null) {
            [, nativeSubmenu] = this.submenu_['createNative_'](MenuItemTypeCode.submenu, window, nativeId);
        }

        const native = new MenuItemNative(this.role_, this.type_, nativeSubmenu, () => {
            if (this.type_ === MenuItemTypeCode.checkbox) {
                this.checked = !this.checked;
            }
            if (this.click != null) {
                this.click(this, window || globals.focusedBrowserWindow);
            }
        });

        native.setEnabled(this.enabled_);
        native.setLabel(this.label_);
        native.setChecked(this.checked_);

        const acceleratorTokens = parseAcceleratorToTokens(this.accelerator_);
        native.setAccelerator(acceleratorTokens);

        this.natives_.set(nativeId, native);

        return native;
    }

    /** @internal */
    private destroyNative_(nativeId: number): void {
        const native = this.natives_.get(nativeId)!;
        native.destroy();
        this.natives_.delete(nativeId);
        if (this.submenu_ != null) {
            this.submenu_['destroyNative_'](nativeId);
        }
    }
};
