// null0 host C header, generated 2023-01-22T05:06:25.552Z

#include <sys/time.h>

#include "wasm3.h"
#include "m3_env.h"
#include "physfs.h"

#define RIMAGE_IMPLEMENTATION
#include "rimage.h"

static M3Environment* env;
static M3Runtime* runtime;
static M3Module* module;

static M3Function* new_func;
static M3Function* cart_load;
static M3Function* cart_update;
Image* screenBuffer;

struct timeval now, start;

// this checks the general state of the runtime, to make sure there are no errors lingering
static void null0_check_wasm3_is_ok () {
  M3ErrorInfo error;
  m3_GetErrorInfo(runtime, &error);
  if (error.result) {
    fprintf(stderr, "%s - %s\n", error.result, error.message);
  }
}

// all wasm3 functions return same sort of error-pattern, so this wraps that
static void null0_check_wasm3 (M3Result result) {
  if (result) {
    null0_check_wasm3_is_ok();
  }
}

// IMPORTS

// Fatal error
// this is an assemblyscript-thing, but you can export one from any language
// message/filename is WTF-16 encoded, which is a bit annoying
static m3ApiRawFunction (null0_abort) {
  m3ApiGetArgMem(const char*, message);
  m3ApiGetArgMem(const char*, fileName);
  m3ApiGetArg(uint16_t, lineNumber);
  m3ApiGetArg(uint16_t, columnNumber);
  fprintf(stderr, "%s at %s:%d:%d\n", message, fileName, lineNumber, columnNumber);
  m3ApiSuccess();
}

// Log a string
static m3ApiRawFunction (null0_log) {
  m3ApiGetArgMem(const char*, message);
  printf("%s\n", message);
  m3ApiSuccess();
}

// Load image from file into CPU memory (RAM)
static m3ApiRawFunction (null0_LoadImage) {
  m3ApiReturnType (Image);
  m3ApiGetArgMem(const char*, fileName);
  m3ApiReturn(LoadImage(fileName));
  
}

// Load image sequence from file (frames appended to image.data)
static m3ApiRawFunction (null0_LoadImageAnim) {
  m3ApiReturnType (Image);
  m3ApiGetArgMem(const char*, fileName);
  m3ApiGetArgMem(int*, frames);
  m3ApiReturn(LoadImageAnim(fileName, frames));
  
}

// Load image from screen buffer and (screenshot)
static m3ApiRawFunction (null0_LoadImageFromScreen) {
  m3ApiReturnType (Image);
  
  m3ApiReturn(LoadImageFromScreen());
  
}

// Unload image from CPU memory (RAM)
static m3ApiRawFunction (null0_UnloadImage) {
  
  m3ApiGetArg(Image, image);
  
  UnloadImage(image);
  m3ApiSuccess();
}

// Export image data to file, returns true on success
static m3ApiRawFunction (null0_ExportImage) {
  m3ApiReturnType (bool);
  m3ApiGetArg(Image, image);
  m3ApiGetArgMem(const char*, fileName);
  m3ApiReturn(ExportImage(image, fileName));
  
}

// Generate image: plain color
static m3ApiRawFunction (null0_GenImageColor) {
  m3ApiReturnType (Image);
  m3ApiGetArg(int, width);
  m3ApiGetArg(int, height);
  m3ApiGetArg(Color, color);
  m3ApiReturn(GenImageColor(width, height, color));
  
}

// Generate image: vertical gradient
static m3ApiRawFunction (null0_GenImageGradientV) {
  m3ApiReturnType (Image);
  m3ApiGetArg(int, width);
  m3ApiGetArg(int, height);
  m3ApiGetArg(Color, top);
  m3ApiGetArg(Color, bottom);
  m3ApiReturn(GenImageGradientV(width, height, top, bottom));
  
}

