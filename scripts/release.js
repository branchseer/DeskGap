const path = require('path');
const fse = require('fs-extra');
const fs = require('fs');
const { execSync } = require('child_process');

const projectFolder = path.resolve(__dirname, '..');

const buildFolder = require('./build-folder');
const coreFolder = path.join(projectFolder, 'core');
const npmFolder = path.join(projectFolder, 'npm');

clean();
build();
archive();

function clean() {
    const deletingPaths = [ 
        buildFolder,
        path.join(coreFolder, 'dist'), path.join(coreFolder, 'build'),
        path.join(npmFolder, 'types')
    ]
    
    for (const deletingPath of deletingPaths) {
        fse.removeSync(path.resolve(projectFolder, deletingPath));
    }   
}

function build() {
    fs.mkdirSync(buildFolder, { recursive: true });

    runCommands([
        'npm run configure-native',
        'npm run build-native',
        'npm run build-lib-node',
        'npm run build-lib-ui'
    ], coreFolder);
    
    const cmakeConfigureCommands = {
        "darwin": `cmake -D CMAKE_OSX_DEPLOYMENT_TARGET:STRING=10.10 -G Xcode ${JSON.stringify(projectFolder)}`,
        "win32": `cmake -G "Visual Studio 15 2017" ${JSON.stringify(projectFolder)}`,
        "linux": `cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ${JSON.stringify(projectFolder)}`
    };

    const cmakeBuildCommand = (process.platform === 'linux' ? 'make': 'cmake --build . --target ALL_BUILD --config Release');
    
    runCommands([
        cmakeConfigureCommands[process.platform],
        cmakeBuildCommand
    ], buildFolder);
    
    
    function runCommands(commands, cwd) {
        for (const command of commands) {
            execSync(command, { stdio: 'inherit', cwd });
        }
    }    
}


function archive() {
    const npmPackageJSON = require(path.join(npmFolder, 'package.json'));
    const platformAndArch = `${process.platform}-${process.arch}`;
    const zipFileName = `deskgap-v${npmPackageJSON.version}-${platformAndArch}.zip`
    const zipFullPath = path.join(buildFolder, zipFileName);
    
    const archiver = require('archiver');
    const crypto = require('crypto');
    
    const archive = archiver('zip').once('warning', (err) => {
        throw err;
    });
    
    const buildReleaseFolder = path.join(buildFolder, 'Release');
    if (process.platform === 'darwin') {
        archive.glob('DeskGap.app/**', {
            cwd: buildReleaseFolder,
            ignore: ['.DS_Store']
        });
    }
    else {
        archive.glob('DeskGap/**', {
            cwd: buildReleaseFolder,
            ignore: ['.DS_Store']
        });
        if (process.platform === 'win32') {
            const vcRedisDllFolder = 'SysWOW64';
            for (const vcRedisDll of ['vcruntime140.dll', 'msvcp140.dll']) {
                archive.file(path.join(process.env.SYSTEMROOT, vcRedisDllFolder, vcRedisDll), { name: 'DeskGap/' + vcRedisDll });
            }
        }
    }
    
    archive.pipe(crypto.createHash('sha256')).on('readable', function () {
        const data = this.read();
        if (data) {
            const sha256hex = data.toString('hex');
    
            const npmDistFileFolder = path.join(npmFolder, 'dist_files');
            fse.mkdirpSync(npmDistFileFolder);
            fs.writeFileSync(
                path.join(npmDistFileFolder, platformAndArch + ".json"),
                JSON.stringify({
                    filename: zipFileName,
                    sha256: sha256hex
                }),
                'utf8'
            );
        }
    });
    archive.pipe(fs.createWriteStream(zipFullPath));
    archive.finalize();    
}
