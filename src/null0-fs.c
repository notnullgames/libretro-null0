#include "null0-fs.h"

// given a filename, this will load the bytes for it
Null0CartData null0_load_file(char* filename) {
  Null0CartData out;

  FILE* f = fopen(filename, "rb");
  if (!f) {
    fprintf(stderr, "Cannot open file.\n");
    return out;
  }
  fseek(f, 0, SEEK_END);
  out.size = ftell(f);
  fseek(f, 0, SEEK_SET);

  out.bytes = (unsigned char*)malloc(out.size);
  if (!out.bytes) {
    fprintf(stderr, "cannot allocate memory for wasm binary\n");
    return out;
  }

  if (fread(out.bytes, 1, out.size, f) != out.size) {
    fprintf(stderr, "cannot read file\n");
    return out;
  }
  fclose(f);
  f = NULL;

  return out;
}

// Does a file exist in physfs?
bool null0_fs_exists(const char* filename) {
  PHYSFS_Stat stat;
  if (PHYSFS_stat(filename, &stat) == 0) {
    return false;
  }
  return stat.filetype == PHYSFS_FILETYPE_REGULAR;
}

// read a file from physfs
unsigned char* null0_fs_file_read(const char* filename, unsigned int* bytesRead) {
  if (!null0_fs_exists(filename)) {
    fprintf(stderr, "file: Tried to load non-existing file '%s'\n", filename);
    bytesRead = 0;
    return 0;
  }

  // Open up the file.
  void* handle = PHYSFS_openRead(filename);
  if (handle == 0) {
    fprintf(stderr, "file: Could not open file '%s'\n", filename);
    bytesRead = 0;
    return 0;
  }

  // Check to see how large the file is.
  int size = PHYSFS_fileLength(handle);
  if (size == -1) {
    bytesRead = 0;
    PHYSFS_close(handle);
    fprintf(stderr, "file: Cannot determine size of file '%s'\n", filename);
    return 0;
  }

  // Close safely when it's empty.
  if (size == 0) {
    PHYSFS_close(handle);
    bytesRead = 0;
    return 0;
  }

  // Read the file, return if it's empty.
  void* buffer = malloc(size);
  int read = PHYSFS_readBytes(handle, buffer, size);
  if (read < 0) {
    bytesRead = 0;
    free(buffer);
    PHYSFS_close(handle);
    fprintf(stderr, "file: Cannot read bytes '%s'\n", filename);
    return 0;
  }

  // Close the file handle, and return the bytes read and the buffer.
  PHYSFS_close(handle);
  *bytesRead = read;
  return buffer;
}

// use first 4 bytes of file to detect zip
bool null0_fs_is_zip(u8* bytes) {
  return bytes[0] == 80 && bytes[1] == 75 && bytes[2] == 3 && bytes[3] == 4;
}

// use first 4 bytes of file to detect wasm
bool null0_fs_is_wasm(u8* bytes) {
  return bytes[0] == 0 && bytes[1] == 97 && bytes[2] == 115 && bytes[3] == 109;
}

// IMPORT: Check if a file exists
static m3ApiRawFunction(null0_import_fs_exists) {
  m3ApiReturnType(bool);
  m3ApiGetArgMem(const char*, filename);
  m3ApiReturn(null0_fs_exists(filename));
  m3ApiSuccess();
}

// initialize wasm-exposed API
bool null0_fs_init_wasm() {
  // physfs is already mounted

  // mount the write-dir, so the cart can read from it
  PHYSFS_mount(PHYSFS_getWriteDir(), NULL, 0);

  // IMPORTS (to wasm)
  m3_LinkRawFunction(null0.module, "env", "null0_exists", "i(*)", &null0_import_fs_exists);

  return true;
}