const nugget = require('nugget');
const crypto = require('crypto');
const fs = require('fs');

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

exports.sha256OfPath = (path) => {
    return new Promise((resolve, reject) => {
        fs.createReadStream(path)
            .once('error', reject)
            .pipe(crypto.createHash('sha256'))
            .on('readable', function () {
                const data = this.read();
                if (data) {
                    resolve(data.toString('hex'));
                }
            });
    });
};
