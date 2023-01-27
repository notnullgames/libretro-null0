// null0 host C header

// TODO: look into wasm3 wasi header for better string processing

#include <dirent.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include "m3_env.h"
#include "physfs.h"
#include "wasm3.h"

// https://github.com/Mashpoe/c-vector
#define VEC_IMPLEMENTATION
#include "vec.h"

// #define SUPPORT_MODULE_RTEXTURES
#define SUPPORT_FILEFORMAT_BMP
#define SUPPORT_FILEFORMAT_PNG
#define SUPPORT_FILEFORMAT_TGA
#define SUPPORT_FILEFORMAT_JPG
#define SUPPORT_FILEFORMAT_GIF
// #define SUPPORT_FILEFORMAT_QOI
// #define SUPPORT_FILEFORMAT_PSD
// #define SUPPORT_FILEFORMAT_HDR
// #define SUPPORT_FILEFORMAT_PIC
// #define SUPPORT_FILEFORMAT_PNM
// #define SUPPORT_FILEFORMAT_DDS
// #define SUPPORT_FILEFORMAT_PKM
// #define SUPPORT_FILEFORMAT_KTX
// #define SUPPORT_FILEFORMAT_PVR
// #define SUPPORT_FILEFORMAT_ASTC
// #define SUPPORT_IMAGE_EXPORT
#define SUPPORT_IMAGE_MANIPULATION
// #define SUPPORT_IMAGE_GENERATION

// https://github.com/RobLoach/rimage
#define RIMAGE_IMPLEMENTATION
#include "rimage.h"

static M3Environment* env;
static M3Runtime* runtime;
static M3Module* module;

static M3Function* new_func;
static M3Function* cart_load;
static M3Function* cart_update;
Image* screenBuffer;

// TODO: I'm sure this is not the right way to hold on to these.
Image resourceImages[1024];
unsigned int imageCounter = 0;

struct timeval now, start;

// Get pointer to extension for a filename string (includes the dot: .png)
const char* GetFileExtension(const char* fileName) {
  const char* dot = strrchr(fileName, '.');

  if (!dot || dot == fileName)
    return NULL;

  return dot;
}

// report physfs error
void TracePhysFSError(const char* detail) {
  int errorCode = PHYSFS_getLastErrorCode();
  if (errorCode == PHYSFS_ERR_OK) {
    fprintf(stderr, "null0 fs: %s", detail);
  } else {
    const char* errorMessage = PHYSFS_getErrorByCode(errorCode);
    fprintf(stderr, "null0 fs: %s (%s)", errorMessage, detail);
  }
}

// Determine if a file exists in the search path.
bool FileExistsInPhysFS(const char* fileName) {
  PHYSFS_Stat stat;
  if (PHYSFS_stat(fileName, &stat) == 0) {
    return false;
  }
  return stat.filetype == PHYSFS_FILETYPE_REGULAR;
}

// Get filsize in bytes from PhysFS.
int FileSizeFromPhysFS(const char* fileName) {
  if (FileExistsInPhysFS(fileName)) {
    void* handle = PHYSFS_openRead(fileName);
    int size = PHYSFS_fileLength(handle);
    return size;
  } else {
    return -1;
  }
}

// Loads the given file as a byte array from PhysFS (read).
unsigned char* LoadFileDataFromPhysFS(const char* fileName, unsigned int* bytesRead) {
  if (!FileExistsInPhysFS(fileName)) {
    fprintf(stderr, "null0: Tried to load unexisting file '%s'", fileName);
    *bytesRead = 0;
    return 0;
  }

  // Open up the file.
  void* handle = PHYSFS_openRead(fileName);
  if (handle == 0) {
    TracePhysFSError(fileName);
    *bytesRead = 0;
    return 0;
  }

  // Check to see how large the file is.
  int size = PHYSFS_fileLength(handle);
  if (size == -1) {
    *bytesRead = 0;
    PHYSFS_close(handle);
    fprintf(stderr, "null0: Cannot determine size of file '%s'", fileName);
    return 0;
  }

  // Close safely when it's empty.
  if (size == 0) {
    PHYSFS_close(handle);
    *bytesRead = 0;
    return 0;
  }

  // Read the file, return if it's empty.
  void* buffer = malloc(size);
  int read = PHYSFS_readBytes(handle, buffer, size);
  if (read < 0) {
    *bytesRead = 0;
    free(buffer);
    PHYSFS_close(handle);
    TracePhysFSError(fileName);
    return 0;
  }

  // Close the file handle, and return the bytes read and the buffer.
  PHYSFS_close(handle);
  *bytesRead = read;
  return buffer;
}

// Load an image from PhysFS.
Image LoadImageFromPhysFS(const char* fileName) {
  unsigned int bytesRead;
  unsigned char* fileData = LoadFileDataFromPhysFS(fileName, &bytesRead);
  if (bytesRead == 0) {
    struct Image output;
    output.data = 0;
    output.width = 0;
    output.height = 0;
    return output;
  }

  // Load from the memory.
  const char* extension = GetFileExtension(fileName);
  printf("image: %s - %s - %d\n", fileName, extension, bytesRead);
  Image image = LoadImageFromMemory(extension, fileData, bytesRead);
  free(fileData);
  return image;
}

// this checks the general state of the runtime, to make sure there are no errors lingering
static void null0_check_wasm3_is_ok() {
  M3ErrorInfo error;
  m3_GetErrorInfo(runtime, &error);
  if (error.result) {
    fprintf(stderr, "%s - %s\n", error.result, error.message);
  }
}

// all wasm3 functions return same sort of error-pattern, so this wraps that
static void null0_check_wasm3(M3Result result) {
  if (result) {
    null0_check_wasm3_is_ok();
  }
}

// copy a buffer into wasm RAM and return wasm-pointer (for strings)
uint32_t lowerBuffer(unsigned char* buffer, size_t len, M3Memory* _mem) {
  uint32_t wPointer;
  null0_check_wasm3(m3_CallV(new_func, len, 1));
  m3_GetResultsV(new_func, &wPointer);
  char* wBuffer = m3ApiOffsetToPtr(wPointer);
  memcpy(wBuffer, buffer, len);
  return wPointer;
}

// IMPORTS

// exposed to wasm to seed random-generator
static m3ApiRawFunction(null0_seed) {
  m3ApiReturnType(double);
  gettimeofday(&now, NULL);
  m3ApiReturn(now.tv_usec);
}

//  Fatal error - call this from your code on a fatal runtime error, similar to assemblyscript's abort(), but it's utf8
static m3ApiRawFunction(null0_fatal) {
  m3ApiGetArgMem(const char*, message);
  m3ApiGetArgMem(const char*, fileName);
  m3ApiGetArg(uint16_t, lineNumber);
  m3ApiGetArg(uint16_t, columnNumber);
  fprintf(stderr, "%s at %s:%d:%d\n", message, fileName, lineNumber, columnNumber);
  m3ApiSuccess();
}

// Log a string
static m3ApiRawFunction(null0_log) {
  m3ApiGetArgMem(const char*, message);
  printf("%s\n", message);
  m3ApiSuccess();
}

// Read a text-file from virtual filesystem
static m3ApiRawFunction(null0_ReadText) {
  m3ApiReturnType(uint32_t);
  m3ApiGetArgMem(const char*, fileName);

  if (!FileExistsInPhysFS(fileName)) {
    fprintf(stderr, "null0: %s does not exist.\n", fileName);
    m3ApiReturn(0);
    return 0;
  }

  // example of what you want to return
  unsigned int* bytesRead;
  unsigned char* buffer = LoadFileDataFromPhysFS(fileName, bytesRead);

  m3ApiReturn(lowerBuffer(buffer, strlen((const char*)buffer) + 1, _mem));
}

// Clear background of screen-buffer image
static m3ApiRawFunction(null0_ClearBackground) {
  m3ApiGetArgMem(Color*, color);
  ImageClearBackground(screenBuffer, *color);
  m3ApiSuccess();
}

