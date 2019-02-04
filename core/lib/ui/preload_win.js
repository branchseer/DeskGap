window.deskgap = {
    platform: 'win32',
    postStringMessage: function (string) {
        window.external.notify('m' + string);
    }
}

window.addEventListener('mousedown', function(e) {
    var currentElement = e.target;
    while (currentElement != null) {
        if (currentElement.hasAttribute('data-deskgap-no-drag')) {
            break;
        }
        else if (currentElement.hasAttribute('data-deskgap-drag')) {
            window.setImmediate(function() {
                window.external.notify('d')
            });
            break;
        }
        currentElement = currentElement.parentElement;
    }
});

document.addEventListener("DOMContentLoaded", function () {
    window.external.notify("t" + document.title);
    new MutationObserver(function () {
        window.external.notify("t" + document.title);
    }).observe(
        document.querySelector('title'),
        { characterData: true, childList: true }
    );
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
