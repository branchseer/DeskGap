import { EventEmitter } from '../common/events'

declare var window: any;

//This is the interface of window.deskgap in preload_${platform}.js
interface InternalDeskGap {
    //These 2 are previously defined in a platform-specific manner.
    platform: string;
    postStringMessage: (message: string) => void;

    //This is to be defined in this file.
    messageReceived: (channelName: string, args: any[]) => void;
}


//preload.ts will modify window.deskgap so we need to keep a reference here.
export const internalDeskGap: InternalDeskGap = window.deskgap;

export class MessageUI extends EventEmitter<any> {
    constructor() {
        super();
        internalDeskGap.messageReceived = (channelName, args) => {
            this.trigger_(channelName, null, ...args);
        };
    }
    /**
     * Send a message to the node thread asynchronously via channel, you can also send arbitrary arguments.
     * Arguments will be serialized in JSON internally and hence no functions or prototype chain will be included.
     * The node thread handles it by listening for channel with messageNode.
     * @param channel Channel name. Do not use channel names that are prefixed by `$`. They are reserved for internal use.
     * @param args Arguments
     */
    send(channel: string | number, ...args: any[]): void {
        const message = [channel, args];
        const serializedMessage = JSON.stringify(message);
        internalDeskGap.postStringMessage(serializedMessage);
    }
}

export const messageUI = new MessageUI();