// Draw text (using default font) within an image (destination)
static m3ApiRawFunction(null0_DrawText) {
  m3ApiGetArgMem(Image*, dst);
  m3ApiGetArgMem(Font*, font);
  m3ApiGetArgMem(const char*, text);
  m3ApiGetArg(uint32_t, posX);
  m3ApiGetArg(uint32_t, posY);
  m3ApiGetArg(uint32_t, fontSize);
  m3ApiGetArgMem(Color*, color);
  ImageDrawText(screenBuffer, *font, text, posX, posY, fontSize, *color);
  m3ApiSuccess();
}

// Draw a circle on screen-buffer image
static m3ApiRawFunction(null0_DrawCircle) {
  m3ApiGetArg(int32_t, centerX);
  m3ApiGetArg(int32_t, centerY);
  m3ApiGetArg(int32_t, radius);
  m3ApiGetArgMem(Color*, color);
  ImageDrawCircle(screenBuffer, centerX, centerY, radius, *color);
  m3ApiSuccess();
}

// Draw a line on screen-buffer image
static m3ApiRawFunction(null0_DrawLine) {
  m3ApiGetArg(int32_t, startPosX);
  m3ApiGetArg(int32_t, startPosY);
  m3ApiGetArg(int32_t, endPosX);
  m3ApiGetArg(int32_t, endPosY);
  m3ApiGetArgMem(Color*, color);
  ImageDrawLine(screenBuffer, startPosX, startPosY, endPosX, endPosY, *color);
  m3ApiSuccess();
}

// Draw a rectangle on screen-buffer image
static m3ApiRawFunction(null0_DrawRectangle) {
  m3ApiGetArg(int32_t, posX);
  m3ApiGetArg(int32_t, posY);
  m3ApiGetArg(int32_t, width);
  m3ApiGetArg(int32_t, height);
  m3ApiGetArgMem(Color*, color);
  ImageDrawRectangle(screenBuffer, posX, posY, width, height, *color);
  m3ApiSuccess();
}

// Load image from file into CPU memory (RAM)
static m3ApiRawFunction(null0_LoadImage) {
  m3ApiReturnType(uint32_t);
  m3ApiGetArgMem(const char*, fileName);
  resourceImages[imageCounter++] = LoadImageFromPhysFS(fileName);
  m3ApiReturn(imageCounter);
}

// Draw a source image within a destination image (tint applied to source)
static m3ApiRawFunction(null0_Draw) {
  m3ApiGetArg(uint32_t, src);
  m3ApiGetArgMem(Rectangle*, srcRec);
  m3ApiGetArgMem(Rectangle*, dstRec);
  m3ApiGetArgMem(Color*, tint);
  ImageDraw(screenBuffer, resourceImages[src], *srcRec, *dstRec, *tint);
  m3ApiSuccess();
}

