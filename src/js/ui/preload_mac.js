window.deskgap = {
    platform: 'darwin',
    postStringMessage: function (string) {
        window.webkit.messageHandlers.stringMessage.postMessage(string);
    }
}

window.addEventListener('mousedown', function(e) {
    if (e.button !== 0) return;

    var currentElement = e.target;
    while (currentElement != null) {
        if (currentElement.hasAttribute('data-deskgap-no-drag')) {
            break;
        }
        else if (currentElement.hasAttribute('data-deskgap-drag')) {
            window.webkit.messageHandlers.windowDrag.postMessage(null);
        }
        currentElement = currentElement.parentElement;
    }
});
