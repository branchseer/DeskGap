const { setNativeExceptionConstructor } = require('./bindings');

export default class NativeException extends Error {
    constructor(name: string, message: string) {
        super(message);
        this.name = name;
    }
}

setNativeExceptionConstructor(NativeException);