/*

// Load image sequence from file (frames appended to image.data)
static m3ApiRawFunction (null0_LoadImageAnim) {
  m3ApiReturnType (Image); // Image
  m3ApiGetArgMem(const char*, fileName);
  m3ApiGetArgMem(int*, frames);
  m3ApiReturn(LoadImageAnim(fileName, frames));

}

// Unload image from CPU memory (RAM)
static m3ApiRawFunction (null0_UnloadImage) {

  m3ApiGetArg(Image, image);

  UnloadImage(image);
  m3ApiSuccess();
}

// Export image data to file, returns true on success
static m3ApiRawFunction (null0_ExportImage) {
  m3ApiReturnType (bool); // bool
  m3ApiGetArg(Image, image);
  m3ApiGetArgMem(const char*, fileName);
  m3ApiReturn(ExportImage(image, fileName));

}

// Generate image: plain color
static m3ApiRawFunction (null0_GenImageColor) {
  m3ApiReturnType (Image); // Image
  m3ApiGetArg(uint32_t, width);
  m3ApiGetArg(uint32_t, height);
  m3ApiGetArg(Color, color);
  m3ApiReturn(GenImageColor(width, height, color));

}

// Generate image: vertical gradient
static m3ApiRawFunction (null0_GenImageGradientV) {
  m3ApiReturnType (Image); // Image
  m3ApiGetArg(uint32_t, width);
  m3ApiGetArg(uint32_t, height);
  m3ApiGetArg(Color, top);
  m3ApiGetArg(Color, bottom);
  m3ApiReturn(GenImageGradientV(width, height, top, bottom));

}

// Generate image: horizontal gradient
static m3ApiRawFunction (null0_GenImageGradientH) {
  m3ApiReturnType (Image); // Image
  m3ApiGetArg(uint32_t, width);
  m3ApiGetArg(uint32_t, height);
  m3ApiGetArg(Color, left);
  m3ApiGetArg(Color, right);
  m3ApiReturn(GenImageGradientH(width, height, left, right));

}

// Generate image: radial gradient
static m3ApiRawFunction (null0_GenImageGradientRadial) {
  m3ApiReturnType (Image); // Image
  m3ApiGetArg(uint32_t, width);
  m3ApiGetArg(uint32_t, height);
  m3ApiGetArg(float, density);
  m3ApiGetArg(Color, inner);
  m3ApiGetArg(Color, outer);
  m3ApiReturn(GenImageGradientRadial(width, height, density, inner, outer));

}

// Generate image: checked
static m3ApiRawFunction (null0_GenImageChecked) {
  m3ApiReturnType (Image); // Image
  m3ApiGetArg(uint32_t, width);
  m3ApiGetArg(uint32_t, height);
  m3ApiGetArg(uint32_t, checksX);
  m3ApiGetArg(uint32_t, checksY);
  m3ApiGetArg(Color, col1);
  m3ApiGetArg(Color, col2);
  m3ApiReturn(GenImageChecked(width, height, checksX, checksY, col1, col2));

}

// Generate image: white noise
static m3ApiRawFunction (null0_GenImageWhiteNoise) {
  m3ApiReturnType (Image); // Image
  m3ApiGetArg(uint32_t, width);
  m3ApiGetArg(uint32_t, height);
  m3ApiGetArg(float, factor);
  m3ApiReturn(GenImageWhiteNoise(width, height, factor));

}

// Generate image: perlin noise
static m3ApiRawFunction (null0_GenImagePerlinNoise) {
  m3ApiReturnType (Image); // Image
  m3ApiGetArg(uint32_t, width);
  m3ApiGetArg(uint32_t, height);
  m3ApiGetArg(uint32_t, offsetX);
  m3ApiGetArg(uint32_t, offsetY);
  m3ApiGetArg(float, scale);
  m3ApiReturn(GenImagePerlinNoise(width, height, offsetX, offsetY, scale));

}

// Generate image: cellular algorithm, bigger tileSize means bigger cells
static m3ApiRawFunction (null0_GenImageCellular) {
  m3ApiReturnType (Image); // Image
  m3ApiGetArg(uint32_t, width);
  m3ApiGetArg(uint32_t, height);
  m3ApiGetArg(uint32_t, tileSize);
  m3ApiReturn(GenImageCellular(width, height, tileSize));

}

// Generate image: grayscale image from text data
static m3ApiRawFunction (null0_GenImageText) {
  m3ApiReturnType (Image); // Image
  m3ApiGetArg(uint32_t, width);
  m3ApiGetArg(uint32_t, height);
  m3ApiGetArgMem(const char*, text);
  m3ApiReturn(GenImageText(width, height, text));

}

// Create an image duplicate (useful for transformations)
static m3ApiRawFunction (null0_ImageCopy) {
  m3ApiReturnType (Image); // Image
  m3ApiGetArg(Image, image);
  m3ApiReturn(ImageCopy(image));

}

// Create an image from another image piece
static m3ApiRawFunction (null0_ImageFromImage) {
  m3ApiReturnType (Image); // Image
  m3ApiGetArg(Image, image);
  m3ApiGetArg(Rectangle, rec);
  m3ApiReturn(ImageFromImage(image, rec));

}

// Create an image from text (default font)
static m3ApiRawFunction (null0_ImageText) {
  m3ApiReturnType (Image); // Image
  m3ApiGetArgMem(const char*, text);
  m3ApiGetArg(uint32_t, fontSize);
  m3ApiGetArg(Color, color);
  m3ApiReturn(ImageText(text, fontSize, color));

}

// Create an image from text (custom sprite font)
static m3ApiRawFunction (null0_ImageTextEx) {
  m3ApiReturnType (Image); // Image
  m3ApiGetArg(Font, font);
  m3ApiGetArgMem(const char*, text);
  m3ApiGetArg(float, fontSize);
  m3ApiGetArg(float, spacing);
  m3ApiGetArg(Color, tint);
  m3ApiReturn(ImageTextEx(font, text, fontSize, spacing, tint));

}

// Convert image data to desired format
static m3ApiRawFunction (null0_ImageFormat) {

  m3ApiGetArgMem(Image*, image);
  m3ApiGetArg(uint32_t, newFormat);

  ImageFormat(image, newFormat);
  m3ApiSuccess();
}

// Convert image to POT (power-of-two)
static m3ApiRawFunction (null0_ImageToPOT) {

  m3ApiGetArgMem(Image*, image);
  m3ApiGetArg(Color, fill);

  ImageToPOT(image, fill);
  m3ApiSuccess();
}

// Crop an image to a defined rectangle
static m3ApiRawFunction (null0_ImageCrop) {

  m3ApiGetArgMem(Image*, image);
  m3ApiGetArg(Rectangle, crop);

  ImageCrop(image, crop);
  m3ApiSuccess();
}

// Crop image depending on alpha value
static m3ApiRawFunction (null0_ImageAlphaCrop) {

  m3ApiGetArgMem(Image*, image);
  m3ApiGetArg(float, threshold);

  ImageAlphaCrop(image, threshold);
  m3ApiSuccess();
}

// Clear alpha channel to desired color
static m3ApiRawFunction (null0_ImageAlphaClear) {

  m3ApiGetArgMem(Image*, image);
  m3ApiGetArg(Color, color);
  m3ApiGetArg(float, threshold);

  ImageAlphaClear(image, color, threshold);
  m3ApiSuccess();
}

// Apply alpha mask to image
static m3ApiRawFunction (null0_ImageAlphaMask) {

  m3ApiGetArgMem(Image*, image);
  m3ApiGetArg(Image, alphaMask);

  ImageAlphaMask(image, alphaMask);
  m3ApiSuccess();
}

// Premultiply alpha channel
static m3ApiRawFunction (null0_ImageAlphaPremultiply) {

  m3ApiGetArgMem(Image*, image);

  ImageAlphaPremultiply(image);
  m3ApiSuccess();
}

// Apply Gaussian blur using a box blur approximation
static m3ApiRawFunction (null0_ImageBlurGaussian) {

  m3ApiGetArgMem(Image*, image);
  m3ApiGetArg(uint32_t, blurSize);

  ImageBlurGaussian(image, blurSize);
  m3ApiSuccess();
}

// Resize image (Bicubic scaling algorithm)
static m3ApiRawFunction (null0_ImageResize) {

  m3ApiGetArgMem(Image*, image);
  m3ApiGetArg(uint32_t, newWidth);
  m3ApiGetArg(uint32_t, newHeight);

  ImageResize(image, newWidth, newHeight);
  m3ApiSuccess();
}

// Resize image (Nearest-Neighbor scaling algorithm)
static m3ApiRawFunction (null0_ImageResizeNN) {

  m3ApiGetArgMem(Image*, image);
  m3ApiGetArg(uint32_t, newWidth);
  m3ApiGetArg(uint32_t, newHeight);

  ImageResizeNN(image, newWidth, newHeight);
  m3ApiSuccess();
}

// Resize canvas and fill with color
static m3ApiRawFunction (null0_ImageResizeCanvas) {

  m3ApiGetArgMem(Image*, image);
  m3ApiGetArg(uint32_t, newWidth);
  m3ApiGetArg(uint32_t, newHeight);
  m3ApiGetArg(uint32_t, offsetX);
  m3ApiGetArg(uint32_t, offsetY);
  m3ApiGetArg(Color, fill);

  ImageResizeCanvas(image, newWidth, newHeight, offsetX, offsetY, fill);
  m3ApiSuccess();
}

// Compute all mipmap levels for a provided image
static m3ApiRawFunction (null0_ImageMipmaps) {

  m3ApiGetArgMem(Image*, image);

  ImageMipmaps(image);
  m3ApiSuccess();
}

// Dither image data to 16bpp or lower (Floyd-Steinberg dithering)
static m3ApiRawFunction (null0_ImageDither) {

  m3ApiGetArgMem(Image*, image);
  m3ApiGetArg(uint32_t, rBpp);
  m3ApiGetArg(uint32_t, gBpp);
  m3ApiGetArg(uint32_t, bBpp);
  m3ApiGetArg(uint32_t, aBpp);

  ImageDither(image, rBpp, gBpp, bBpp, aBpp);
  m3ApiSuccess();
}

// Flip image vertically
static m3ApiRawFunction (null0_ImageFlipVertical) {

  m3ApiGetArgMem(Image*, image);

  ImageFlipVertical(image);
  m3ApiSuccess();
}

// Flip image horizontally
static m3ApiRawFunction (null0_ImageFlipHorizontal) {

  m3ApiGetArgMem(Image*, image);

  ImageFlipHorizontal(image);
  m3ApiSuccess();
}

// Rotate image clockwise 90deg
static m3ApiRawFunction (null0_ImageRotateCW) {

  m3ApiGetArgMem(Image*, image);

  ImageRotateCW(image);
  m3ApiSuccess();
}

// Rotate image counter-clockwise 90deg
static m3ApiRawFunction (null0_ImageRotateCCW) {

  m3ApiGetArgMem(Image*, image);

  ImageRotateCCW(image);
  m3ApiSuccess();
}

// Modify image color: tint
static m3ApiRawFunction (null0_ImageColorTint) {

  m3ApiGetArgMem(Image*, image);
  m3ApiGetArg(Color, color);

  ImageColorTint(image, color);
  m3ApiSuccess();
}

// Modify image color: invert
static m3ApiRawFunction (null0_ImageColorInvert) {

  m3ApiGetArgMem(Image*, image);

  ImageColorInvert(image);
  m3ApiSuccess();
}

// Modify image color: grayscale
static m3ApiRawFunction (null0_ImageColorGrayscale) {

  m3ApiGetArgMem(Image*, image);

  ImageColorGrayscale(image);
  m3ApiSuccess();
}

// Modify image color: contrast (-100 to 100)
static m3ApiRawFunction (null0_ImageColorContrast) {

  m3ApiGetArgMem(Image*, image);
  m3ApiGetArg(float, contrast);

  ImageColorContrast(image, contrast);
  m3ApiSuccess();
}

// Modify image color: brightness (-255 to 255)
static m3ApiRawFunction (null0_ImageColorBrightness) {

  m3ApiGetArgMem(Image*, image);
  m3ApiGetArg(uint32_t, brightness);

  ImageColorBrightness(image, brightness);
  m3ApiSuccess();
}

// Modify image color: replace color
static m3ApiRawFunction (null0_ImageColorReplace) {

  m3ApiGetArgMem(Image*, image);
  m3ApiGetArg(Color, color);
  m3ApiGetArg(Color, replace);

  ImageColorReplace(image, color, replace);
  m3ApiSuccess();
}

// Get image pixel color at (x, y) position
static m3ApiRawFunction (null0_GetImageColor) {
  m3ApiReturnType (Color); // Color
  m3ApiGetArg(Image, image);
  m3ApiGetArg(uint32_t, x);
  m3ApiGetArg(uint32_t, y);
  m3ApiReturn(GetImageColor(image, x, y));

}

// Clear image background with given color
static m3ApiRawFunction (null0_ImageClearBackground) {

  m3ApiGetArgMem(Image*, dst);
  m3ApiGetArg(Color, color);

  ImageClearBackground(dst, color);
  m3ApiSuccess();
}

// Draw pixel within an image
static m3ApiRawFunction (null0_ImageDrawPixel) {

  m3ApiGetArgMem(Image*, dst);
  m3ApiGetArg(uint32_t, posX);
  m3ApiGetArg(uint32_t, posY);
  m3ApiGetArg(Color, color);

  ImageDrawPixel(dst, posX, posY, color);
  m3ApiSuccess();
}

// Draw pixel within an image (Vector version)
static m3ApiRawFunction (null0_ImageDrawPixelV) {

  m3ApiGetArgMem(Image*, dst);
  m3ApiGetArg(Vector2, position);
  m3ApiGetArg(Color, color);

  ImageDrawPixelV(dst, position, color);
  m3ApiSuccess();
}

// Draw line within an image (Vector version)
static m3ApiRawFunction (null0_ImageDrawLineV) {

  m3ApiGetArgMem(Image*, dst);
  m3ApiGetArg(Vector2, start);
  m3ApiGetArg(Vector2, end);
  m3ApiGetArg(Color, color);

  ImageDrawLineV(dst, start, end, color);
  m3ApiSuccess();
}

// Draw a filled circle within an image (Vector version)
static m3ApiRawFunction (null0_ImageDrawCircleV) {

  m3ApiGetArgMem(Image*, dst);
  m3ApiGetArg(Vector2, center);
  m3ApiGetArg(uint32_t, radius);
  m3ApiGetArg(Color, color);

  ImageDrawCircleV(dst, center, radius, color);
  m3ApiSuccess();
}

// Draw circle outline within an image
static m3ApiRawFunction (null0_ImageDrawCircleLines) {

  m3ApiGetArgMem(Image*, dst);
  m3ApiGetArg(uint32_t, centerX);
  m3ApiGetArg(uint32_t, centerY);
  m3ApiGetArg(uint32_t, radius);
  m3ApiGetArg(Color, color);

  ImageDrawCircleLines(dst, centerX, centerY, radius, color);
  m3ApiSuccess();
}

// Draw circle outline within an image (Vector version)
static m3ApiRawFunction (null0_ImageDrawCircleLinesV) {

  m3ApiGetArgMem(Image*, dst);
  m3ApiGetArg(Vector2, center);
  m3ApiGetArg(uint32_t, radius);
  m3ApiGetArg(Color, color);

  ImageDrawCircleLinesV(dst, center, radius, color);
  m3ApiSuccess();
}

// Draw rectangle within an image
static m3ApiRawFunction (null0_ImageDrawRectangle) {

  m3ApiGetArgMem(Image*, dst);
  m3ApiGetArg(uint32_t, posX);
  m3ApiGetArg(uint32_t, posY);
  m3ApiGetArg(uint32_t, width);
  m3ApiGetArg(uint32_t, height);
  m3ApiGetArg(Color, color);

  ImageDrawRectangle(dst, posX, posY, width, height, color);
  m3ApiSuccess();
}

// Draw rectangle within an image (Vector version)
static m3ApiRawFunction (null0_ImageDrawRectangleV) {

  m3ApiGetArgMem(Image*, dst);
  m3ApiGetArg(Vector2, position);
  m3ApiGetArg(Vector2, size);
  m3ApiGetArg(Color, color);

  ImageDrawRectangleV(dst, position, size, color);
  m3ApiSuccess();
}

// Draw rectangle within an image
static m3ApiRawFunction (null0_ImageDrawRectangleRec) {

  m3ApiGetArgMem(Image*, dst);
  m3ApiGetArg(Rectangle, rec);
  m3ApiGetArg(Color, color);

  ImageDrawRectangleRec(dst, rec, color);
  m3ApiSuccess();
}

// Draw rectangle lines within an image
static m3ApiRawFunction (null0_ImageDrawRectangleLines) {

  m3ApiGetArgMem(Image*, dst);
  m3ApiGetArg(Rectangle, rec);
  m3ApiGetArg(uint32_t, thick);
  m3ApiGetArg(Color, color);

  ImageDrawRectangleLines(dst, rec, thick, color);
  m3ApiSuccess();
}

// Draw text (using default font) within an image (destination)
static m3ApiRawFunction (null0_ImageDrawText) {

  m3ApiGetArgMem(Image*, dst);
  m3ApiGetArg(Font, font);
  m3ApiGetArgMem(const char*, text);
  m3ApiGetArg(uint32_t, posX);
  m3ApiGetArg(uint32_t, posY);
  m3ApiGetArg(uint32_t, fontSize);
  m3ApiGetArg(Color, color);

  ImageDrawText(dst, font, text, posX, posY, fontSize, color);
  m3ApiSuccess();
}

// Draw text (custom sprite font) within an image (destination)
static m3ApiRawFunction (null0_ImageDrawTextEx) {

  m3ApiGetArgMem(Image*, dst);
  m3ApiGetArg(Font, font);
  m3ApiGetArgMem(const char*, text);
  m3ApiGetArg(Vector2, position);
  m3ApiGetArg(float, fontSize);
  m3ApiGetArg(float, spacing);
  m3ApiGetArg(Color, tint);

  ImageDrawTextEx(dst, font, text, position, fontSize, spacing, tint);
  m3ApiSuccess();
}

// Unload texture from GPU memory (VRAM)
static m3ApiRawFunction (null0_UnloadTexture) {

  m3ApiGetArg(Texture2D, texture);

  UnloadTexture(texture);
  m3ApiSuccess();
}

// Unload render texture from GPU memory (VRAM)
static m3ApiRawFunction (null0_UnloadRenderTexture) {

  m3ApiGetArg(RenderTexture2D, target);

  UnloadRenderTexture(target);
  m3ApiSuccess();
}

// Update GPU texture with new data
static m3ApiRawFunction (null0_UpdateTexture) {

  m3ApiGetArg(Texture2D, texture);
  m3ApiGetArgMem(const void*, pixels);

  UpdateTexture(texture, pixels);
  m3ApiSuccess();
}

// Update GPU texture rectangle with new data
static m3ApiRawFunction (null0_UpdateTextureRec) {

  m3ApiGetArg(Texture2D, texture);
  m3ApiGetArg(Rectangle, rec);
  m3ApiGetArgMem(const void*, pixels);

  UpdateTextureRec(texture, rec, pixels);
  m3ApiSuccess();
}

// Generate GPU mipmaps for a texture
static m3ApiRawFunction (null0_GenTextureMipmaps) {

  m3ApiGetArgMem(Texture2D*, texture);

  GenTextureMipmaps(texture);
  m3ApiSuccess();
}

// Set texture scaling filter mode
static m3ApiRawFunction (null0_SetTextureFilter) {

  m3ApiGetArg(Texture2D, texture);
  m3ApiGetArg(uint32_t, filter);

  SetTextureFilter(texture, filter);
  m3ApiSuccess();
}

// Set texture wrapping mode
static m3ApiRawFunction (null0_SetTextureWrap) {

  m3ApiGetArg(Texture2D, texture);
  m3ApiGetArg(uint32_t, wrap);

  SetTextureWrap(texture, wrap);
  m3ApiSuccess();
}

// Draw a Texture2D
static m3ApiRawFunction (null0_DrawTexture) {

  m3ApiGetArg(Texture2D, texture);
  m3ApiGetArg(uint32_t, posX);
  m3ApiGetArg(uint32_t, posY);
  m3ApiGetArg(Color, tint);

  DrawTexture(texture, posX, posY, tint);
  m3ApiSuccess();
}

// Draw a Texture2D with position defined as Vector2
static m3ApiRawFunction (null0_DrawTextureV) {

  m3ApiGetArg(Texture2D, texture);
  m3ApiGetArg(Vector2, position);
  m3ApiGetArg(Color, tint);

  DrawTextureV(texture, position, tint);
  m3ApiSuccess();
}

// Draw a Texture2D with extended parameters
static m3ApiRawFunction (null0_DrawTextureEx) {

  m3ApiGetArg(Texture2D, texture);
  m3ApiGetArg(Vector2, position);
  m3ApiGetArg(float, rotation);
  m3ApiGetArg(float, scale);
  m3ApiGetArg(Color, tint);

  DrawTextureEx(texture, position, rotation, scale, tint);
  m3ApiSuccess();
}

// Draw a part of a texture defined by a rectangle
static m3ApiRawFunction (null0_DrawTextureRec) {

  m3ApiGetArg(Texture2D, texture);
  m3ApiGetArg(Rectangle, source);
  m3ApiGetArg(Vector2, position);
  m3ApiGetArg(Color, tint);

  DrawTextureRec(texture, source, position, tint);
  m3ApiSuccess();
}

// Draw a part of a texture defined by a rectangle with 'pro' parameters
static m3ApiRawFunction (null0_DrawTexturePro) {

  m3ApiGetArg(Texture2D, texture);
  m3ApiGetArg(Rectangle, source);
  m3ApiGetArg(Rectangle, dest);
  m3ApiGetArg(Vector2, origin);
  m3ApiGetArg(float, rotation);
  m3ApiGetArg(Color, tint);

  DrawTexturePro(texture, source, dest, origin, rotation, tint);
  m3ApiSuccess();
}

// Draws a texture (or part of it) that stretches or shrinks nicely
static m3ApiRawFunction (null0_DrawTextureNPatch) {

  m3ApiGetArg(Texture2D, texture);
  m3ApiGetArg(NPatchInfo, nPatchInfo);
  m3ApiGetArg(Rectangle, dest);
  m3ApiGetArg(Vector2, origin);
  m3ApiGetArg(float, rotation);
  m3ApiGetArg(Color, tint);

  DrawTextureNPatch(texture, nPatchInfo, dest, origin, rotation, tint);
  m3ApiSuccess();
}

// Get color with alpha applied, alpha goes from 0.0f to 1.0f
static m3ApiRawFunction (null0_Fade) {
  m3ApiReturnType (Color); // Color
  m3ApiGetArg(Color, color);
  m3ApiGetArg(float, alpha);
  m3ApiReturn(Fade(color, alpha));

}

// Get hexadecimal value for a Color
static m3ApiRawFunction (null0_ColorToInt) {
  m3ApiReturnType (uint32_t); // int
  m3ApiGetArg(Color, color);
  m3ApiReturn(ColorToInt(color));

}

// Get Color normalized as float [0..1]
static m3ApiRawFunction (null0_ColorNormalize) {
  m3ApiReturnType (Vector4); // Vector4
  m3ApiGetArg(Color, color);
  m3ApiReturn(ColorNormalize(color));

}

// Get Color from normalized values [0..1]
static m3ApiRawFunction (null0_ColorFromNormalized) {
  m3ApiReturnType (Color); // Color
  m3ApiGetArg(Vector4, normalized);
  m3ApiReturn(ColorFromNormalized(normalized));

}

// Get HSV values for a Color, hue [0..360], saturation/value [0..1]
static m3ApiRawFunction (null0_ColorToHSV) {
  m3ApiReturnType (Vector3); // Vector3
  m3ApiGetArg(Color, color);
  m3ApiReturn(ColorToHSV(color));

}

// Get a Color from HSV values, hue [0..360], saturation/value [0..1]
static m3ApiRawFunction (null0_ColorFromHSV) {
  m3ApiReturnType (Color); // Color
  m3ApiGetArg(float, hue);
  m3ApiGetArg(float, saturation);
  m3ApiGetArg(float, value);
  m3ApiReturn(ColorFromHSV(hue, saturation, value));

}

// Get color multiplied with another color
static m3ApiRawFunction (null0_ColorTint) {
  m3ApiReturnType (Color); // Color
  m3ApiGetArg(Color, color);
  m3ApiGetArg(Color, tint);
  m3ApiReturn(ColorTint(color, tint));

}

// Get color with brightness correction, brightness factor goes from -1.0f to 1.0f
static m3ApiRawFunction (null0_ColorBrightness) {
  m3ApiReturnType (Color); // Color
  m3ApiGetArg(Color, color);
  m3ApiGetArg(float, factor);
  m3ApiReturn(ColorBrightness(color, factor));

}

// Get color with contrast correction, contrast values between -1.0f and 1.0f
static m3ApiRawFunction (null0_ColorContrast) {
  m3ApiReturnType (Color); // Color
  m3ApiGetArg(Color, color);
  m3ApiGetArg(float, contrast);
  m3ApiReturn(ColorContrast(color, contrast));

}

// Get color with alpha applied, alpha goes from 0.0f to 1.0f
static m3ApiRawFunction (null0_ColorAlpha) {
  m3ApiReturnType (Color); // Color
  m3ApiGetArg(Color, color);
  m3ApiGetArg(float, alpha);
  m3ApiReturn(ColorAlpha(color, alpha));

}

// Get src alpha-blended into dst color with tint
static m3ApiRawFunction (null0_ColorAlphaBlend) {
  m3ApiReturnType (Color); // Color
  m3ApiGetArg(Color, dst);
  m3ApiGetArg(Color, src);
  m3ApiGetArg(Color, tint);
  m3ApiReturn(ColorAlphaBlend(dst, src, tint));

}

// Get Color structure from hexadecimal value
static m3ApiRawFunction (null0_GetColor) {
  m3ApiReturnType (Color); // Color
  m3ApiGetArg(unsigned int, hexValue);
  m3ApiReturn(GetColor(hexValue));

}

// Get pixel data size in bytes for certain format
static m3ApiRawFunction (null0_GetPixelDataSize) {
  m3ApiReturnType (uint32_t); // int
  m3ApiGetArg(uint32_t, width);
  m3ApiGetArg(uint32_t, height);
  m3ApiGetArg(uint32_t, format);
  m3ApiReturn(GetPixelDataSize(width, height, format));

}

// Generate image font atlas using chars info
static m3ApiRawFunction (null0_GenImageFontAtlas) {
  m3ApiReturnType (Image); // Image
  m3ApiGetArgMem(const GlyphInfo*, chars);
  m3ApiGetArgMem(Rectangle**, recs);
  m3ApiGetArg(uint32_t, glyphCount);
  m3ApiGetArg(uint32_t, fontSize);
  m3ApiGetArg(uint32_t, padding);
  m3ApiGetArg(uint32_t, packMethod);
  m3ApiReturn(GenImageFontAtlas(chars, recs, glyphCount, fontSize, padding, packMethod));

}

// Unload font chars info data (RAM)
static m3ApiRawFunction (null0_UnloadFontData) {

  m3ApiGetArgMem(GlyphInfo*, chars);
  m3ApiGetArg(uint32_t, glyphCount);

  UnloadFontData(chars, glyphCount);
  m3ApiSuccess();
}

// Unload font from GPU memory (VRAM)
static m3ApiRawFunction (null0_UnloadFont) {

  m3ApiGetArg(Font, font);

  UnloadFont(font);
  m3ApiSuccess();
}

// Draw current FPS
static m3ApiRawFunction (null0_DrawFPS) {

  m3ApiGetArg(uint32_t, posX);
  m3ApiGetArg(uint32_t, posY);

  DrawFPS(posX, posY);
  m3ApiSuccess();
}

// Draw text (using default font)
static m3ApiRawFunction (null0_DrawText) {

  m3ApiGetArgMem(const char*, text);
  m3ApiGetArg(uint32_t, posX);
  m3ApiGetArg(uint32_t, posY);
  m3ApiGetArg(uint32_t, fontSize);
  m3ApiGetArg(Color, color);

  DrawText(text, posX, posY, fontSize, color);
  m3ApiSuccess();
}

// Draw text using font and additional parameters
static m3ApiRawFunction (null0_DrawTextEx) {

  m3ApiGetArg(Font, font);
  m3ApiGetArgMem(const char*, text);
  m3ApiGetArg(Vector2, position);
  m3ApiGetArg(float, fontSize);
  m3ApiGetArg(float, spacing);
  m3ApiGetArg(Color, tint);

  DrawTextEx(font, text, position, fontSize, spacing, tint);
  m3ApiSuccess();
}

// Draw text using Font and pro parameters (rotation)
static m3ApiRawFunction (null0_DrawTextPro) {

  m3ApiGetArg(Font, font);
  m3ApiGetArgMem(const char*, text);
  m3ApiGetArg(Vector2, position);
  m3ApiGetArg(Vector2, origin);
  m3ApiGetArg(float, rotation);
  m3ApiGetArg(float, fontSize);
  m3ApiGetArg(float, spacing);
  m3ApiGetArg(Color, tint);

  DrawTextPro(font, text, position, origin, rotation, fontSize, spacing, tint);
  m3ApiSuccess();
}

// Draw one character (codepoint)
static m3ApiRawFunction (null0_DrawTextCodepoint) {

  m3ApiGetArg(Font, font);
  m3ApiGetArg(uint32_t, codepoint);
  m3ApiGetArg(Vector2, position);
  m3ApiGetArg(float, fontSize);
  m3ApiGetArg(Color, tint);

  DrawTextCodepoint(font, codepoint, position, fontSize, tint);
  m3ApiSuccess();
}

// Draw multiple character (codepoint)
static m3ApiRawFunction (null0_DrawTextCodepoints) {

  m3ApiGetArg(Font, font);
  m3ApiGetArgMem(const int*, codepoints);
  m3ApiGetArg(uint32_t, count);
  m3ApiGetArg(Vector2, position);
  m3ApiGetArg(float, fontSize);
  m3ApiGetArg(float, spacing);
  m3ApiGetArg(Color, tint);

  DrawTextCodepoints(font, codepoints, count, position, fontSize, spacing, tint);
  m3ApiSuccess();
}

// Get glyph index position in font for a codepoint (unicode character), fallback to '?' if not found
static m3ApiRawFunction (null0_GetGlyphIndex) {
  m3ApiReturnType (uint32_t); // int
  m3ApiGetArg(Font, font);
  m3ApiGetArg(uint32_t, codepoint);
  m3ApiReturn(GetGlyphIndex(font, codepoint));

}

// Load all codepoints from a UTF-8 text string, codepoints count returned by parameter
static m3ApiRawFunction (null0_LoadCodepoints) {
  m3ApiReturnType (uint32_t); // int
  m3ApiGetArgMem(const char*, text);
  m3ApiGetArgMem(int*, count);
  m3ApiReturn(LoadCodepoints(text, count));

}

// Unload codepoints data from memory
static m3ApiRawFunction (null0_UnloadCodepoints) {

  m3ApiGetArgMem(int*, codepoints);

  UnloadCodepoints(codepoints);
  m3ApiSuccess();
}

// Get total number of codepoints in a UTF-8 encoded string
static m3ApiRawFunction (null0_GetCodepointCount) {
  m3ApiReturnType (uint32_t); // int
  m3ApiGetArgMem(const char*, text);
  m3ApiReturn(GetCodepointCount(text));

}

// Get next codepoint in a UTF-8 encoded string, 0x3f('?') is returned on failure
static m3ApiRawFunction (null0_GetCodepoint) {
  m3ApiReturnType (uint32_t); // int
  m3ApiGetArgMem(const char*, text);
  m3ApiGetArgMem(int*, codepointSize);
  m3ApiReturn(GetCodepoint(text, codepointSize));

}

// Get next codepoint in a UTF-8 encoded string, 0x3f('?') is returned on failure
static m3ApiRawFunction (null0_GetCodepointNext) {
  m3ApiReturnType (uint32_t); // int
  m3ApiGetArgMem(const char*, text);
  m3ApiGetArgMem(int*, codepointSize);
  m3ApiReturn(GetCodepointNext(text, codepointSize));

}

// Get previous codepoint in a UTF-8 encoded string, 0x3f('?') is returned on failure
static m3ApiRawFunction (null0_GetCodepointPrevious) {
  m3ApiReturnType (uint32_t); // int
  m3ApiGetArgMem(const char*, text);
  m3ApiGetArgMem(int*, codepointSize);
  m3ApiReturn(GetCodepointPrevious(text, codepointSize));

}

// Copy one string to another, returns bytes copied
static m3ApiRawFunction (null0_TextCopy) {
  m3ApiReturnType (uint32_t); // int
  m3ApiGetArgMem(char*, dst);
  m3ApiGetArgMem(const char*, src);
  m3ApiReturn(TextCopy(dst, src));

}

// Check if two text string are equal
static m3ApiRawFunction (null0_TextIsEqual) {
  m3ApiReturnType (bool); // bool
  m3ApiGetArgMem(const char*, text1);
  m3ApiGetArgMem(const char*, text2);
  m3ApiReturn(TextIsEqual(text1, text2));

}

// Append text at specific position and move cursor!
static m3ApiRawFunction (null0_TextAppend) {

  m3ApiGetArgMem(char*, text);
  m3ApiGetArgMem(const char*, append);
  m3ApiGetArgMem(int*, position);

  TextAppend(text, append, position);
  m3ApiSuccess();
}

// Find first text occurrence within a string
static m3ApiRawFunction (null0_TextFindIndex) {
  m3ApiReturnType (uint32_t); // int
  m3ApiGetArgMem(const char*, text);
  m3ApiGetArgMem(const char*, find);
  m3ApiReturn(TextFindIndex(text, find));

}

// Get integer value from text (negative values not supported)
static m3ApiRawFunction (null0_TextToInteger) {
  m3ApiReturnType (uint32_t); // int
  m3ApiGetArgMem(const char*, text);
  m3ApiReturn(TextToInteger(text));

}

*/

