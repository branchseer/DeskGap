import { messageUI as untypedMessageUI } from './bootstrap';

import { 
    ChannelNames, GLOBAL_OBJECT_ID, CURRENT_WINDOW_ID,
    INodeValueRequest,
    ChannelsSentByUI, ChannelsSentByNode, NodeValueAction, NodeValueActionType, NodeValueInvokeReceiverType, Value, ValueType
} from '../common/async-node';

const messageUI = untypedMessageUI as {
    on<K extends keyof ChannelsSentByNode>(eventName: K, listener: (e: any, ...args: ChannelsSentByNode[K]) => void): typeof messageUI;
    send<K extends keyof ChannelsSentByUI>(name: K, ...args: ChannelsSentByUI[K]): void;
}

let currentId = 0;

const requestsByID = new Map<number, {
    resolve: (value: any) => void,
    reject: (error: Error) => void
}>();

class NodeError extends Error { }

messageUI.on(ChannelNames.RESULT, (e, id, result, isResultError) => {
    const request = requestsByID.get(id)!;
    requestsByID.delete(id);
    if (isResultError) {
        request.reject(new NodeError(result));
    }
    else {
        request.resolve(result);
    }
});

export class NodeValue {
    private readonly request_: INodeValueRequest
    private constructor(requestOrId: INodeValueRequest | number) {
        if (typeof requestOrId === 'number') {
            this.request_ = { target: requestOrId, actions: [] };
        }
        else {
            this.request_ = requestOrId;
        }
    }
    private appendingActions(...actions: NodeValueAction[]): NodeValue {
        return new NodeValue({
            target: this.request_.target,
            actions: this.request_.actions.concat(actions)
        })
    }
    private static getArgValues(params: any[]): Value[] {
        return params.map((param): Value => {
            if (param instanceof NodeValue) {
                return [ValueType.VALUE_REQUEST, param.request_];
            }
            else {
                return [ValueType.ARBITRARY, param];
            }
        });
    }

    prop(...names: string[]): NodeValue {  
        return this.appendingActions(...names.map((name): NodeValueAction => [NodeValueActionType.PROP, name]));
    }
    invoke(methodName: string, ...params: any[]): NodeValue {
        return this.appendingActions([NodeValueActionType.INVOKE, [ NodeValueInvokeReceiverType.METHOD, methodName ], NodeValue.getArgValues(params)]);
    }
    construct(...params: any[]) {
        return this.appendingActions([NodeValueActionType.INVOKE, [ NodeValueInvokeReceiverType.NEW ], NodeValue.getArgValues(params)]);
    }
    resolve() {
        return this.appendingActions([NodeValueActionType.RESOLVE]);
    }

    value(): Promise<any> {
        const id = currentId;
        currentId++;

        const result = new Promise<any>((resolve, reject) => {
            requestsByID.set(id, { resolve, reject });
        });
        messageUI.send(ChannelNames.GET_VALUE, id, this.request_);
        return result;
    }

    object(): Promise<NodeValue> {
        const id = currentId;
        currentId++;
        const result = new Promise<NodeValue>((resolve, reject) => {
            requestsByID.set(id, { resolve: (objectId: number) => resolve(new NodeValue(objectId)), reject });
        });
        messageUI.send(ChannelNames.GET_OBJECT, id, this.request_);
        return result;
    }

    static readonly globalObject = new NodeValue(GLOBAL_OBJECT_ID);
    static readonly currentWindow = new NodeValue(CURRENT_WINDOW_ID);

    static require(moduleName: string): Promise<NodeValue> {
        const id = currentId;   
        currentId++;

        const result = new Promise<NodeValue>((resolve, reject) => {
            requestsByID.set(id, { resolve: (objectId: number) => resolve(new NodeValue(objectId)), reject });
        });
        messageUI.send(ChannelNames.GET_MODULE_OBJECT, id, moduleName);
        return result;
    }
}

export default {
    getGlobal(name: string): NodeValue {
        return NodeValue.globalObject.prop(name);
    },
    require(moduleName: string): Promise<NodeValue> {
        return NodeValue.require(moduleName);
    },
    getCurrentWindow(): NodeValue {
        return NodeValue.currentWindow;
    }
}
