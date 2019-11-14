import { EventEmitter, IEventMap } from '../common/events';
import { WebView } from './webview';
import { RegisterAsyncNodeHandlers } from './internal/async-node';
import { RegisterJSEvaluationHandlers } from './internal/js-evaluation';

const messageNode = new EventEmitter<any, WebView>();

RegisterAsyncNodeHandlers(messageNode);
RegisterJSEvaluationHandlers(messageNode);

export default messageNode;
