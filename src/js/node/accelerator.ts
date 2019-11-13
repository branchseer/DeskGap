
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

export const parseAcceleratorToTokens = (expr: string): string[] => {
	const tokens: string[] = [];
	for (let token of expr.split('+')) {
		token = token.trim().toLowerCase();
		if (token.length === 0) continue;
		tokens.push(acceleratorTokenAliases[token] || token);
	}
	return tokens;
}
