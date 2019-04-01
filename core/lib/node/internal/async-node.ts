import { WebView } from '../webview';
import { entryPath } from './bootstrap'
import { 
    ChannelNames, GLOBAL_OBJECT_ID, CURRENT_WINDOW_ID, CURRENT_WEBVIEW_ID,
    INodeValueRequest,
    ChannelsSentByUI, ChannelsSentByNode, NodeValueActionType, Value, ValueType, NodeValueInvokeReceiverType
} from '../../common/async-node';
import { BrowserWindow } from '../browser-window';

import path = require('path');
import moduleUtils = require('module');

function assertUnreachable(x: never) { }


const getValue = async (value: Value, sender: WebView): Promise<any> => {
    switch (value[0]) {
        case ValueType.VALUE_REQUEST: return await getValueFromRequest(value[1], sender);
        case ValueType.ARBITRARY: return value[1];
        default: assertUnreachable(value);
    }
}

const getValues = (values: Value[], sender: WebView): Promise<any[]> => {
    return Promise.all(values.map(val => getValue(val, sender)));
}

const getValueFromRequest = async (request: INodeValueRequest, sender: WebView): Promise<any> => {
    const { target, actions } = request;

    let currentValue;
    
    if (target === GLOBAL_OBJECT_ID) {
        currentValue = global;
    }
    else if (target === CURRENT_WINDOW_ID) {
        currentValue = BrowserWindow.fromWebView(sender);
    }
    else if (target === CURRENT_WEBVIEW_ID) {
        currentValue = sender;
    }
    else {
        currentValue =sender["asyncNodeObjectsById_"].get(target);
    }

    for (const action of actions) {
        switch (action[0]) {
        case NodeValueActionType.PROP:
            const [, propName] = action;
            currentValue = currentValue[propName];
            break;
        case NodeValueActionType.INVOKE:
            const [, receiver, args] = action;
            switch (receiver[0]) {
            case NodeValueInvokeReceiverType.METHOD:
                const [, methodName] = receiver;
                currentValue = currentValue[methodName](...await getValues(args, sender));
                break;
            case NodeValueInvokeReceiverType.NEW:
                currentValue = new currentValue(...await getValues(args, sender));
                break;
            case NodeValueInvokeReceiverType.VALUE:
                const [, thisValue] = receiver;
                currentValue = currentValue.apply(await getValue(thisValue, sender), await getValues(args, sender));
                break;
            default:
                assertUnreachable(receiver);
            }
            break;
        case NodeValueActionType.RESOLVE:
            currentValue = await currentValue;
            break;
        default:
            assertUnreachable(action);
        }
    }
    return currentValue;
}

export function RegisterAsyncNodeHandlers(untypedMessageNode: any) {
    const messageNode = untypedMessageNode as {
        on<K extends keyof ChannelsSentByUI>(eventName: K, listener: (e: {
            sender: WebView & { send<C extends keyof ChannelsSentByNode>(channelName: C, ...args: ChannelsSentByNode[C]): void }
        }, ...args: ChannelsSentByUI[K]) => void): typeof messageNode;
    }


    messageNode.on(ChannelNames.GET_VALUE, (e, requestId, request) => {
        getValueFromRequest(request, e.sender).then(result => {
            e.sender.send(ChannelNames.RESULT, requestId, result);
        }, error => {
            e.sender.send(ChannelNames.RESULT, requestId, new String(error), true);
        });
    });

    messageNode.on(ChannelNames.GET_OBJECT, (e, requestId, request) => {
        getValueFromRequest(request, e.sender).then(result => {
            e.sender['asyncNodeObjectsById_'].set(requestId, result);
            e.sender.send(ChannelNames.RESULT, requestId, requestId);
        }, error => {
            e.sender.send(ChannelNames.RESULT, requestId, new String(error), true);
        });
    });

    //createRequireFromPath accepts a path and creates a require function that acts as if it's called in the file of that path.
    //The path must be in a form of file, but it doesn't matter if it exists.
    const requireFromEntry = moduleUtils.createRequireFromPath(path.join(entryPath, 'file.js'));

    messageNode.on(ChannelNames.GET_MODULE_OBJECT, (e, requestId, moduleName) => {
        try {
            let theModuleName = moduleName;

            const moduleObject = requireFromEntry(theModuleName);
            e.sender['asyncNodeObjectsById_'].set(requestId, moduleObject);
            e.sender.send(ChannelNames.RESULT, requestId, requestId);
        }
        catch (error) {
            e.sender.send(ChannelNames.RESULT, requestId, new String(error), true);
        } 
    });
}
