const path = require('path');

const mode = process.env.NODE_ENV || 'production';

module.exports = {
    mode,
    devtool: mode === 'production' ? false: 'inline-source-map',
    entry: [
        path.join(__dirname, 'preload.ts')
    ],
    output: {
        path: path.resolve(__dirname, '..', '..', 'dist', 'ui'),
        filename: `preload.js`
    },
    resolve: {
        extensions: [".ts", ".js" ]
    },
    module: {
        rules: [{
            test: /\.ts$/,
            exclude: /node_modules/,
            loader: 'ts-loader'
        }]
    }
};
