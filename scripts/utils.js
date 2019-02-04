const nugget = require('nugget');

exports.downloadFile = (url, target) => new Promise((resolve, reject) => {
    nugget(url, { target }, (errors) => {
        if (errors) {
            reject(errors[0]);
        }
        else {
            resolve();
        }
    });
});
