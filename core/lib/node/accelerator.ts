
const acceleratorTokenAliases = {
    'command': 'cmd',
    'control': 'ctrl',
    'return': 'enter',
    'escape': 'esc',
    'cmdorctrl': (process.platform === 'darwin') ? 'cmd': 'ctrl',
    'alt': (process.platform === 'darwin') ? 'option': null,
    'plus': '+'
} as Record<string, string>;

acceleratorTokenAliases['commandorcontrol'] = acceleratorTokenAliases['cmdorctrl'];

export const parseAcceleratorToTokens = (expr: string): string[] => expr.split('+').map(t => {
    let token = t.trim().toLowerCase();
    return acceleratorTokenAliases[token] || token;
});
