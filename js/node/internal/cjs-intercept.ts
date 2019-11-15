//Reference: https://github.com/electron/electron/blob/24b3d66767d89c0b119d1fcc738db4b7f456d913/lib/common/reset-search-paths.ts

import path = require('path');

const Module = require('module')

// Clear Node's global search paths.
Module.globalPaths.length = 0

// Prevent Node from adding paths outside this app to search paths.
const resourcesPathWithTrailingSlash = process.resourcesPath + path.sep
const originalNodeModulePaths = Module._nodeModulePaths
Module._nodeModulePaths = function (from: string) {
  const paths: string[] = originalNodeModulePaths(from)
  const fromPath = path.resolve(from) + path.sep
  // If "from" is outside the app then we do nothing.
  if (fromPath.startsWith(resourcesPathWithTrailingSlash)) {
    return paths.filter(function (candidate) {
      return candidate.startsWith(resourcesPathWithTrailingSlash)
    })
  } else {
    return paths
  }
}


const originalResolveFilename = Module._resolveFilename
Module._resolveFilename = function (request: string, parent: NodeModule, isMain: boolean) {
  if (request === 'deskgap') {
    return 'deskgap'
  } else {
    return originalResolveFilename(request, parent, isMain)
  }
}

export function registerModule(deskgap: any): void {
    // Make a fake DeskGap module that we will insert into the module cache
    const deskgapModule = new Module('deskgap', null)
    deskgapModule.id = 'deskgap'
    deskgapModule.loaded = true
    deskgapModule.filename = 'deskgap'
    Object.defineProperty(deskgapModule, 'exports', {
        get: () => deskgap
    })

    Module._cache['deskgap'] = deskgapModule
}
