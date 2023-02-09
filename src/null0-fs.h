#ifndef NULL0_FS_H
#define NULL0_FS_H

#include "null0.h"

// given a filename, this will load the bytes for it
Null0CartData null0_load_file(char* filename);

// Does a file exist in physfs?
bool null0_fs_exists(const char* fileName);

// use first 4 bytes of file to detect zip
bool null0_fs_is_zip(u8* bytes);

// use first 4 bytes of file to detect wasm
bool null0_fs_is_wasm(u8* bytes);

// initialize wasm-exposed API
bool null0_fs_init_wasm();

// read a file from physfs
unsigned char* null0_fs_file_read(const char* filename, unsigned int* bytesRead);

#endif  // NULL0_FS_H