// Generate image: horizontal gradient
static m3ApiRawFunction (null0_GenImageGradientH) {
  m3ApiReturnType (Image);
  m3ApiGetArg(int, width);
  m3ApiGetArg(int, height);
  m3ApiGetArg(Color, left);
  m3ApiGetArg(Color, right);
  m3ApiReturn(GenImageGradientH(width, height, left, right));
  
}

// Generate image: radial gradient
static m3ApiRawFunction (null0_GenImageGradientRadial) {
  m3ApiReturnType (Image);
  m3ApiGetArg(int, width);
  m3ApiGetArg(int, height);
  m3ApiGetArg(float, density);
  m3ApiGetArg(Color, inner);
  m3ApiGetArg(Color, outer);
  m3ApiReturn(GenImageGradientRadial(width, height, density, inner, outer));
  
}

// Generate image: checked
static m3ApiRawFunction (null0_GenImageChecked) {
  m3ApiReturnType (Image);
  m3ApiGetArg(int, width);
  m3ApiGetArg(int, height);
  m3ApiGetArg(int, checksX);
  m3ApiGetArg(int, checksY);
  m3ApiGetArg(Color, col1);
  m3ApiGetArg(Color, col2);
  m3ApiReturn(GenImageChecked(width, height, checksX, checksY, col1, col2));
  
}

// Generate image: white noise
static m3ApiRawFunction (null0_GenImageWhiteNoise) {
  m3ApiReturnType (Image);
  m3ApiGetArg(int, width);
  m3ApiGetArg(int, height);
  m3ApiGetArg(float, factor);
  m3ApiReturn(GenImageWhiteNoise(width, height, factor));
  
}

// Generate image: perlin noise
static m3ApiRawFunction (null0_GenImagePerlinNoise) {
  m3ApiReturnType (Image);
  m3ApiGetArg(int, width);
  m3ApiGetArg(int, height);
  m3ApiGetArg(int, offsetX);
  m3ApiGetArg(int, offsetY);
  m3ApiGetArg(float, scale);
  m3ApiReturn(GenImagePerlinNoise(width, height, offsetX, offsetY, scale));
  
}

// Generate image: cellular algorithm, bigger tileSize means bigger cells
static m3ApiRawFunction (null0_GenImageCellular) {
  m3ApiReturnType (Image);
  m3ApiGetArg(int, width);
  m3ApiGetArg(int, height);
  m3ApiGetArg(int, tileSize);
  m3ApiReturn(GenImageCellular(width, height, tileSize));
  
}

// Generate image: grayscale image from text data
static m3ApiRawFunction (null0_GenImageText) {
  m3ApiReturnType (Image);
  m3ApiGetArg(int, width);
  m3ApiGetArg(int, height);
  m3ApiGetArgMem(const char*, text);
  m3ApiReturn(GenImageText(width, height, text));
  
}

// Create an image duplicate (useful for transformations)
static m3ApiRawFunction (null0_ImageCopy) {
  m3ApiReturnType (Image);
  m3ApiGetArg(Image, image);
  m3ApiReturn(ImageCopy(image));
  
}

// Create an image from another image piece
static m3ApiRawFunction (null0_ImageFromImage) {
  m3ApiReturnType (Image);
  m3ApiGetArg(Image, image);
  m3ApiGetArg(Rectangle, rec);
  m3ApiReturn(ImageFromImage(image, rec));
  
}

// Create an image from text (default font)
static m3ApiRawFunction (null0_ImageText) {
  m3ApiReturnType (Image);
  m3ApiGetArgMem(const char*, text);
  m3ApiGetArg(int, fontSize);
  m3ApiGetArg(Color, color);
  m3ApiReturn(ImageText(text, fontSize, color));
  
}

// Convert image data to desired format
static m3ApiRawFunction (null0_ImageFormat) {
  
  m3ApiGetArgMem(Image*, image);
  m3ApiGetArg(int, newFormat);
  
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
  m3ApiGetArg(int, blurSize);
  
  ImageBlurGaussian(image, blurSize);
  m3ApiSuccess();
}

