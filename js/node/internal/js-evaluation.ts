import { IEventMap, EventEmitter } from '../../common/events';

enum ChannelNames {
    RESOLVED = '$5',
    REJECTED = '$6',
}

interface ChannelsSentByUI extends IEventMap {
    [ChannelNames.RESOLVED]: [/* evaluation id */number, any];
    [ChannelNames.REJECTED]: [/* evaluation id */number, /* error message */string];
}

let lastEvaluationId = 0;

const evaluations = new Map<number, { resolve: (result: any) => void, reject: (error: any) => void }>();

export function RegisterJSEvaluationHandlers(messageNode: EventEmitter<ChannelsSentByUI>) {
    messageNode.on(ChannelNames.RESOLVED, (e, evaluationId, result) => {
        const evaluation = evaluations.get(evaluationId);
        if (evaluation != null) {
            evaluation.resolve(result);
            evaluations.delete(evaluationId);
        }
    });
    messageNode.on(ChannelNames.REJECTED, (e, evaluationId: number, errorMessage: string) => {
        const evaluation = evaluations.get(evaluationId);
        if (evaluation != null) {
            evaluation.reject(new JavascriptEvaluationError(errorMessage));
            evaluations.delete(evaluationId);
        }
    });
}

export function evaluateJavaScript(webViewNative: any, code: string): Promise<any> {
    const evaluationId = ++lastEvaluationId;
    const wrappedCode = `Promise.resolve(${code}).then(
        function (result) { window.deskgap.messageUI.send('${ChannelNames.RESOLVED}', ${evaluationId}, result); },
        function (error) { window.deskgap.messageUI.send('${ChannelNames.REJECTED}', ${evaluationId}, error.message || error.toString()); }
    ); void 0;`;

    return new Promise<any>((resolve, reject) => {
        evaluations.set(evaluationId, { resolve, reject });
        webViewNative.executeJavaScript(wrappedCode, (errorMessage: string | null) => {
            if (errorMessage != null) { // Synchronous error happened.
                evaluations.delete(evaluationId);
                reject(new JavascriptEvaluationError(errorMessage));
            }
        });
    });
};


export class JavascriptEvaluationError extends Error {
    constructor(message: string) {
        super(message);
        this.name = "JavascriptEvaluationError";
    }
}
