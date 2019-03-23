// The whole script will be wrapped into (function() { ... })(), so it's ok to return here.
// The reason of this check is that this preload script is executed whenever IWebBrowser's DISPID_NAVIGATECOMPLETE2 happened,
// This DISPID_NAVIGATECOMPLETE2 event could happen when an html anchor link is clicked, this will cause the preload script 
// being executed in the same page for multiple times.
if (window.deskgap != null) return;

window.deskgap = {
    platform: 'win32',
    postStringMessage: function (string) {
        window.external.post(string);
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
            window.external.drag();
            break;
        }
        currentElement = currentElement.parentElement;
    }
});

window.addEventListener('keydown', function(e) {
    if ((e.keyCode === 187 || e.keyCode === 189) && e.ctrlKey === true) {
        //Preventing ctrl+(+|-) zooming;
        e.preventDefault();
    }
});
window.addEventListener('wheel', function(e) {
    if (e.ctrlKey === true) {
        //Preventing ctrl-scroll zooming
        e.preventDefault();
    }
});
