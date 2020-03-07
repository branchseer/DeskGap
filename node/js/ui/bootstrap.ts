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