// call this in your game-loop
void null0_update() {
  gettimeofday(&now, NULL);
  if (cart_update) {
    null0_check_wasm3(m3_CallV(cart_update, (now.tv_sec - start.tv_sec) * 1000000 + now.tv_usec - start.tv_usec));
  } else {
    // TODO: some cute embedded "no update" screen here
  }
}

// this is the actual wasm-runner (after you have setup FS and stuff)
bool null0_start(const void* wasmBuffer, size_t byteLength) {
  gettimeofday(&start, NULL);

  env = m3_NewEnvironment();
  runtime = m3_NewRuntime(env, 1024 * 1024, NULL);
  null0_check_wasm3(m3_ParseModule(env, &module, wasmBuffer, byteLength));
  null0_check_wasm3(m3_LoadModule(runtime, module));

  // IMPORTS
  m3_LinkRawFunction(module, "env", "seed", "F()", &null0_seed);
  m3_LinkRawFunction(module, "env", "null0_fatal", "v(**ii)", &null0_fatal);
  m3_LinkRawFunction(module, "env", "null0_log", "v(*)", &null0_log);
  m3_LinkRawFunction(module, "env", "null0_ReadText", "*(*)", &null0_ReadText);
  m3_LinkRawFunction(module, "env", "null0_ClearBackground", "v(i)", &null0_ClearBackground);
  m3_LinkRawFunction(module, "env", "null0_DrawText", "v(iiiii)", &null0_DrawText);
  m3_LinkRawFunction(module, "env", "null0_DrawCircle", "v(iiii)", &null0_DrawCircle);
  m3_LinkRawFunction(module, "env", "null0_DrawLine", "v(iiiii)", &null0_DrawLine);
  m3_LinkRawFunction(module, "env", "null0_DrawRectangle", "v(iiiii)", &null0_DrawRectangle);
  m3_LinkRawFunction(module, "env", "null0_LoadImage", "*(*)", &null0_LoadImage);
  m3_LinkRawFunction(module, "env", "null0_Draw", "v(****)", &null0_Draw);

  // m3_LinkRawFunction(module, "env", "null0_LoadImageAnim", "i(ii)", &null0_LoadImageAnim);
  // m3_LinkRawFunction(module, "env", "null0_UnloadImage", "v(i)", &null0_UnloadImage);
  // m3_LinkRawFunction(module, "env", "null0_ExportImage", "i(ii)", &null0_ExportImage);
  // m3_LinkRawFunction(module, "env", "null0_GenImageColor", "i(iii)", &null0_GenImageColor);
  // m3_LinkRawFunction(module, "env", "null0_GenImageGradientV", "i(iiii)", &null0_GenImageGradientV);
  // m3_LinkRawFunction(module, "env", "null0_GenImageGradientH", "i(iiii)", &null0_GenImageGradientH);
  // m3_LinkRawFunction(module, "env", "null0_GenImageGradientRadial", "i(iiiii)", &null0_GenImageGradientRadial);
  // m3_LinkRawFunction(module, "env", "null0_GenImageChecked", "i(iiiiii)", &null0_GenImageChecked);
  // m3_LinkRawFunction(module, "env", "null0_GenImageWhiteNoise", "i(iii)", &null0_GenImageWhiteNoise);
  // m3_LinkRawFunction(module, "env", "null0_GenImagePerlinNoise", "i(iiiii)", &null0_GenImagePerlinNoise);
  // m3_LinkRawFunction(module, "env", "null0_GenImageCellular", "i(iii)", &null0_GenImageCellular);
  // m3_LinkRawFunction(module, "env", "null0_GenImageText", "i(iii)", &null0_GenImageText);
  // m3_LinkRawFunction(module, "env", "null0_ImageCopy", "i(i)", &null0_ImageCopy);
  // m3_LinkRawFunction(module, "env", "null0_ImageFromImage", "i(ii)", &null0_ImageFromImage);
  // m3_LinkRawFunction(module, "env", "null0_ImageText", "i(iii)", &null0_ImageText);
  // m3_LinkRawFunction(module, "env", "null0_ImageTextEx", "i(iiiii)", &null0_ImageTextEx);
  // m3_LinkRawFunction(module, "env", "null0_ImageFormat", "v(ii)", &null0_ImageFormat);
  // m3_LinkRawFunction(module, "env", "null0_ImageToPOT", "v(ii)", &null0_ImageToPOT);
  // m3_LinkRawFunction(module, "env", "null0_ImageCrop", "v(ii)", &null0_ImageCrop);
  // m3_LinkRawFunction(module, "env", "null0_ImageAlphaCrop", "v(ii)", &null0_ImageAlphaCrop);
  // m3_LinkRawFunction(module, "env", "null0_ImageAlphaClear", "v(iii)", &null0_ImageAlphaClear);
  // m3_LinkRawFunction(module, "env", "null0_ImageAlphaMask", "v(ii)", &null0_ImageAlphaMask);
  // m3_LinkRawFunction(module, "env", "null0_ImageAlphaPremultiply", "v(i)", &null0_ImageAlphaPremultiply);
  // m3_LinkRawFunction(module, "env", "null0_ImageBlurGaussian", "v(ii)", &null0_ImageBlurGaussian);
  // m3_LinkRawFunction(module, "env", "null0_ImageResize", "v(iii)", &null0_ImageResize);
  // m3_LinkRawFunction(module, "env", "null0_ImageResizeNN", "v(iii)", &null0_ImageResizeNN);
  // m3_LinkRawFunction(module, "env", "null0_ImageResizeCanvas", "v(iiiiii)", &null0_ImageResizeCanvas);
  // m3_LinkRawFunction(module, "env", "null0_ImageMipmaps", "v(i)", &null0_ImageMipmaps);
  // m3_LinkRawFunction(module, "env", "null0_ImageDither", "v(iiiii)", &null0_ImageDither);
  // m3_LinkRawFunction(module, "env", "null0_ImageFlipVertical", "v(i)", &null0_ImageFlipVertical);
  // m3_LinkRawFunction(module, "env", "null0_ImageFlipHorizontal", "v(i)", &null0_ImageFlipHorizontal);
  // m3_LinkRawFunction(module, "env", "null0_ImageRotateCW", "v(i)", &null0_ImageRotateCW);
  // m3_LinkRawFunction(module, "env", "null0_ImageRotateCCW", "v(i)", &null0_ImageRotateCCW);
  // m3_LinkRawFunction(module, "env", "null0_ImageColorTint", "v(ii)", &null0_ImageColorTint);
  // m3_LinkRawFunction(module, "env", "null0_ImageColorInvert", "v(i)", &null0_ImageColorInvert);
  // m3_LinkRawFunction(module, "env", "null0_ImageColorGrayscale", "v(i)", &null0_ImageColorGrayscale);
  // m3_LinkRawFunction(module, "env", "null0_ImageColorContrast", "v(ii)", &null0_ImageColorContrast);
  // m3_LinkRawFunction(module, "env", "null0_ImageColorBrightness", "v(ii)", &null0_ImageColorBrightness);
  // m3_LinkRawFunction(module, "env", "null0_ImageColorReplace", "v(iii)", &null0_ImageColorReplace);
  // m3_LinkRawFunction(module, "env", "null0_GetImageColor", "i(iii)", &null0_GetImageColor);
  // m3_LinkRawFunction(module, "env", "null0_ImageClearBackground", "v(ii)", &null0_ImageClearBackground);
  // m3_LinkRawFunction(module, "env", "null0_ImageDrawPixel", "v(iiii)", &null0_ImageDrawPixel);
  // m3_LinkRawFunction(module, "env", "null0_ImageDrawPixelV", "v(iii)", &null0_ImageDrawPixelV);
  // m3_LinkRawFunction(module, "env", "null0_ImageDrawLine", "v(iiiiii)", &null0_ImageDrawLine);
  // m3_LinkRawFunction(module, "env", "null0_ImageDrawLineV", "v(iiii)", &null0_ImageDrawLineV);
  // m3_LinkRawFunction(module, "env", "null0_ImageDrawCircle", "v(iiiii)", &null0_ImageDrawCircle);
  // m3_LinkRawFunction(module, "env", "null0_ImageDrawCircleV", "v(iiii)", &null0_ImageDrawCircleV);
  // m3_LinkRawFunction(module, "env", "null0_ImageDrawCircleLines", "v(iiiii)", &null0_ImageDrawCircleLines);
  // m3_LinkRawFunction(module, "env", "null0_ImageDrawCircleLinesV", "v(iiii)", &null0_ImageDrawCircleLinesV);
  // m3_LinkRawFunction(module, "env", "null0_ImageDrawRectangle", "v(iiiiii)", &null0_ImageDrawRectangle);
  // m3_LinkRawFunction(module, "env", "null0_ImageDrawRectangleV", "v(iiii)", &null0_ImageDrawRectangleV);
  // m3_LinkRawFunction(module, "env", "null0_ImageDrawRectangleRec", "v(iii)", &null0_ImageDrawRectangleRec);
  // m3_LinkRawFunction(module, "env", "null0_ImageDrawRectangleLines", "v(iiii)", &null0_ImageDrawRectangleLines);
  // m3_LinkRawFunction(module, "env", "null0_ImageDrawText", "v(iiiiiii)", &null0_ImageDrawText);
  // m3_LinkRawFunction(module, "env", "null0_ImageDrawTextEx", "v(iiiiiii)", &null0_ImageDrawTextEx);
  // m3_LinkRawFunction(module, "env", "null0_UnloadTexture", "v(i)", &null0_UnloadTexture);
  // m3_LinkRawFunction(module, "env", "null0_UnloadRenderTexture", "v(i)", &null0_UnloadRenderTexture);
  // m3_LinkRawFunction(module, "env", "null0_UpdateTexture", "v(ii)", &null0_UpdateTexture);
  // m3_LinkRawFunction(module, "env", "null0_UpdateTextureRec", "v(iii)", &null0_UpdateTextureRec);
  // m3_LinkRawFunction(module, "env", "null0_GenTextureMipmaps", "v(i)", &null0_GenTextureMipmaps);
  // m3_LinkRawFunction(module, "env", "null0_SetTextureFilter", "v(ii)", &null0_SetTextureFilter);
  // m3_LinkRawFunction(module, "env", "null0_SetTextureWrap", "v(ii)", &null0_SetTextureWrap);
  // m3_LinkRawFunction(module, "env", "null0_DrawTexture", "v(iiii)", &null0_DrawTexture);
  // m3_LinkRawFunction(module, "env", "null0_DrawTextureV", "v(iii)", &null0_DrawTextureV);
  // m3_LinkRawFunction(module, "env", "null0_DrawTextureEx", "v(iiiii)", &null0_DrawTextureEx);
  // m3_LinkRawFunction(module, "env", "null0_DrawTextureRec", "v(iiii)", &null0_DrawTextureRec);
  // m3_LinkRawFunction(module, "env", "null0_DrawTexturePro", "v(iiiiii)", &null0_DrawTexturePro);
  // m3_LinkRawFunction(module, "env", "null0_DrawTextureNPatch", "v(iiiiii)", &null0_DrawTextureNPatch);
  // m3_LinkRawFunction(module, "env", "null0_Fade", "i(ii)", &null0_Fade);
  // m3_LinkRawFunction(module, "env", "null0_ColorToInt", "i(i)", &null0_ColorToInt);
  // m3_LinkRawFunction(module, "env", "null0_ColorNormalize", "i(i)", &null0_ColorNormalize);
  // m3_LinkRawFunction(module, "env", "null0_ColorFromNormalized", "i(i)", &null0_ColorFromNormalized);
  // m3_LinkRawFunction(module, "env", "null0_ColorToHSV", "i(i)", &null0_ColorToHSV);
  // m3_LinkRawFunction(module, "env", "null0_ColorFromHSV", "i(iii)", &null0_ColorFromHSV);
  // m3_LinkRawFunction(module, "env", "null0_ColorTint", "i(ii)", &null0_ColorTint);
  // m3_LinkRawFunction(module, "env", "null0_ColorBrightness", "i(ii)", &null0_ColorBrightness);
  // m3_LinkRawFunction(module, "env", "null0_ColorContrast", "i(ii)", &null0_ColorContrast);
  // m3_LinkRawFunction(module, "env", "null0_ColorAlpha", "i(ii)", &null0_ColorAlpha);
  // m3_LinkRawFunction(module, "env", "null0_ColorAlphaBlend", "i(iii)", &null0_ColorAlphaBlend);
  // m3_LinkRawFunction(module, "env", "null0_GetColor", "i(i)", &null0_GetColor);
  // m3_LinkRawFunction(module, "env", "null0_GetPixelDataSize", "i(iii)", &null0_GetPixelDataSize);
  // m3_LinkRawFunction(module, "env", "null0_GenImageFontAtlas", "i(iiiiii)", &null0_GenImageFontAtlas);
  // m3_LinkRawFunction(module, "env", "null0_UnloadFontData", "v(ii)", &null0_UnloadFontData);
  // m3_LinkRawFunction(module, "env", "null0_UnloadFont", "v(i)", &null0_UnloadFont);
  // m3_LinkRawFunction(module, "env", "null0_DrawFPS", "v(ii)", &null0_DrawFPS);
  // m3_LinkRawFunction(module, "env", "null0_DrawTextEx", "v(iiiiii)", &null0_DrawTextEx);
  // m3_LinkRawFunction(module, "env", "null0_DrawTextPro", "v(iiiiiiii)", &null0_DrawTextPro);
  // m3_LinkRawFunction(module, "env", "null0_DrawTextCodepoint", "v(iiiii)", &null0_DrawTextCodepoint);
  // m3_LinkRawFunction(module, "env", "null0_DrawTextCodepoints", "v(iiiiiii)", &null0_DrawTextCodepoints);
  // m3_LinkRawFunction(module, "env", "null0_GetGlyphIndex", "i(ii)", &null0_GetGlyphIndex);
  // m3_LinkRawFunction(module, "env", "null0_LoadCodepoints", "i(ii)", &null0_LoadCodepoints);
  // m3_LinkRawFunction(module, "env", "null0_UnloadCodepoints", "v(i)", &null0_UnloadCodepoints);
  // m3_LinkRawFunction(module, "env", "null0_GetCodepointCount", "i(i)", &null0_GetCodepointCount);
  // m3_LinkRawFunction(module, "env", "null0_GetCodepoint", "i(ii)", &null0_GetCodepoint);
  // m3_LinkRawFunction(module, "env", "null0_GetCodepointNext", "i(ii)", &null0_GetCodepointNext);
  // m3_LinkRawFunction(module, "env", "null0_GetCodepointPrevious", "i(ii)", &null0_GetCodepointPrevious);
  // m3_LinkRawFunction(module, "env", "null0_TextCopy", "i(ii)", &null0_TextCopy);
  // m3_LinkRawFunction(module, "env", "null0_TextIsEqual", "i(ii)", &null0_TextIsEqual);
  // m3_LinkRawFunction(module, "env", "null0_TextAppend", "v(iii)", &null0_TextAppend);
  // m3_LinkRawFunction(module, "env", "null0_TextFindIndex", "i(ii)", &null0_TextFindIndex);
  // m3_LinkRawFunction(module, "env", "null0_TextToInteger", "i(i)", &null0_TextToInteger);

  null0_check_wasm3_is_ok();

  // EXPORTS
  m3_FindFunction(&new_func, runtime, "__new");
  m3_FindFunction(&cart_load, runtime, "load");
  m3_FindFunction(&cart_update, runtime, "update");

  null0_check_wasm3_is_ok();

  // TODO: call wasm conf and get info before allowing write
  // PHYSFS_setWriteDir(PHYSFS_getPrefDir("null0", name));

  if (!new_func) {
    // this means no string-returns
    // it should be exported: __new(size: usize, id: u32 (1)): usize
    printf("null0: no __new() (string-retuns disabled.)\n");
  }

  if (cart_load) {
    null0_check_wasm3(m3_CallV(cart_load));
  }

  if (!cart_update) {
    // do load of any assets for "no update" screen here
  }
  return true;
}

