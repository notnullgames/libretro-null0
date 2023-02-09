// This is a simple tester, so I can check things as I go

#define PNTR_SUPPORT_DEFAULT_FONT
#define PNTR_PIXELFORMAT_ARGB
#define PNTR_IMPLEMENTATION

#include <SDL.h>
#include "null0.h"

SDL_Window* window;
SDL_Surface* screen;
SDL_Surface* surface;

bool shouldClose = false;

void mainloop() {
  SDL_Event event;
  while (SDL_PollEvent(&event) != 0) {
    switch (event.type) {
      case SDL_QUIT:
        shouldClose = true;
        break;
    }
  }

  if (shouldClose) {
    SDL_FreeSurface(surface);
    null0_wasm_unload();
    SDL_DestroyWindow(window);
    SDL_Quit();
  } else {
    null0_wasm_update();
    SDL_BlitSurface(surface, NULL, screen, NULL);
    SDL_UpdateWindowSurface(window);
  }
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s cart[.null0|.wasm|.zip]\n", argv[0]);
    return 1;
  }

  if (access(argv[1], F_OK) == -1) {
    fprintf(stderr, "cannot open %s\n", argv[1]);
    return 1;
  }

  PHYSFS_init(argv[1]);
  null0.env = m3_NewEnvironment();
  null0.runtime = m3_NewRuntime(null0.env, 1024 * 1024, NULL);

  Null0CartData cart;

  bool lookInPhys = false;

  DIR* dirptr = opendir(argv[1]);
  if (dirptr == NULL) {
    cart = null0_load_file(argv[1]);
    if (cart.size == 0) {
      fprintf(stderr, "cannot open %s\n", argv[1]);
      return 1;
    }
    Null0CartData sourceFile = null0_load_file(argv[1]);
    if (null0_fs_is_zip(sourceFile.bytes)) {
      // it's a zip, mount it
      PHYSFS_mountMemory(sourceFile.bytes, sourceFile.size, NULL, argv[1], NULL, 0);
      lookInPhys = true;
    } else if (null0_fs_is_wasm(sourceFile.bytes)) {
      // it's a wasm file
      cart = sourceFile;
    }
  } else {
    // it's a directory, mount it
    PHYSFS_mount(argv[1], NULL, 0);
    lookInPhys = true;
  }

  if (lookInPhys) {
    if (!null0_fs_exists("main.wasm")) {
      fprintf(stderr, "No main.wasm.\n");
      return 1;
    }

    PHYSFS_File* wasmFile = PHYSFS_openRead("main.wasm");
    PHYSFS_uint64 wasmLen = PHYSFS_fileLength(wasmFile);
    u8* wasmBuffer = malloc(wasmLen);
    PHYSFS_sint64 bytesRead = PHYSFS_readBytes(wasmFile, wasmBuffer, wasmLen);
    PHYSFS_close(wasmFile);
    cart.size = wasmLen;
    cart.bytes = (u8*)wasmBuffer;
  }

  if (!null0_fs_is_wasm(cart.bytes)) {
    fprintf(stderr, "Invalid wasm.\n");
    return 1;
  }

  bool success = null0_wasm_init(cart);

  if (!success) {
    fprintf(stderr, "Error loading wasm.\n");
    return 1;
  }

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  window = SDL_CreateWindow("null0", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, null0.images[0]->width, null0.images[0]->height, SDL_WINDOW_SHOWN);
  screen = SDL_GetWindowSurface(window);
  surface = SDL_CreateRGBSurfaceWithFormatFrom(null0.images[0]->data, null0.images[0]->width, null0.images[0]->height, 8, null0.images[0]->pitch, SDL_PIXELFORMAT_ARGB8888);

  while (!shouldClose) {
    mainloop();
    SDL_Delay(16);
  }

  return 0;
}