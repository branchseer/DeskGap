import { internalDeskGap } from './bootstrap'
import JSONTalk, { IServices, IServiceClient } from 'json-talk'

const jsonTalkServices: IServices = { }
const jsonTalk = new JSONTalk<any>((message) => {
    internalDeskGap.postStringMessage(JSON.stringify(message));
}, jsonTalkServices);

export class DeskGapInBroswer<Services extends IServices> {
    readonly platform = <'darwin' | 'win32' | 'linux'>internalDeskGap.platform;
    publishServices(services: IServices) {
        //No Object.assign in IE11
        for (const key of Object.keys(services)) {
            jsonTalkServices[key] = services[key];
        }
    }
    getService<ServiceName extends (keyof Services & string)>(serviceName: ServiceName): IServiceClient<Services[ServiceName]> {
        return jsonTalk.connectService(serviceName)
    }
}

declare global {
    interface Window {
        deskgap: DeskGapInBroswer<any>;
    }
}

window.deskgap = new DeskGapInBroswer();

// Receiving message from the node land, called in webview.ts
Object.defineProperty(window.deskgap, "__messageReceived", {
    value: (msg: any) => { jsonTalk.feedMessage(msg) }
});
