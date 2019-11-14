import { BrowserWindow } from './browser-window';
import path = require('path');
import fs = require('fs');
const { dialogNative } = require('./bindings');

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

export type FileOpenDialogProperty = keyof typeof FileOpenDialogPropertyEnum;

export interface IFileOpenDialogOptions extends IFileDialogCommonOptions {
    properties: FileOpenDialogProperty[]
}
export interface IFileSaveDialogOptions extends IFileDialogCommonOptions {
    nameFieldLabel: string | null;
    showsTagField: boolean | null;
}

const FileOpenDialogPropertyEnum = {
    openFile: 1 << 0,
    openDirectory: 1 << 1,
    multiSelections: 1 << 2,
    showHiddenFiles: 1 << 3,
    createDirectory: 1 << 4,
    promptToCreate: 1 << 5,
    noResolveAliases: 1 << 6,
    treatPackageAsDirectory: 1 << 7
};

interface INativeFileDialogCommonOptions extends IFileDialogCommonOptions {
    defaultDirectory: string | null;
    defaultFilename: string | null;
}

interface INativeFileOpenDialogOptions {
    commonOptions: INativeFileDialogCommonOptions;
    propertyBits: number;
}

interface INativeFileSaveDialogOptions {
    commonOptions: INativeFileDialogCommonOptions;
    nameFieldLabel: string | null;
    showsTagField: boolean | null;
}

const prepareCommonOptions = (options:  Partial<IFileDialogCommonOptions>): INativeFileDialogCommonOptions => {
    const result: INativeFileDialogCommonOptions = Object.assign({
        title: null,
        defaultPath: null,
        buttonLabel: null,
        filters: [],
        message: null,
        defaultDirectory: null,
        defaultFilename: null
    }, options);

    if (result.defaultPath != null) {
        let isDirectory = false;
        try {
            const pathInfo = fs.lstatSync(result.defaultPath);
            isDirectory = pathInfo.isDirectory();
        }
        catch (e) { }

        if (isDirectory) {
            result.defaultDirectory = result.defaultPath;
        }
        else {
            const pathDirname = path.dirname(result.defaultPath);
            result.defaultDirectory = pathDirname == '.' ? null : pathDirname;
            result.defaultFilename = path.basename(result.defaultPath);
        }
    }

    for (const filter of result.filters) {
        if (filter.extensions.includes('*')) {
            filter.extensions = [];
        }

        if (filter.extensions.length === 0) {
            filter.name += " (*.*)";
        }
        else {
            filter.name += ` (${filter.extensions.map(ex => `*.${ex}`).join(', ')})`;
        }
    }
    return result;
}

export default class Dialog {
    static showErrorBox(title: string, content: string): void {
        dialogNative.showErrorBox(title, content);
    }

    static showOpenDialog(browserWindow: BrowserWindow, options: Partial<IFileOpenDialogOptions>, callback: (filePaths: string[] | null) => void): void {
        let propertyBits = 0;
        for (const property of options.properties || []) {
            if (property in FileOpenDialogPropertyEnum) {
                propertyBits |= FileOpenDialogPropertyEnum[property];
            }
            else {
                throw new TypeError(`Invalid property for file open dialog: ${property}`);
            }
        }

        if (!(propertyBits & FileOpenDialogPropertyEnum.openDirectory)) {
            propertyBits |= FileOpenDialogPropertyEnum.openFile;
        }

        const commonOptions = prepareCommonOptions(options);

        const nativeOptions: INativeFileOpenDialogOptions = {
            commonOptions,
            propertyBits
        };

        dialogNative.showOpenDialog(browserWindow != null ? browserWindow["native_"] : null, nativeOptions, callback);
    }

    static showOpenDialogAsync(browserWindow: BrowserWindow, options: Partial<IFileOpenDialogOptions>): Promise<{ filePaths: string[] | null}> {
        return new Promise((resolve) => {
            Dialog.showOpenDialog(browserWindow, options, (filePaths) => resolve({ filePaths }));
        });
    }


    static showSaveDialog(browserWindow: BrowserWindow, options: Partial<IFileSaveDialogOptions>, callback: (filePath: string | null) => void): void {
        const commonOptions = prepareCommonOptions(options);
        
        const nativeOptions: INativeFileSaveDialogOptions = Object.assign({
            nameFieldLabel: null,
            showsTagField: null,
        }, options, { commonOptions });

        dialogNative.showSaveDialog(browserWindow != null ? browserWindow["native_"] : null, nativeOptions, callback);
    }

    static showSaveDialogAsync(browserWindow: BrowserWindow, options: Partial<IFileOpenDialogOptions>): Promise<{ filePath: string | null}> {
        return new Promise((resolve) => {
            Dialog.showSaveDialog(browserWindow, options, (filePath) => resolve({ filePath }));
        });
    }
}
