//Import the original EventEmitter, but declaring it not having any methods, so there won't be conflicts with the strong-typed methods (on, emit...)
declare interface VanillaEventEmitterConstructor {
    new(): { };
}
const VanillaEventEmitter = require('events') as VanillaEventEmitterConstructor;

export interface IEventMap
{
    [eventType: string]: any[]
}

export interface IEventObject<Sender> {
    readonly sender: Sender;
    preventDefault(): void;
}

/**
 * @param EventMap The mapping between event names and args for listeners.
 * @param Sender The type of `e.sender`, the emitter itself by default.
 */
export declare interface EventEmitter<EventMap extends IEventMap, Sender = null> {
    setMaxListeners(n: number): this;

    emit<K extends keyof EventMap>(eventName: K, e: IEventObject<Sender extends null ? this: Sender>, ...args: EventMap[K]): boolean;

    on<K extends keyof EventMap>(eventName: K, listener: (e: IEventObject<Sender extends null ? this: Sender>, ...args: EventMap[K]) => void): this;
    once<K extends keyof EventMap>(eventName: K, listener: (e: IEventObject<Sender extends null ? this: Sender>, ...args: EventMap[K]) => void): this;

    removeListener<K extends keyof EventMap>(eventName: K, listener: (e: IEventObject<Sender extends null ? this: Sender>, ...args: EventMap[K]) => void): this;
    removeAllListeners<K extends keyof EventMap>(eventName?: K): this;
    listeners<K extends keyof EventMap>(eventName: K): Array<(e: IEventObject<Sender extends null ? this: Sender>, ...args: EventMap[K]) => void>;
    listenerCount<K extends keyof EventMap>(eventName: K): number;
};

export interface TriggerOptions<Sender> {
    sender?: Sender;
    defaultAction?: () => void;
}

export class EventEmitter<EventMap extends IEventMap, Sender = null> extends VanillaEventEmitter {
    /** @internal */ 
    protected trigger_<K extends keyof EventMap>(eventName: K, options?: TriggerOptions<Sender extends null ? this: Sender> | null, ...args: EventMap[K]): boolean {
        const theOptions = options || { };
        const sender = theOptions.sender || this;
        let isDefaultPrevented = false;
        const eventObject = {
            get sender() { return sender; },
            preventDefault() { isDefaultPrevented = true },
        }
        
        try {
            return this.emit(eventName, eventObject as any, ...args);
        }
        finally {
            if (!isDefaultPrevented) {
                if (theOptions.defaultAction != null) theOptions.defaultAction();
            }
        }
    }
}
