const path = require('path');
const fs = require('fs');

const inputFilePath = process.argv[2];
const outputFilePath = process.argv[3];

const regularizedFilename = path.basename(inputFilePath).replace(/\W/g, '_').toUpperCase();

const fileContent = fs.readFileSync(inputFilePath);

fs.writeFileSync(outputFilePath, `
char BIN2CODE_${regularizedFilename}_CONTENT[]={${fileContent.join(',')},0};
int BIN2CODE_${regularizedFilename}_SIZE=${fileContent.length};
`);
