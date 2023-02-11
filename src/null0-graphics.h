#ifndef NULL0_GRAPHICS_H
#define NULL0_GRAPHICS_H

#define PNTR_SUPPORT_DEFAULT_FONT
#define PNTR_PIXELFORMAT_ARGB
#define PNTR_IMPLEMENTATION

#include "m3_env.h"
#include "pntr.h"
#include "wasm3.h"

pntr_image* null0_images[NULL0_RESOURCE_MAX];
u8 currentImage = 0;

pntr_font* null0_fonts[NULL0_RESOURCE_MAX];
u8 currentFont = 0;

struct Null0Vector2 {
  u32 x;
  u32 y;
};
typedef struct Null0Vector2 Null0Vector2;

static m3ApiRawFunction(null0_import_draw_circle) {
  m3ApiGetArg(u8, dst);
  m3ApiGetArg(int, centerX);
  m3ApiGetArg(int, centerY);
  m3ApiGetArg(int, radius);
  m3ApiGetArgMem(pntr_color*, color);
  pntr_draw_circle(null0_images[dst], centerX, centerY, radius, *color);
  const char* err = pntr_get_error();
  if (err != NULL) {
    fprintf(stderr, "null0: %s\n", err);
  }
  m3ApiSuccess();
}

static m3ApiRawFunction(null0_import_draw_image) {
  m3ApiGetArg(u8, dst);
  m3ApiGetArg(u8, src);
  m3ApiGetArg(int, posX);
  m3ApiGetArg(int, posY);
  pntr_draw_image(null0_images[dst], null0_images[src], posX, posY);
  const char* err = pntr_get_error();
  if (err != NULL) {
    fprintf(stderr, "null0: %s\n", err);
  }
  m3ApiSuccess();
}

static m3ApiRawFunction(null0_import_draw_line) {
  m3ApiGetArg(u8, dst);
  m3ApiGetArg(int, startPosX);
  m3ApiGetArg(int, startPosY);
  m3ApiGetArg(int, endPosX);
  m3ApiGetArg(int, endPosY);
  m3ApiGetArgMem(pntr_color*, color);
  pntr_draw_line(null0_images[dst], startPosX, startPosY, endPosX, endPosY, *color);
  const char* err = pntr_get_error();
  if (err != NULL) {
    fprintf(stderr, "null0: %s\n", err);
  }
  m3ApiSuccess();
}

static m3ApiRawFunction(null0_import_draw_pixel) {
  m3ApiGetArg(u8, dst);
  m3ApiGetArg(int, x);
  m3ApiGetArg(int, y);
  m3ApiGetArgMem(pntr_color*, color);
  pntr_draw_pixel(null0_images[dst], x, y, *color);
  const char* err = pntr_get_error();
  if (err != NULL) {
    fprintf(stderr, "null0: %s\n", err);
  }
  m3ApiSuccess();
}

static m3ApiRawFunction(null0_import_draw_rectangle) {
  m3ApiGetArg(u8, dst);
  m3ApiGetArg(int, posX);
  m3ApiGetArg(int, posY);
  m3ApiGetArg(int, width);
  m3ApiGetArg(int, height);
  m3ApiGetArgMem(pntr_color*, color);
  pntr_draw_rectangle(null0_images[dst], posX, posY, width, height, *color);
  const char* err = pntr_get_error();
  if (err != NULL) {
    fprintf(stderr, "null0: %s\n", err);
  }
  m3ApiSuccess();
}

static m3ApiRawFunction(null0_import_draw_text) {
  m3ApiGetArg(u8, dst);
  m3ApiGetArg(u8, font);
  m3ApiGetArgMem(const char*, text);
  m3ApiGetArg(int, posX);
  m3ApiGetArg(int, posY);
  pntr_draw_text(null0_images[dst], null0_fonts[font], text, posX, posY);
  const char* err = pntr_get_error();
  if (err != NULL) {
    fprintf(stderr, "null0: %s\n", err);
  }
  m3ApiSuccess();
}

static m3ApiRawFunction(null0_import_image_clear) {
  m3ApiGetArg(u8, dst);
  m3ApiGetArgMem(pntr_color*, color);
  pntr_clear_background(null0_images[dst], *color);
  const char* err = pntr_get_error();
  if (err != NULL) {
    fprintf(stderr, "null0: %s\n", err);
  }
  m3ApiSuccess();
}

static m3ApiRawFunction(null0_import_image_color_invert) {
  m3ApiGetArg(u8, dst);
  pntr_image_color_invert(null0_images[dst]);
  const char* err = pntr_get_error();
  if (err != NULL) {
    fprintf(stderr, "null0: %s\n", err);
  }
  m3ApiSuccess();
}

