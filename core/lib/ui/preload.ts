import { messageUI, internalDeskGap } from './bootstrap';
import asyncNode from './async-node';

export class UIDeskGap {
    readonly platform = <'darwin' | 'win32'>internalDeskGap.platform;
    readonly ipcRenderer = messageUI;
    readonly messageUI = messageUI;
    readonly asyncNode = asyncNode;
}

declare global {
    interface Window {
        deskgap: UIDeskGap;
    }
}

window.deskgap = new UIDeskGap();

//Re-expose the messageReceived which is defined in message.ts
(<any>window).deskgap.__messageReceived = internalDeskGap.messageReceived;
