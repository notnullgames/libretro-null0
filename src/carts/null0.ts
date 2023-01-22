// null0 assemblyscript header, generated 2023-01-22T03:07:12.018Z

@unmanaged
export class Color {
  r: u8;
  g: u8;
  b: u8;
  a: u8;
}

@unmanaged
export class Vector3 {
  x: f32;
  y: f32;
  z: f32;
}

@unmanaged
export class Vector4 {
  x: f32;
  y: f32;
  z: f32;
  w: f32;
}

@unmanaged
export class Rectangle {
  x: f32;
  y: f32;
  height: f32;
  width: f32;
}

export const LIGHTGRAY: Color = { r:  200, g: 200, b: 200, a: 255 }
export const GRAY: Color      = { r:  130, g: 130, b: 130, a: 255 }
export const DARKGRAY: Color  = { r:  80,  g: 80,  b: 80 , a: 255 }
export const LIGHTGREY: Color = { r:  200, g: 200, b: 200, a: 255 }
export const GREY: Color      = { r:  130, g: 130, b: 130, a: 255 }
export const DARKGREY: Color  = { r:  80,  g: 80,  b: 80 , a: 255 }
export const YELLOW: Color    = { r:  253, g: 249, b: 0  , a: 255 }
export const GOLD: Color      = { r:  255, g: 203, b: 0  , a: 255 }
export const ORANGE: Color    = { r:  255, g: 161, b: 0  , a: 255 }
export const PINK: Color      = { r:  255, g: 109, b: 194, a: 255 }
export const RED: Color       = { r:  230, g: 41,  b: 55 , a: 255 }
export const MAROON: Color    = { r:  190, g: 33,  b: 55 , a: 255 }
export const GREEN: Color     = { r:  0,   g: 228, b: 48 , a: 255 }
export const LIME: Color      = { r:  0,   g: 158, b: 47 , a: 255 }
export const DARKGREEN: Color = { r:  0,   g: 117, b: 44 , a: 255 }
export const SKYBLUE: Color   = { r:  102, g: 191, b: 255, a: 255 }
export const BLUE: Color      = { r:  0,   g: 121, b: 241, a: 255 }
export const DARKBLUE: Color  = { r:  0,   g: 82,  b: 172, a: 255 }
export const PURPLE: Color    = { r:  200, g: 122, b: 255, a: 255 }
export const VIOLET: Color    = { r:  135, g: 60,  b: 190, a: 255 }
export const DARKPURPL: Color = { r:  112, g: 31,  b: 126, a: 255 }
export const BEIGE: Color     = { r:  211, g: 176, b: 131, a: 255 }
export const BROWN: Color     = { r:  127, g: 106, b: 79 , a: 255 }
export const DARKBROWN: Color = { r:  76,  g: 63,  b: 47 , a: 255 }
export const WHITE: Color     = { r:  255, g: 255, b: 255, a: 255 }
export const BLACK: Color     = { r:  0,   g: 0,   b: 0  , a: 255 }
export const BLANK: Color     = { r:  0,   g: 0,   b: 0  , a: 0   }
export const MAGENTA: Color   = { r:  255, g: 0,   b: 255, a: 255 }
export const RAYWHITE: Color  = { r:  245, g: 245, b: 245, a: 255 }

// IMPORTS

// Load image from file into CPU memory (RAM)
@external("env", "null0_LoadImage")
declare function _LoadImage(fileName: ArrayBuffer): u32
export function LoadImage(fileName: string): u32 {
  return _LoadImage(String.UTF8.encode(fileName, true))
}

// Load image sequence from file (frames appended to image.data)
@external("env", "null0_LoadImageAnim")
declare function _LoadImageAnim(fileName: ArrayBuffer, frames: i32[]): u32
export function LoadImageAnim(fileName: string, frames: i32[]): u32 {
  return _LoadImageAnim(String.UTF8.encode(fileName, true), frames)
}

// Load image from screen buffer and (screenshot)
@external("env", "null0_LoadImageFromScreen")
export declare function LoadImageFromScreen(undefined: void): u32

// Unload image from CPU memory (RAM)
@external("env", "null0_UnloadImage")
export declare function UnloadImage(image: u32): void

// Export image data to file, returns true on success
@external("env", "null0_ExportImage")
declare function _ExportImage(image: u32, fileName: ArrayBuffer): bool
export function ExportImage(image: u32, fileName: string): bool {
  return _ExportImage(image, String.UTF8.encode(fileName, true))
}

// Generate image: plain color
@external("env", "null0_GenImageColor")
export declare function GenImageColor(width: i32, height: i32, color: Color): u32

