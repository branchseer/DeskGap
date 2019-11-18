import { EventEmitter, IEventMap } from '../common/events';

const spNative = require('./bindings').systemPreferencesNative;

export interface UserDefaultTypes {
    'string': string;
    'array': any[];
    'dictionary': object;
    'boolean': boolean;
    'integer': number;
    'float': number;
    'double': number;
    'url': string
}

const userDefaultGetters = {
    'string': spNative.getUserDefaultString,
    'array': spNative.getUserDefaultArrayJSON,
    'dictionary': spNative.getUserDefaultDictionaryJSON,
    'boolean': spNative.getUserDefaultBool,
    'integer': spNative.getUserDefaultInteger,
    'float': spNative.getUserDefaultFloat,
    'double': spNative.getUserDefaultDouble,
    'url': spNative.getUserDefaultURL
};

export interface SystemPreferenceEvents extends IEventMap {
    'dark-mode-toggled': []
}

let isDarkMode: boolean;

export class SystemPreference extends EventEmitter<SystemPreferenceEvents> {
    /** @internal */ private isDarkMode_: boolean;
    constructor() {
        super();
        this.isDarkMode_ = <boolean>spNative.getAndWatchDarkMode(() => {
            this.isDarkMode_ = !this.isDarkMode_;
            this.trigger_('dark-mode-toggled');
        });
    }
    getUserDefault<K extends keyof UserDefaultTypes>(key: string, type: K): UserDefaultTypes[K]  {
        const result = userDefaultGetters[type](key);
        if (type === 'array' || type === 'dictionary') {
            return JSON.parse(result);
        }
        else {
            return result;
        }
    }
    isDarkMode(): boolean {
        return this.isDarkMode_;
    }
}

const systemPreference = new SystemPreference();

export default systemPreference;