// load a "cart", which is either a directory or a wasm/zip file
bool null0_load(Image* imageBuffer, const char* fileBuffer, size_t byteLength, const char* filePath) {
  PHYSFS_init("null0");
  screenBuffer = imageBuffer;

  DIR* dirptr;
  if (access(filePath, F_OK) != -1) {
    if ((dirptr = opendir(filePath)) != NULL) {
      // filePath is a dir
      PHYSFS_mount(filePath, NULL, 0);
    } else {
      if (fileBuffer[0] == 0x50 && fileBuffer[1] == 0x4b && fileBuffer[2] == 0x03 && fileBuffer[3] == 0x04) {
        // fileBuffer is a zip
        // TODO: mount the bytes, instead of path (no duplication of reading bytes)
        PHYSFS_mount(filePath, NULL, 0);
      } else if (fileBuffer[0] == 0x00 && fileBuffer[1] == 0x61 && fileBuffer[2] == 0x73 && fileBuffer[3] == 0x6d) {
        // fileBuffer is a wasm-file
        return null0_start(fileBuffer, byteLength);
      } else {
        // fileBuffer is a non-wasm file
        fprintf(stderr, "null0: Cart is not a directory, zip or wasm file.\n");
        return false;
      }
    }
    // if they are still here, it means filesystem is mounted and should have a main.wasm file
    if (!FileExistsInPhysFS("main.wasm")) {
      fprintf(stderr, "null0: No main.wasm.\n");
      return false;
    }
    PHYSFS_File* wasmFile = PHYSFS_openRead("main.wasm");
    PHYSFS_uint64 wasmLen = PHYSFS_fileLength(wasmFile);
    u8* wasmBuffer[wasmLen];
    PHYSFS_sint64 bytesRead = PHYSFS_readBytes(wasmFile, wasmBuffer, wasmLen);
    if (bytesRead == -1) {
      fprintf(stderr, "null0: Error opening main.wasm: %s\n", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
      return false;
    }
    return null0_start(wasmBuffer, bytesRead);
  } else {
    fprintf(stderr, "null0: Can't access cart.\n");
    return false;
  }
}

// this is called when the cart unloads
void null_unload() {}
