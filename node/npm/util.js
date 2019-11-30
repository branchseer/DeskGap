const nugget = require('nugget');
const crypto = require('crypto');
const fs = require('fs');
const path = require('path');

exports.downloadFile = (filename, target) => new Promise((resolve, reject) => {
    const distFolder = process.env.DESKGAP_DIST_FOLDER;
    if (distFolder != null) {
        fs.copyFileSync(path.join(distFolder, filename), target);
        return resolve();
    }

    const opts = { target };
    const bintrayUser = process.env.DESKGAP_BINTRAY_USER;
    const bintrayKey = process.env.DESKGAP_BINTRAY_KEY;
    if (bintrayUser != null && bintrayKey != null) {
    //For testing unpublished binaries
        opts.headers = {
            Authorization: "Basic " + new Buffer(bintrayUser + ":" + bintrayKey).toString('base64')
        }
    }

    nugget("https://dl.bintray.com/patr0nus/DeskGap/" + filename, opts, (errors) => {
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
