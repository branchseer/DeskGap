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
};

(function () {

    var ie8 = window.addEventListener == null;
    var leftButtonCode = ie8 ? 1: 0;

    function on(target, eventName, handlerReturningBool) {
        var handler = function (e) {
            if (handlerReturningBool(e) === false) {
                if (e.preventDefault) {
                    e.preventDefault();
                }
                else {
                    e.returnValue = false;
                }
            }
        }
        if (!ie8) {
            target.addEventListener(eventName, handler);
        }
        else {
            target.attachEvent('on' + eventName, handler);
        }
    }
    
    on(document, 'mousedown', function(e) {
        if (e.button !== leftButtonCode) return;
    
        var currentElement = e.target || e.srcElement;
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
    
    on(document, 'keydown', function (e) {
        if ((e.keyCode === 187 || e.keyCode === 189) && e.ctrlKey === true) {
            //Preventing ctrl+(+|-) zooming;
            return false;
        }
    });
    
    on(document, 'mousewheel', function (e) {
        if (e.ctrlKey === true) {
            //Preventing ctrl-scroll zooming
           return false;
        }
    });
})();
