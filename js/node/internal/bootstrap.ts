import path = require('path');
const { setNativeExceptionConstructor } = require('../bindings');


//The host program passed [argv[0], deskgap lib path, entry path, serializedPlatformData, ...restOfArgv] to node::Start()
const internalArgv = process.argv.splice(1, 3);

export const entryPath = internalArgv[1];

export class NativeException extends Error {
    constructor(name: string, message: string) {
        super(message);
        this.name = name;
    }
}

setNativeExceptionConstructor(NativeException);

//The native land doesn't have a mechanism to get __dirname, so we need to tell it from the js land.
