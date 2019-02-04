import * as AppModule from '../../app';
import appInfo from '../app-info';

const lazyApp = (): AppModule.App => {
  const theAppModule = require('../../app') as typeof AppModule;
  return theAppModule.default;
};

import { MenuItemConstructorOptions } from '../../menu'

export type Role = 'about' | 'close' | 'copy' | 'cut' | 'delete' | 'front' | 'help' | 'hide' |
    'hideothers' | 'minimize' | 'paste' | 'pasteAndMatchStyle' | 'quit' | 'redo' | 'reload' | 
    'selectAll' | 'services' | 'recentDocuments' | 'clearRecentDocuments' | 'startSpeaking' |
    'stopSpeaking' | 'toggleDevTools' | 'toggleFullScreen' | 'undo' | 'unhide' | 'zoom' |
    'editMenu' | 'windowMenu';

const roleDefaults: Record<string, Partial<MenuItemConstructorOptions>> = {
    about: {
      get label() {
        return process.platform === 'linux' ? 'About' : `About ${appInfo.name}`
      }
    },
    close: {
      label: process.platform === 'darwin' ? 'Close Window' : 'Close',
      accelerator: 'CommandOrControl+W',
      click(item, window) {
        window.close();
      }
    },
    copy: {
      label: 'Copy',
      accelerator: 'CommandOrControl+C',
    },
    cut: {
      label: 'Cut',
      accelerator: 'CommandOrControl+X',
    },
    delete: {
      label: 'Delete',
    },
    front: {
      label: 'Bring All to Front'
    },
    help: {
      label: 'Help'
    },
    hide: {
      get label() { return `Hide ${appInfo.name}` },
      accelerator: 'Command+H'
    },
    hideothers: {
      label: 'Hide Others',
      accelerator: 'Command+Alt+H'
    },
    minimize: {
      label: 'Minimize',
      accelerator: 'CommandOrControl+M',
      click(item, window) {
        window.minimize();
      }
    },
    paste: {
      label: 'Paste',
      accelerator: 'CommandOrControl+V',
    },
    pasteandmatchstyle: {
      label: 'Paste and Match Style',
      accelerator: 'Option+Shift+CommandOrControl+V',
    },
    quit: {
      get label()  {
        switch (process.platform) {
          case 'darwin': return `Quit ${appInfo.name}`
          case 'win32': return 'Exit'
          default: return 'Quit'
        }
      },
      click() {
        lazyApp().quit();
      },
      accelerator: process.platform === 'win32' ? '' : 'CommandOrControl+Q',
    },
    redo: {
      label: 'Redo',
      accelerator: process.platform === 'win32' ? 'Control+Y' : 'Shift+CommandOrControl+Z',
    },
    reload: {
      label: 'Reload',
      accelerator: 'CmdOrCtrl+R',
      click(item, window) {
        window.reload();
      }
    },
    selectall: {
      label: 'Select All',
      accelerator: 'CommandOrControl+A',
    },
    services: {
      label: 'Services'
    },
    recentdocuments: {
      label: 'Open Recent'
    },
    clearrecentdocuments: {
      label: 'Clear Menu'
    },
    startspeaking: {
      label: 'Start Speaking'
    },
    stopspeaking: {
      label: 'Stop Speaking'
    },
    toggledevtools: {
      accelerator: process.platform === 'darwin' ? 'Alt+Command+I' : 'Ctrl+Shift+I',
    },
    togglefullscreen: {
      label: 'Toggle Full Screen',
      accelerator: process.platform === 'darwin' ? 'Control+Command+F' : 'F11',
    },
    undo: {
      label: 'Undo',
      accelerator: 'CommandOrControl+Z',
    },
    unhide: {
      label: 'Show All'
    },
    zoom: {
      label: 'Zoom'
    },

    editmenu: {
      label: 'Edit',
      submenu: [
        {
          role: 'undo'
        },
        {
          role: 'redo'
        },
        {
          type: 'separator'
        },
        {
          role: 'cut'
        },
        {
          role: 'copy'
        },
        {
          role: 'paste'
        },
  
        process.platform === 'darwin' ? {
          role: 'pasteAndMatchStyle'
        } : null,
  
        {
          role: 'delete'
        },
  
        process.platform === 'win32' ? {
          type: 'separator'
        } : null,
  
        {
          role: 'selectAll'
        },

        process.platform === 'darwin' ? {
          type: 'separator'
        } : null,

        process.platform === 'darwin' ? {
          label: 'Speech',
          submenu: [
            {
              role: 'startSpeaking'
            },
            {
              role: 'stopSpeaking'
            }
          ]
        } : null
      ]
    },
  
    windowmenu: {
      label: 'Window',
      submenu: [
        {
          role: 'minimize'
        },
        {
          role: 'close'
        },

        process.platform === 'darwin' ? {
          role: 'zoom'
        } : null,
  
        process.platform === 'darwin' ? {
          type: 'separator'
        } : null,
  
        process.platform === 'darwin' ? {
          role: 'front'
        } : null
      ]
    }
}

export default roleDefaults;
