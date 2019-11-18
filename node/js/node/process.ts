const { appNative } = require('./bindings');

declare global {
    namespace NodeJS {
        interface Process {
            resourcesPath: string;
        }
    }
}

process.resourcesPath = appNative.getResourcePath();
process.argv = appNative.getArgv();

export { }
