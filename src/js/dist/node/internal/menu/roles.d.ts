import { MenuItemConstructorOptions } from '../../menu';
export declare type Role = 'about' | 'close' | 'copy' | 'cut' | 'delete' | 'front' | 'help' | 'hide' | 'hideothers' | 'minimize' | 'paste' | 'pasteAndMatchStyle' | 'quit' | 'redo' | 'reload' | 'selectAll' | 'services' | 'recentDocuments' | 'clearRecentDocuments' | 'startSpeaking' | 'stopSpeaking' | 'toggleDevTools' | 'toggleFullScreen' | 'undo' | 'unhide' | 'zoom' | 'editMenu' | 'windowMenu';
declare const roleDefaults: Record<string, Partial<MenuItemConstructorOptions>>;
export default roleDefaults;
