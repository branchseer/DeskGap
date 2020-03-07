## Changed
- Downgrade Node.js to v12.4.0, because newer versions have issues running in the macOS sandbox

## Removed
- Node-webview communication methods: messageNode, messageUI, asyncNode

## Added
- A rpc-style node-webview communication methods (to be documented, see node/test/webview.js for examples)
