const path = require('path');


module.exports = {
    node: {
        __dirname: false
    },
    target: 'node',
    entry: [
        path.join(__dirname, 'index.ts')
    ],
    output: {
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