// Generate image: vertical gradient
@external("env", "null0_GenImageGradientV")
export declare function GenImageGradientV(width: i32, height: i32, top: Color, bottom: Color): u32

// Generate image: horizontal gradient
@external("env", "null0_GenImageGradientH")
export declare function GenImageGradientH(width: i32, height: i32, left: Color, right: Color): u32

// Generate image: radial gradient
@external("env", "null0_GenImageGradientRadial")
export declare function GenImageGradientRadial(width: i32, height: i32, density: float, inner: Color, outer: Color): u32

// Generate image: checked
@external("env", "null0_GenImageChecked")
export declare function GenImageChecked(width: i32, height: i32, checksX: i32, checksY: i32, col1: Color, col2: Color): u32

// Generate image: white noise
@external("env", "null0_GenImageWhiteNoise")
export declare function GenImageWhiteNoise(width: i32, height: i32, factor: float): u32

// Generate image: perlin noise
@external("env", "null0_GenImagePerlinNoise")
export declare function GenImagePerlinNoise(width: i32, height: i32, offsetX: i32, offsetY: i32, scale: float): u32

// Generate image: cellular algorithm, bigger tileSize means bigger cells
@external("env", "null0_GenImageCellular")
export declare function GenImageCellular(width: i32, height: i32, tileSize: i32): u32

// Generate image: grayscale image from text data
@external("env", "null0_GenImageText")
declare function _GenImageText(width: i32, height: i32, text: ArrayBuffer): u32
export function GenImageText(width: i32, height: i32, text: string): u32 {
  return _GenImageText(width, height, String.UTF8.encode(text, true))
}

// Create an image duplicate (useful for transformations)
@external("env", "null0_ImageCopy")
export declare function ImageCopy(image: u32): u32

// Create an image from another image piece
@external("env", "null0_ImageFromImage")
export declare function ImageFromImage(image: u32, rec: Rectangle): u32

// Create an image from text (default font)
@external("env", "null0_ImageText")
declare function _ImageText(text: ArrayBuffer, fontSize: i32, color: Color): u32
export function ImageText(text: string, fontSize: i32, color: Color): u32 {
  return _ImageText(String.UTF8.encode(text, true), fontSize, color)
}

// Convert image data to desired format
@external("env", "null0_ImageFormat")
export declare function ImageFormat(image: u32, newFormat: i32): void

// Convert image to POT (power-of-two)
@external("env", "null0_ImageToPOT")
export declare function ImageToPOT(image: u32, fill: Color): void

// Crop an image to a defined rectangle
@external("env", "null0_ImageCrop")
export declare function ImageCrop(image: u32, crop: Rectangle): void

// Crop image depending on alpha value
@external("env", "null0_ImageAlphaCrop")
export declare function ImageAlphaCrop(image: u32, threshold: float): void

// Clear alpha channel to desired color
@external("env", "null0_ImageAlphaClear")
export declare function ImageAlphaClear(image: u32, color: Color, threshold: float): void

// Apply alpha mask to image
@external("env", "null0_ImageAlphaMask")
export declare function ImageAlphaMask(image: u32, alphaMask: u32): void

// Premultiply alpha channel
@external("env", "null0_ImageAlphaPremultiply")
export declare function ImageAlphaPremultiply(image: u32): void

// Apply Gaussian blur using a box blur approximation
@external("env", "null0_ImageBlurGaussian")
export declare function ImageBlurGaussian(image: u32, blurSize: i32): void

// Resize image (Bicubic scaling algorithm)
@external("env", "null0_ImageResize")
export declare function ImageResize(image: u32, newWidth: i32, newHeight: i32): void

// Resize image (Nearest-Neighbor scaling algorithm)
@external("env", "null0_ImageResizeNN")
export declare function ImageResizeNN(image: u32, newWidth: i32, newHeight: i32): void

// Resize canvas and fill with color
@external("env", "null0_ImageResizeCanvas")
export declare function ImageResizeCanvas(image: u32, newWidth: i32, newHeight: i32, offsetX: i32, offsetY: i32, fill: Color): void

// Compute all mipmap levels for a provided image
@external("env", "null0_ImageMipmaps")
export declare function ImageMipmaps(image: u32): void

// Dither image data to 16bpp or lower (Floyd-Steinberg dithering)
@external("env", "null0_ImageDither")
export declare function ImageDither(image: u32, rBpp: i32, gBpp: i32, bBpp: i32, aBpp: i32): void

// Flip image vertically
@external("env", "null0_ImageFlipVertical")
export declare function ImageFlipVertical(image: u32): void

// Flip image horizontally
@external("env", "null0_ImageFlipHorizontal")
export declare function ImageFlipHorizontal(image: u32): void