// Resize image (Bicubic scaling algorithm)
static m3ApiRawFunction (null0_ImageResize) {
  
  m3ApiGetArgMem(Image*, image);
  m3ApiGetArg(int, newWidth);
  m3ApiGetArg(int, newHeight);
  
  ImageResize(image, newWidth, newHeight);
  m3ApiSuccess();
}

// Resize image (Nearest-Neighbor scaling algorithm)
static m3ApiRawFunction (null0_ImageResizeNN) {
  
  m3ApiGetArgMem(Image*, image);
  m3ApiGetArg(int, newWidth);
  m3ApiGetArg(int, newHeight);
  
  ImageResizeNN(image, newWidth, newHeight);
  m3ApiSuccess();
}

// Resize canvas and fill with color
static m3ApiRawFunction (null0_ImageResizeCanvas) {
  
  m3ApiGetArgMem(Image*, image);
  m3ApiGetArg(int, newWidth);
  m3ApiGetArg(int, newHeight);
  m3ApiGetArg(int, offsetX);
  m3ApiGetArg(int, offsetY);
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
  m3ApiGetArg(int, rBpp);
  m3ApiGetArg(int, gBpp);
  m3ApiGetArg(int, bBpp);
  m3ApiGetArg(int, aBpp);
  
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
  m3ApiGetArg(int, brightness);
  
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
  m3ApiReturnType (Color);
  m3ApiGetArg(Image, image);
  m3ApiGetArg(int, x);
  m3ApiGetArg(int, y);
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
  m3ApiGetArg(int, posX);
  m3ApiGetArg(int, posY);
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

// Draw line within an image
static m3ApiRawFunction (null0_ImageDrawLine) {
  
  m3ApiGetArgMem(Image*, dst);
  m3ApiGetArg(int, startPosX);
  m3ApiGetArg(int, startPosY);
  m3ApiGetArg(int, endPosX);
  m3ApiGetArg(int, endPosY);
  m3ApiGetArg(Color, color);
  
  ImageDrawLine(dst, startPosX, startPosY, endPosX, endPosY, color);
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

// Draw circle within an image
static m3ApiRawFunction (null0_ImageDrawCircle) {
  
  m3ApiGetArgMem(Image*, dst);
  m3ApiGetArg(int, centerX);
  m3ApiGetArg(int, centerY);
  m3ApiGetArg(int, radius);
  m3ApiGetArg(Color, color);
  
  ImageDrawCircle(dst, centerX, centerY, radius, color);
  m3ApiSuccess();
}

// Draw circle within an image (Vector version)
static m3ApiRawFunction (null0_ImageDrawCircleV) {
  
  m3ApiGetArgMem(Image*, dst);
  m3ApiGetArg(Vector2, center);
  m3ApiGetArg(int, radius);
  m3ApiGetArg(Color, color);
  
  ImageDrawCircleV(dst, center, radius, color);
  m3ApiSuccess();
}

// Draw circle outline within an image
static m3ApiRawFunction (null0_ImageDrawCircleLines) {
  
  m3ApiGetArgMem(Image*, dst);
  m3ApiGetArg(int, centerX);
  m3ApiGetArg(int, centerY);
  m3ApiGetArg(int, radius);
  m3ApiGetArg(Color, color);
  
  ImageDrawCircleLines(dst, centerX, centerY, radius, color);
  m3ApiSuccess();
}

// Draw circle outline within an image (Vector version)
static m3ApiRawFunction (null0_ImageDrawCircleLinesV) {
  
  m3ApiGetArgMem(Image*, dst);
  m3ApiGetArg(Vector2, center);
  m3ApiGetArg(int, radius);
  m3ApiGetArg(Color, color);
  
  ImageDrawCircleLinesV(dst, center, radius, color);
  m3ApiSuccess();
}

// Draw rectangle within an image
static m3ApiRawFunction (null0_ImageDrawRectangle) {
  
  m3ApiGetArgMem(Image*, dst);
  m3ApiGetArg(int, posX);
  m3ApiGetArg(int, posY);
  m3ApiGetArg(int, width);
  m3ApiGetArg(int, height);
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
  m3ApiGetArg(int, thick);
  m3ApiGetArg(Color, color);
  
  ImageDrawRectangleLines(dst, rec, thick, color);
  m3ApiSuccess();
}

// Draw a source image within a destination image (tint applied to source)
static m3ApiRawFunction (null0_ImageDraw) {
  
  m3ApiGetArgMem(Image*, dst);
  m3ApiGetArg(Image, src);
  m3ApiGetArg(Rectangle, srcRec);
  m3ApiGetArg(Rectangle, dstRec);
  m3ApiGetArg(Color, tint);
  
  ImageDraw(dst, src, srcRec, dstRec, tint);
  m3ApiSuccess();
}

// Draw text (using default font) within an image (destination)
static m3ApiRawFunction (null0_ImageDrawText) {
  
  m3ApiGetArgMem(Image*, dst);
  m3ApiGetArgMem(const char*, text);
  m3ApiGetArg(int, posX);
  m3ApiGetArg(int, posY);
  m3ApiGetArg(int, fontSize);
  m3ApiGetArg(Color, color);
  
  ImageDrawText(dst, text, posX, posY, fontSize, color);
  m3ApiSuccess();
}

// Get color with alpha applied, alpha goes from 0.0f to 1.0f
static m3ApiRawFunction (null0_Fade) {
  m3ApiReturnType (Color);
  m3ApiGetArg(Color, color);
  m3ApiGetArg(float, alpha);
  m3ApiReturn(Fade(color, alpha));
  
}

// Get hexadecimal value for a Color
static m3ApiRawFunction (null0_ColorToInt) {
  m3ApiReturnType (uint32_t);
  m3ApiGetArg(Color, color);
  m3ApiReturn(ColorToInt(color));
  
}

// Get Color normalized as float [0..1]
static m3ApiRawFunction (null0_ColorNormalize) {
  m3ApiReturnType (Vector4);
  m3ApiGetArg(Color, color);
  m3ApiReturn(ColorNormalize(color));
  
}

// Get Color from normalized values [0..1]
static m3ApiRawFunction (null0_ColorFromNormalized) {
  m3ApiReturnType (Color);
  m3ApiGetArg(Vector4, normalized);
  m3ApiReturn(ColorFromNormalized(normalized));
  
}

// Get HSV values for a Color, hue [0..360], saturation/value [0..1]
static m3ApiRawFunction (null0_ColorToHSV) {
  m3ApiReturnType (Vector3);
  m3ApiGetArg(Color, color);
  m3ApiReturn(ColorToHSV(color));
  
}

// Get a Color from HSV values, hue [0..360], saturation/value [0..1]
static m3ApiRawFunction (null0_ColorFromHSV) {
  m3ApiReturnType (Color);
  m3ApiGetArg(float, hue);
  m3ApiGetArg(float, saturation);
  m3ApiGetArg(float, value);
  m3ApiReturn(ColorFromHSV(hue, saturation, value));
  
}

// Get color multiplied with another color
static m3ApiRawFunction (null0_ColorTint) {
  m3ApiReturnType (Color);
  m3ApiGetArg(Color, color);
  m3ApiGetArg(Color, tint);
  m3ApiReturn(ColorTint(color, tint));
  
}

// Get color with brightness correction, brightness factor goes from -1.0f to 1.0f
static m3ApiRawFunction (null0_ColorBrightness) {
  m3ApiReturnType (Color);
  m3ApiGetArg(Color, color);
  m3ApiGetArg(float, factor);
  m3ApiReturn(ColorBrightness(color, factor));
  
}

// Get color with contrast correction, contrast values between -1.0f and 1.0f
static m3ApiRawFunction (null0_ColorContrast) {
  m3ApiReturnType (Color);
  m3ApiGetArg(Color, color);
  m3ApiGetArg(float, contrast);
  m3ApiReturn(ColorContrast(color, contrast));
  
}

// Get color with alpha applied, alpha goes from 0.0f to 1.0f
static m3ApiRawFunction (null0_ColorAlpha) {
  m3ApiReturnType (Color);
  m3ApiGetArg(Color, color);
  m3ApiGetArg(float, alpha);
  m3ApiReturn(ColorAlpha(color, alpha));
  
}

// Get src alpha-blended into dst color with tint
static m3ApiRawFunction (null0_ColorAlphaBlend) {
  m3ApiReturnType (Color);
  m3ApiGetArg(Color, dst);
  m3ApiGetArg(Color, src);
  m3ApiGetArg(Color, tint);
  m3ApiReturn(ColorAlphaBlend(dst, src, tint));
  
}

// Get Color structure from hexadecimal value
static m3ApiRawFunction (null0_GetColor) {
  m3ApiReturnType (Color);
  m3ApiGetArg(unsigned int, hexValue);
  m3ApiReturn(GetColor(hexValue));
  
}

// Get pixel data size in bytes for certain format
static m3ApiRawFunction (null0_GetPixelDataSize) {
  m3ApiReturnType (uint32_t);
  m3ApiGetArg(int, width);
  m3ApiGetArg(int, height);
  m3ApiGetArg(int, format);
  m3ApiReturn(GetPixelDataSize(width, height, format));
  
}


// call this in your game-loop
void null0_update_loop() {
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
  runtime = m3_NewRuntime (env, 1024 * 1024, NULL);
  null0_check_wasm3(m3_ParseModule (env, &module, wasmBuffer, byteLength));
  null0_check_wasm3(m3_LoadModule(runtime, module));

  // IMPORTS
  m3_LinkRawFunction(module, "env", "abort", "v(**ii)", &null0_abort);
  m3_LinkRawFunction(module, "env", "null0_log", "v(*)", &null0_log);
  m3_LinkRawFunction(module, "env", "null0_LoadImage", "i(i)", &null0_LoadImage);
  m3_LinkRawFunction(module, "env", "null0_LoadImageAnim", "i(ii)", &null0_LoadImageAnim);
  m3_LinkRawFunction(module, "env", "null0_LoadImageFromScreen", "i(i)", &null0_LoadImageFromScreen);
  m3_LinkRawFunction(module, "env", "null0_UnloadImage", "v(i)", &null0_UnloadImage);
  m3_LinkRawFunction(module, "env", "null0_ExportImage", "i(ii)", &null0_ExportImage);
  m3_LinkRawFunction(module, "env", "null0_GenImageColor", "i(iii)", &null0_GenImageColor);
  m3_LinkRawFunction(module, "env", "null0_GenImageGradientV", "i(iiii)", &null0_GenImageGradientV);
  m3_LinkRawFunction(module, "env", "null0_GenImageGradientH", "i(iiii)", &null0_GenImageGradientH);
  m3_LinkRawFunction(module, "env", "null0_GenImageGradientRadial", "i(iiiii)", &null0_GenImageGradientRadial);
  m3_LinkRawFunction(module, "env", "null0_GenImageChecked", "i(iiiiii)", &null0_GenImageChecked);
  m3_LinkRawFunction(module, "env", "null0_GenImageWhiteNoise", "i(iii)", &null0_GenImageWhiteNoise);
  m3_LinkRawFunction(module, "env", "null0_GenImagePerlinNoise", "i(iiiii)", &null0_GenImagePerlinNoise);
  m3_LinkRawFunction(module, "env", "null0_GenImageCellular", "i(iii)", &null0_GenImageCellular);
  m3_LinkRawFunction(module, "env", "null0_GenImageText", "i(iii)", &null0_GenImageText);
  m3_LinkRawFunction(module, "env", "null0_ImageCopy", "i(i)", &null0_ImageCopy);
  m3_LinkRawFunction(module, "env", "null0_ImageFromImage", "i(ii)", &null0_ImageFromImage);
  m3_LinkRawFunction(module, "env", "null0_ImageText", "i(iii)", &null0_ImageText);
  m3_LinkRawFunction(module, "env", "null0_ImageFormat", "v(ii)", &null0_ImageFormat);
  m3_LinkRawFunction(module, "env", "null0_ImageToPOT", "v(ii)", &null0_ImageToPOT);
  m3_LinkRawFunction(module, "env", "null0_ImageCrop", "v(ii)", &null0_ImageCrop);
  m3_LinkRawFunction(module, "env", "null0_ImageAlphaCrop", "v(ii)", &null0_ImageAlphaCrop);
  m3_LinkRawFunction(module, "env", "null0_ImageAlphaClear", "v(iii)", &null0_ImageAlphaClear);
  m3_LinkRawFunction(module, "env", "null0_ImageAlphaMask", "v(ii)", &null0_ImageAlphaMask);
  m3_LinkRawFunction(module, "env", "null0_ImageAlphaPremultiply", "v(i)", &null0_ImageAlphaPremultiply);
  m3_LinkRawFunction(module, "env", "null0_ImageBlurGaussian", "v(ii)", &null0_ImageBlurGaussian);
  m3_LinkRawFunction(module, "env", "null0_ImageResize", "v(iii)", &null0_ImageResize);
  m3_LinkRawFunction(module, "env", "null0_ImageResizeNN", "v(iii)", &null0_ImageResizeNN);
  m3_LinkRawFunction(module, "env", "null0_ImageResizeCanvas", "v(iiiiii)", &null0_ImageResizeCanvas);
  m3_LinkRawFunction(module, "env", "null0_ImageMipmaps", "v(i)", &null0_ImageMipmaps);
  m3_LinkRawFunction(module, "env", "null0_ImageDither", "v(iiiii)", &null0_ImageDither);
  m3_LinkRawFunction(module, "env", "null0_ImageFlipVertical", "v(i)", &null0_ImageFlipVertical);
  m3_LinkRawFunction(module, "env", "null0_ImageFlipHorizontal", "v(i)", &null0_ImageFlipHorizontal);
  m3_LinkRawFunction(module, "env", "null0_ImageRotateCW", "v(i)", &null0_ImageRotateCW);
  m3_LinkRawFunction(module, "env", "null0_ImageRotateCCW", "v(i)", &null0_ImageRotateCCW);
  m3_LinkRawFunction(module, "env", "null0_ImageColorTint", "v(ii)", &null0_ImageColorTint);
  m3_LinkRawFunction(module, "env", "null0_ImageColorInvert", "v(i)", &null0_ImageColorInvert);
  m3_LinkRawFunction(module, "env", "null0_ImageColorGrayscale", "v(i)", &null0_ImageColorGrayscale);
  m3_LinkRawFunction(module, "env", "null0_ImageColorContrast", "v(ii)", &null0_ImageColorContrast);
  m3_LinkRawFunction(module, "env", "null0_ImageColorBrightness", "v(ii)", &null0_ImageColorBrightness);
  m3_LinkRawFunction(module, "env", "null0_ImageColorReplace", "v(iii)", &null0_ImageColorReplace);
  m3_LinkRawFunction(module, "env", "null0_GetImageColor", "i(iii)", &null0_GetImageColor);
  m3_LinkRawFunction(module, "env", "null0_ImageClearBackground", "v(ii)", &null0_ImageClearBackground);
  m3_LinkRawFunction(module, "env", "null0_ImageDrawPixel", "v(iiii)", &null0_ImageDrawPixel);
  m3_LinkRawFunction(module, "env", "null0_ImageDrawPixelV", "v(iii)", &null0_ImageDrawPixelV);
  m3_LinkRawFunction(module, "env", "null0_ImageDrawLine", "v(iiiiii)", &null0_ImageDrawLine);
  m3_LinkRawFunction(module, "env", "null0_ImageDrawLineV", "v(iiii)", &null0_ImageDrawLineV);
  m3_LinkRawFunction(module, "env", "null0_ImageDrawCircle", "v(iiiii)", &null0_ImageDrawCircle);
  m3_LinkRawFunction(module, "env", "null0_ImageDrawCircleV", "v(iiii)", &null0_ImageDrawCircleV);
  m3_LinkRawFunction(module, "env", "null0_ImageDrawCircleLines", "v(iiiii)", &null0_ImageDrawCircleLines);
  m3_LinkRawFunction(module, "env", "null0_ImageDrawCircleLinesV", "v(iiii)", &null0_ImageDrawCircleLinesV);
  m3_LinkRawFunction(module, "env", "null0_ImageDrawRectangle", "v(iiiiii)", &null0_ImageDrawRectangle);
  m3_LinkRawFunction(module, "env", "null0_ImageDrawRectangleV", "v(iiii)", &null0_ImageDrawRectangleV);
  m3_LinkRawFunction(module, "env", "null0_ImageDrawRectangleRec", "v(iii)", &null0_ImageDrawRectangleRec);
  m3_LinkRawFunction(module, "env", "null0_ImageDrawRectangleLines", "v(iiii)", &null0_ImageDrawRectangleLines);
  m3_LinkRawFunction(module, "env", "null0_ImageDraw", "v(iiiii)", &null0_ImageDraw);
  m3_LinkRawFunction(module, "env", "null0_ImageDrawText", "v(iiiiii)", &null0_ImageDrawText);
  m3_LinkRawFunction(module, "env", "null0_Fade", "i(ii)", &null0_Fade);
  m3_LinkRawFunction(module, "env", "null0_ColorToInt", "i(i)", &null0_ColorToInt);
  m3_LinkRawFunction(module, "env", "null0_ColorNormalize", "i(i)", &null0_ColorNormalize);
  m3_LinkRawFunction(module, "env", "null0_ColorFromNormalized", "i(i)", &null0_ColorFromNormalized);
  m3_LinkRawFunction(module, "env", "null0_ColorToHSV", "i(i)", &null0_ColorToHSV);
  m3_LinkRawFunction(module, "env", "null0_ColorFromHSV", "i(iii)", &null0_ColorFromHSV);
  m3_LinkRawFunction(module, "env", "null0_ColorTint", "i(ii)", &null0_ColorTint);
  m3_LinkRawFunction(module, "env", "null0_ColorBrightness", "i(ii)", &null0_ColorBrightness);
  m3_LinkRawFunction(module, "env", "null0_ColorContrast", "i(ii)", &null0_ColorContrast);
  m3_LinkRawFunction(module, "env", "null0_ColorAlpha", "i(ii)", &null0_ColorAlpha);
  m3_LinkRawFunction(module, "env", "null0_ColorAlphaBlend", "i(iii)", &null0_ColorAlphaBlend);
  m3_LinkRawFunction(module, "env", "null0_GetColor", "i(i)", &null0_GetColor);
  m3_LinkRawFunction(module, "env", "null0_GetPixelDataSize", "i(iii)", &null0_GetPixelDataSize);

  null0_check_wasm3_is_ok();

  // EXPORTS
  m3_FindFunction(&new_func, runtime, "__new");
  m3_FindFunction(&cart_load, runtime, "load");
  m3_FindFunction(&cart_update, runtime, "update");

  null0_check_wasm3_is_ok();

  if (!new_func) {
    // this means no string-returns
    // it should be exported: __new(size: usize, id: u32 (1)): usize
    printf("no __new.\n");
  }

  if (cart_load) {
    null0_check_wasm3(m3_CallV(cart_load));
  } else {
    printf("no load.\n");
  }

  if (!cart_update) {
    printf("no update.\n");
    // do load of any assets for "no update" screen here
  }
  return true;
}

// load a "cart", which is either a directory or a wasm/zip file
bool null0_load(Image* imageBuffer, const void* fileBuffer, size_t byteLength, const char* filePath) {
  screenBuffer = imageBuffer;
}

// call this when you are ready to exit
void null_unload() {}


