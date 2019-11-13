import { MenuItemConstructorOptions } from '../../menu'
import shell from '../../shell';

/** @internal */
const template: Array<Partial<MenuItemConstructorOptions> | null> = [
    {
      role: 'editMenu'
    },
    {
      label: 'View',
      submenu: [
        {
          role: 'reload'
        },
        {
          role: 'toggleDevTools'
        },
        process.platform === 'darwin' ? {
          type: 'separator'
        }: null,
        process.platform === 'darwin' ? {
          role: 'toggleFullScreen'
        }: null
      ]
    },
    {
      role: 'windowMenu'
    },
    {
      role: 'help',
      submenu: [
        {
          label: 'Website',
          click () {
            shell.openExternal('https://deskgap.com/')
          }
        },
        {
          label: 'GitHub Repo',
          click () {
            shell.openExternal('https://github.com/patr0nus/DeskGap')
          }
        }
      ]
    }
  ];

  if (process.platform === 'darwin') {
    template.unshift({
      label: 'DeskGap',
      submenu: [
        {
          role: 'about'
        },
        {
          type: 'separator'
        },
        {
          role: 'services',
          submenu: []
        },
        {
          type: 'separator'
        },
        {
          role: 'hide'
        },
        {
          role: 'hideothers'
        },
        {
          role: 'unhide'
        },
        {
          type: 'separator'
        },
        {
          role: 'quit'
        }
      ]
    })
   
  } else {
    template.unshift({
      label: 'File',
      submenu: [{
        role: 'quit'
      }]
    })
  }

/** @internal */
export default template;
