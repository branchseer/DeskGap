import { EventEmitter } from '../common/events';
import { WebView } from './webview';
declare const messageNode: EventEmitter<any, WebView>;
export default messageNode;