// Rotate image clockwise 90deg
@external("env", "null0_ImageRotateCW")
export declare function ImageRotateCW(image: u32): void

// Rotate image counter-clockwise 90deg
@external("env", "null0_ImageRotateCCW")
export declare function ImageRotateCCW(image: u32): void

// Modify image color: tint
@external("env", "null0_ImageColorTint")
export declare function ImageColorTint(image: u32, color: Color): void

// Modify image color: invert
@external("env", "null0_ImageColorInvert")
export declare function ImageColorInvert(image: u32): void

// Modify image color: grayscale
@external("env", "null0_ImageColorGrayscale")
export declare function ImageColorGrayscale(image: u32): void

// Modify image color: contrast (-100 to 100)
@external("env", "null0_ImageColorContrast")
export declare function ImageColorContrast(image: u32, contrast: float): void

// Modify image color: brightness (-255 to 255)
@external("env", "null0_ImageColorBrightness")
export declare function ImageColorBrightness(image: u32, brightness: i32): void

// Modify image color: replace color
@external("env", "null0_ImageColorReplace")
export declare function ImageColorReplace(image: u32, color: Color, replace: Color): void

// Get image pixel color at (x, y) position
@external("env", "null0_GetImageColor")
export declare function GetImageColor(image: u32, x: i32, y: i32): Color

// Clear image background with given color
@external("env", "null0_ImageClearBackground")
export declare function ImageClearBackground(dst: u32, color: Color): void

// Draw pixel within an image
@external("env", "null0_ImageDrawPixel")
export declare function ImageDrawPixel(dst: u32, posX: i32, posY: i32, color: Color): void

// Draw pixel within an image (Vector version)
@external("env", "null0_ImageDrawPixelV")
export declare function ImageDrawPixelV(dst: u32, position: Vector2, color: Color): void

// Draw line within an image
@external("env", "null0_ImageDrawLine")
export declare function ImageDrawLine(dst: u32, startPosX: i32, startPosY: i32, endPosX: i32, endPosY: i32, color: Color): void

// Draw line within an image (Vector version)
@external("env", "null0_ImageDrawLineV")
export declare function ImageDrawLineV(dst: u32, start: Vector2, end: Vector2, color: Color): void

// Draw circle within an image
@external("env", "null0_ImageDrawCircle")
export declare function ImageDrawCircle(dst: u32, centerX: i32, centerY: i32, radius: i32, color: Color): void

// Draw circle within an image (Vector version)
@external("env", "null0_ImageDrawCircleV")
export declare function ImageDrawCircleV(dst: u32, center: Vector2, radius: i32, color: Color): void

// Draw circle outline within an image
@external("env", "null0_ImageDrawCircleLines")
export declare function ImageDrawCircleLines(dst: u32, centerX: i32, centerY: i32, radius: i32, color: Color): void

// Draw circle outline within an image (Vector version)
@external("env", "null0_ImageDrawCircleLinesV")
export declare function ImageDrawCircleLinesV(dst: u32, center: Vector2, radius: i32, color: Color): void

// Draw rectangle within an image
@external("env", "null0_ImageDrawRectangle")
export declare function ImageDrawRectangle(dst: u32, posX: i32, posY: i32, width: i32, height: i32, color: Color): void

// Draw rectangle within an image (Vector version)
@external("env", "null0_ImageDrawRectangleV")
export declare function ImageDrawRectangleV(dst: u32, position: Vector2, size: Vector2, color: Color): void

// Draw rectangle within an image
@external("env", "null0_ImageDrawRectangleRec")
export declare function ImageDrawRectangleRec(dst: u32, rec: Rectangle, color: Color): void

// Draw rectangle lines within an image
@external("env", "null0_ImageDrawRectangleLines")
export declare function ImageDrawRectangleLines(dst: u32, rec: Rectangle, thick: i32, color: Color): void

// Draw a source image within a destination image (tint applied to source)
@external("env", "null0_ImageDraw")
export declare function ImageDraw(dst: u32, src: u32, srcRec: Rectangle, dstRec: Rectangle, tint: Color): void

// Draw text (using default font) within an image (destination)
@external("env", "null0_ImageDrawText")
declare function _ImageDrawText(dst: u32, text: ArrayBuffer, posX: i32, posY: i32, fontSize: i32, color: Color): void
export function ImageDrawText(dst: u32, text: string, posX: i32, posY: i32, fontSize: i32, color: Color): void {
  return _ImageDrawText(dst, String.UTF8.encode(text, true), posX, posY, fontSize, color)
}

