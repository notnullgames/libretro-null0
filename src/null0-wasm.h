#ifndef NULL0_WASM_H
#define NULL0_WASM_H

#include "null0.h"

// load wasm runtime (calls cart's load)
bool null0_wasm_init(Null0CartData cart);

// call this to update runtime (calls cart's update)
void null0_wasm_update();

// call this on unload (calls cart's unload)
void null0_wasm_unload();

// allocate WASM memory & copy value in
u32 wmcopy(void* pointer, size_t size);

// free WASM memory
void wfree(u32 pointer);

#endif  // NULL0_WASM_H