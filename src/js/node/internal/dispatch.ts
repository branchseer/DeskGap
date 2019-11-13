const { delayUISync, commitUISync } = require('../bindings');

let inBulkUISync = false;
export const bulkUISync = (block: () => void) => {
    // return block();
    if (inBulkUISync) {
        return block();
    }

    inBulkUISync = true;
    delayUISync();
    try {
        return block();
    }
    finally {
        inBulkUISync = false;
        commitUISync();
    }
}
