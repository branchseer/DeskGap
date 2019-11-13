import { EventEmitter, IEventMap } from '../common/events';
export interface UserDefaultTypes {
    'string': string;
    'array': any[];
    'dictionary': object;
    'boolean': boolean;
    'integer': number;
    'float': number;
    'double': number;
    'url': string;
}
export interface SystemPreferenceEvents extends IEventMap {
    'dark-mode-toggled': [];
}
export declare class SystemPreference extends EventEmitter<SystemPreferenceEvents> {
    private isDarkMode_;
    constructor();
    getUserDefault<K extends keyof UserDefaultTypes>(key: string, type: K): UserDefaultTypes[K];
    isDarkMode(): boolean;
}
declare const systemPreference: SystemPreference;
export default systemPreference;