// Get color with alpha applied, alpha goes from 0.0f to 1.0f
@external("env", "null0_Fade")
export declare function Fade(color: Color, alpha: float): Color

// Get hexadecimal value for a Color
@external("env", "null0_ColorToInt")
export declare function ColorToInt(color: Color): i32

// Get Color normalized as float [0..1]
@external("env", "null0_ColorNormalize")
export declare function ColorNormalize(color: Color): Vector4

// Get Color from normalized values [0..1]
@external("env", "null0_ColorFromNormalized")
export declare function ColorFromNormalized(normalized: Vector4): Color

// Get HSV values for a Color, hue [0..360], saturation/value [0..1]
@external("env", "null0_ColorToHSV")
export declare function ColorToHSV(color: Color): Vector3

// Get a Color from HSV values, hue [0..360], saturation/value [0..1]
@external("env", "null0_ColorFromHSV")
export declare function ColorFromHSV(hue: float, saturation: float, value: float): Color

// Get color multiplied with another color
@external("env", "null0_ColorTint")
export declare function ColorTint(color: Color, tint: Color): Color

// Get color with brightness correction, brightness factor goes from -1.0f to 1.0f
@external("env", "null0_ColorBrightness")
export declare function ColorBrightness(color: Color, factor: float): Color

// Get color with contrast correction, contrast values between -1.0f and 1.0f
@external("env", "null0_ColorContrast")
export declare function ColorContrast(color: Color, contrast: float): Color

// Get color with alpha applied, alpha goes from 0.0f to 1.0f
@external("env", "null0_ColorAlpha")
export declare function ColorAlpha(color: Color, alpha: float): Color

// Get src alpha-blended into dst color with tint
@external("env", "null0_ColorAlphaBlend")
export declare function ColorAlphaBlend(dst: Color, src: Color, tint: Color): Color

// Get Color structure from hexadecimal value
@external("env", "null0_GetColor")
export declare function GetColor(hexValue: u32): Color

// Get pixel data size in bytes for certain format
@external("env", "null0_GetPixelDataSize")
export declare function GetPixelDataSize(width: i32, height: i32, format: i32): i32

// log a string
@external("env", "null0_log")
declare function _log(text: ArrayBuffer): void
export function log(text: string): void {
  return _log(String.UTF8.encode(text, true))
}

// Clear image background with given color
@external("env", "null0_ClearBackground")
export declare function ClearBackground(color: Color): void

// Draw pixel on the screen
@external("env", "null0_DrawPixel")
export declare function DrawPixel(posX: i32, posY: i32, color: Color): void

// Draw pixel on the screen (Vector version)
@external("env", "null0_DrawPixelV")
export declare function DrawPixelV(position: Vector2, color: Color): void

// Draw line on the screen
@external("env", "null0_DrawLine")
export declare function DrawLine(startPosX: i32, startPosY: i32, endPosX: i32, endPosY: i32, color: Color): void

// Draw line on the screen (Vector version)
@external("env", "null0_DrawLineV")
export declare function DrawLineV(start: Vector2, end: Vector2, color: Color): void

// Draw circle on the screen
@external("env", "null0_DrawCircle")
export declare function DrawCircle(centerX: i32, centerY: i32, radius: i32, color: Color): void

// Draw circle on the screen (Vector version)
@external("env", "null0_DrawCircleV")
export declare function DrawCircleV(center: Vector2, radius: i32, color: Color): void

// Draw rectangle on the screen
@external("env", "null0_DrawRectangle")
export declare function DrawRectangle(posX: i32, posY: i32, width: i32, height: i32, color: Color): void

// Draw rectangle on the screen (Vector version)
@external("env", "null0_DrawRectangleV")
export declare function DrawRectangleV(position: Vector2, size: Vector2, color: Color): void

// Draw rectangle on the screen
@external("env", "null0_DrawRectangleRec")
export declare function DrawRectangleRec(rec: Rectangle, color: Color): void

// Draw rectangle lines on the screen
@external("env", "null0_DrawRectangleLines")
export declare function DrawRectangleLines(rec: Rectangle, thick: i32, color: Color): void

// Draw a source image within a destination image (tint applied to source)
@external("env", "null0_Draw")
export declare function Draw(src: u32, srcRec: Rectangle, dstRec: Rectangle, tint: Color): void

// Draw text (using default font) on the screen (destination)
@external("env", "null0_DrawText")
declare function _DrawText(text: ArrayBuffer, posX: i32, posY: i32, fontSize: i32, color: Color): void
export function DrawText(text: string, posX: i32, posY: i32, fontSize: i32, color: Color): void {
  return _DrawText(String.UTF8.encode(text, true), posX, posY, fontSize, color)
}


