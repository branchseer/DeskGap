const fs = require('fs');
const path = require('path');
const del = require('del');
const { execSync } = require('child_process');

const projectFolder = path.resolve(__dirname, '..');
const npmFolder = path.join(projectFolder, 'npm');
const coreFolder = path.join(projectFolder, 'core');


clean();
build();
execSync('npm publish --dry-run .', { stdio: 'inherit', cwd: npmFolder });


function clean() {
    const deletingPaths = [
        'types',
        'README.md'
    ]
    
    for (const deletingPath of deletingPaths) {
        del.sync(path.join(npmFolder, deletingPath), { force: true });
    }   
}

function build() {
    execSync('npm run build-d-ts', { stdio: 'inherit', cwd: coreFolder });
    fs.copyFileSync(path.join(projectFolder, 'README.md'), path.join(npmFolder, 'README.md'));
}