static m3ApiRawFunction(null0_import_image_color_replace) {
  m3ApiGetArg(u8, dst);
  m3ApiGetArgMem(pntr_color*, color);
  m3ApiGetArgMem(pntr_color*, replace);
  pntr_image_color_replace(null0_images[dst], *color, *replace);
  const char* err = pntr_get_error();
  if (err != NULL) {
    fprintf(stderr, "null0: %s\n", err);
  }
  m3ApiSuccess();
}

static m3ApiRawFunction(null0_import_image_color_tint) {
  m3ApiGetArg(u8, dst);
  m3ApiGetArgMem(pntr_color*, color);
  pntr_image_color_tint(null0_images[dst], *color);
  const char* err = pntr_get_error();
  if (err != NULL) {
    fprintf(stderr, "null0: %s\n", err);
  }
  m3ApiSuccess();
}

static m3ApiRawFunction(null0_import_image_copy) {
  m3ApiReturnType(u8);
  m3ApiGetArg(u8, src);
  null0_images[currentImage++] = pntr_image_copy(null0_images[src]);
  const char* err = pntr_get_error();
  if (err != NULL) {
    fprintf(stderr, "null0: %s\n", err);
  }
  m3ApiReturn(currentImage);
  m3ApiSuccess();
}

static m3ApiRawFunction(null0_import_image_from_image) {
  m3ApiReturnType(u8);
  m3ApiGetArg(u8, src);
  m3ApiGetArg(int, x);
  m3ApiGetArg(int, y);
  m3ApiGetArg(int, width);
  m3ApiGetArg(int, height);

  null0_images[currentImage++] = pntr_image_from_image(null0_images[src], x, y, width, height);
  const char* err = pntr_get_error();
  if (err != NULL) {
    fprintf(stderr, "null0: %s\n", err);
  }
  m3ApiReturn(currentImage);
  m3ApiSuccess();
}

static m3ApiRawFunction(null0_import_image_get_color) {
  m3ApiReturnType(pntr_color*);
  m3ApiGetArg(u8, src);
  m3ApiGetArg(int, x);
  m3ApiGetArg(int, y);
  pntr_color c = pntr_image_get_color(null0_images[src], x, y);
  const char* err = pntr_get_error();
  if (err != NULL) {
    fprintf(stderr, "null0: %s\n", err);
  }
  m3ApiReturn(&c);
  m3ApiSuccess();
}

static m3ApiRawFunction(null0_import_image_load) {
  m3ApiReturnType(u8);
  m3ApiGetArgMem(const char*, filename);
  unsigned int* bytesRead;
  if (bytesRead) {
    unsigned char* fileData = null0_fs_file_read(filename, bytesRead);
    null0_images[currentImage++] = pntr_load_image_from_memory(fileData, *bytesRead);
    const char* err = pntr_get_error();
    if (err != NULL) {
      fprintf(stderr, "null0: %s\n", err);
    }
    m3ApiReturn(currentImage);
  }
  m3ApiSuccess();
}

static m3ApiRawFunction(null0_import_image_new) {
  m3ApiReturnType(u8);
  m3ApiGetArg(int, width);
  m3ApiGetArg(int, height);
  null0_images[currentImage++] = pntr_new_image(width, height);
  const char* err = pntr_get_error();
  if (err != NULL) {
    fprintf(stderr, "null0: %s\n", err);
  }
  m3ApiReturn(currentImage);
  m3ApiSuccess();
}

static m3ApiRawFunction(null0_import_image_resize) {
  m3ApiReturnType(u8);
  m3ApiGetArg(u8, src);
  m3ApiGetArg(int, newWidth);
  m3ApiGetArg(int, newHeight);
  null0_images[currentImage++] = pntr_image_resize(null0_images[src], newWidth, newHeight, PNTR_FILTER_NEARESTNEIGHBOR);
  const char* err = pntr_get_error();
  if (err != NULL) {
    fprintf(stderr, "null0: %s\n", err);
  }
  m3ApiReturn(currentImage);
  m3ApiSuccess();
}

static m3ApiRawFunction(null0_import_load_font_bm) {
  m3ApiReturnType(u8);
  m3ApiGetArgMem(const char*, filename);
  m3ApiGetArgMem(const char*, characters);
  unsigned int* bytesRead;
  unsigned char* fileData = null0_fs_file_read(filename, bytesRead);
  if (bytesRead) {
    null0_fonts[currentFont++] = pntr_load_bmfont_from_memory(fileData, *bytesRead, characters);
    m3ApiReturn(currentFont);
  }
  m3ApiSuccess();
}

static m3ApiRawFunction(null0_import_load_font_ttf) {
  m3ApiReturnType(u8);
  m3ApiGetArgMem(const char*, filename);
  m3ApiGetArgMem(pntr_color*, color);
  m3ApiGetArg(int, fontSize);
  unsigned int* bytesRead;
  unsigned char* fileData = null0_fs_file_read(filename, bytesRead);
  if (bytesRead) {
    pntr_load_ttffont_from_memory(fileData, *bytesRead, fontSize, *color);
    m3ApiReturn(currentFont);
  }
  m3ApiReturn(currentFont);
  m3ApiSuccess();
}

