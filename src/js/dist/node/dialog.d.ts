import { BrowserWindow } from './browser-window';
export interface IFileFilter {
    name: string;
    extensions: string[];
}
export interface IFileDialogCommonOptions {
    title: string | null;
    defaultPath: string | null;
    buttonLabel: string | null;
    filters: IFileFilter[];
    message: string | null;
}
export declare type FileOpenDialogProperty = keyof typeof FileOpenDialogPropertyEnum;
export interface IFileOpenDialogOptions extends IFileDialogCommonOptions {
    properties: FileOpenDialogProperty[];
}
export interface IFileSaveDialogOptions extends IFileDialogCommonOptions {
    nameFieldLabel: string | null;
    showsTagField: boolean | null;
}
declare const FileOpenDialogPropertyEnum: {
    openFile: number;
    openDirectory: number;
    multiSelections: number;
    showHiddenFiles: number;
    createDirectory: number;
    promptToCreate: number;
    noResolveAliases: number;
    treatPackageAsDirectory: number;
};
export default class Dialog {
    static showErrorBox(title: string, content: string): void;
    static showOpenDialog(browserWindow: BrowserWindow, options: Partial<IFileOpenDialogOptions>, callback: (filePaths: string[] | null) => void): void;
    static showOpenDialogAsync(browserWindow: BrowserWindow, options: Partial<IFileOpenDialogOptions>): Promise<{
        filePaths: string[] | null;
    }>;
    static showSaveDialog(browserWindow: BrowserWindow, options: Partial<IFileSaveDialogOptions>, callback: (filePath: string | null) => void): void;
    static showSaveDialogAsync(browserWindow: BrowserWindow, options: Partial<IFileOpenDialogOptions>): Promise<{
        filePath: string | null;
    }>;
}
export {};
