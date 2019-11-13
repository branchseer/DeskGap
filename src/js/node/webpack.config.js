const path = require('path');

const mode = process.env.NODE_ENV || 'production';

module.exports = {
    mode,
    node: {
        __dirname: false
    },
    target: 'node',
    devtool: mode === 'production' ? false: 'inline-source-map',
    entry: [
        path.join(__dirname, 'index.ts')
    ],
    output: {
        path: path.resolve(__dirname, '..', '..', 'dist', 'node'),
        filename: 'index.js',
        libraryTarget: 'commonjs2',
        libraryExport: ''
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