static m3ApiRawFunction(null0_import_load_font_tty) {
  m3ApiReturnType(u8);
  m3ApiGetArgMem(const char*, filename);
  m3ApiGetArg(int, glyphWidth);
  m3ApiGetArg(int, glyphHeight);
  m3ApiGetArgMem(const char*, characters);
  unsigned int* bytesRead;
  unsigned char* fileData = null0_fs_file_read(filename, bytesRead);
  if (bytesRead) {
    pntr_load_ttyfont_from_memory(fileData, *bytesRead, glyphWidth, glyphHeight, characters);
    m3ApiReturn(currentFont);
  }
  m3ApiSuccess();
}

static m3ApiRawFunction(null0_import_text_measure) {
  m3ApiReturnType(Null0Vector2*);
  m3ApiGetArg(u8, font);
  m3ApiGetArgMem(const char*, text);
  pntr_vector m = pntr_measure_text_ex(null0_fonts[font], text);
  Null0Vector2 v = {
      .x = m.x,
      .y = m.y};
  m3ApiReturn(&v);
  m3ApiSuccess();
}

static m3ApiRawFunction(null0_import_image_measure) {
  m3ApiReturnType(Null0Vector2*);
  m3ApiGetArg(u8, dst);
  Null0Vector2 v = {
      .x = null0_images[dst]->width,
      .y = null0_images[dst]->height};
  m3ApiReturn(&v);
  m3ApiSuccess();
}

// get the screen in host
pntr_image* null0_graphics_get() {
  return null0_images[0];
}

bool null0_imports_graphics(M3Module* module) {
  // image 0 is screen
  null0_images[0] = pntr_gen_image_color(NULL0_WIDTH, NULL0_HEIGHT, PNTR_BLACK);

  // font 0 is default font
  null0_fonts[0] = pntr_load_default_font();

  m3_LinkRawFunction(module, "null0", "draw_circle", "v(iiii*)", &null0_import_draw_circle);
  m3_LinkRawFunction(module, "null0", "draw_image", "v(iiii)", &null0_import_draw_image);
  m3_LinkRawFunction(module, "null0", "draw_line", "v(iiiii*)", &null0_import_draw_line);
  m3_LinkRawFunction(module, "null0", "draw_pixel", "v(iii*)", &null0_import_draw_pixel);
  m3_LinkRawFunction(module, "null0", "draw_rectangle", "v(iiiii*)", &null0_import_draw_rectangle);
  m3_LinkRawFunction(module, "null0", "draw_text", "v(ii*ii)", &null0_import_draw_text);
  m3_LinkRawFunction(module, "null0", "image_clear", "v(i*)", &null0_import_image_clear);
  m3_LinkRawFunction(module, "null0", "image_color_invert", "v(i)", &null0_import_image_color_invert);
  m3_LinkRawFunction(module, "null0", "image_color_replace", "v(i**)", &null0_import_image_color_replace);
  m3_LinkRawFunction(module, "null0", "image_image_tint", "v(i*)", &null0_import_image_color_tint);
  m3_LinkRawFunction(module, "null0", "image_copy", "i(i)", &null0_import_image_copy);
  m3_LinkRawFunction(module, "null0", "image_from_image", "i(iiiii)", &null0_import_image_from_image);
  m3_LinkRawFunction(module, "null0", "image_color_get", "*(iii)", &null0_import_image_get_color);
  m3_LinkRawFunction(module, "null0", "image_load", "i(*)", &null0_import_image_load);
  m3_LinkRawFunction(module, "null0", "image_new", "i(ii)", &null0_import_image_new);
  m3_LinkRawFunction(module, "null0", "image_resize", "i(iii)", &null0_import_image_resize);
  m3_LinkRawFunction(module, "null0", "font_load_bm", "i(**)", &null0_import_load_font_bm);
  m3_LinkRawFunction(module, "null0", "font_load_ttf", "i(**i)", &null0_import_load_font_ttf);
  m3_LinkRawFunction(module, "null0", "font_load_tty", "i(*ii*)", &null0_import_load_font_tty);
  m3_LinkRawFunction(module, "null0", "font_measure", "*(i*)", &null0_import_text_measure);
  m3_LinkRawFunction(module, "null0", "image_measure", "*(i)", &null0_import_image_measure);

  return true;
}

// call this when a cart unloads
void null0_graphics_unload() {
  for (int i = 0; i < NULL0_RESOURCE_MAX; i++) {
    if (null0_fonts[i]) {
      pntr_unload_font(null0_fonts[i]);
    }
    if (null0_images[i]) {
      pntr_unload_image(null0_images[i]);
    }
  }
}

#endif  // NULL0_GRAPHICS_H