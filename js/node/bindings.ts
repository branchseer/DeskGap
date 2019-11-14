const bindings = (global as any)._nod_native_mod;
export = bindings;
delete (global as any)._nod_native_mod;
