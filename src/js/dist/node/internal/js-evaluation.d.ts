import { IEventMap, EventEmitter } from '../../common/events';
declare enum ChannelNames {
    RESOLVED = "$5",
    REJECTED = "$6"
}
interface ChannelsSentByUI extends IEventMap {
    [ChannelNames.RESOLVED]: [number, any];
    [ChannelNames.REJECTED]: [number, string];
}
export declare function RegisterJSEvaluationHandlers(messageNode: EventEmitter<ChannelsSentByUI>): void;
export declare function evaluateJavaScript(webViewNative: any, code: string): Promise<any>;
export declare class JavascriptEvaluationError extends Error {
    constructor(message: string);
}
export {};
