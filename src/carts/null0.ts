// null0 assemblyscript header, generated 2023-01-22T01:23:48.560Z

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

// log a string
@external("env", "null0_log")
declare function null0_log(text: ArrayBuffer): void
export function log(text: string): void {
  null0_log(String.UTF8.encode(text, true))
}

export const LIGHTGRAY: Color = { r:  200, g: 200, b: 200, a: 0xFF }
export const GRAY: Color      = { r:  130, g: 130, b: 130, a: 0xFF }
export const DARKGRAY: Color  = { r:  80,  g: 80,  b: 80 , a: 0xFF }
export const LIGHTGREY: Color = { r:  200, g: 200, b: 200, a: 0xFF }
export const GREY: Color      = { r:  130, g: 130, b: 130, a: 0xFF }
export const DARKGREY: Color  = { r:  80,  g: 80,  b: 80 , a: 0xFF }
export const YELLOW: Color    = { r:  253, g: 249, b: 0  , a: 0xFF }
export const GOLD: Color      = { r:  255, g: 203, b: 0  , a: 0xFF }
export const ORANGE: Color    = { r:  255, g: 161, b: 0  , a: 0xFF }
export const PINK: Color      = { r:  255, g: 109, b: 194, a: 0xFF }
export const RED: Color       = { r:  230, g: 41,  b: 55 , a: 0xFF }
export const MAROON: Color    = { r:  190, g: 33,  b: 55 , a: 0xFF }
export const GREEN: Color     = { r:  0,   g: 228, b: 48 , a: 0xFF }
export const LIME: Color      = { r:  0,   g: 158, b: 47 , a: 0xFF }
export const DARKGREEN: Color = { r:  0,   g: 117, b: 44 , a: 0xFF }
export const SKYBLUE: Color   = { r:  102, g: 191, b: 255, a: 0xFF }
export const BLUE: Color      = { r:  0,   g: 121, b: 241, a: 0xFF }
export const DARKBLUE: Color  = { r:  0,   g: 82,  b: 172, a: 0xFF }
export const PURPLE: Color    = { r:  200, g: 122, b: 255, a: 0xFF }
export const VIOLET: Color    = { r:  135, g: 60,  b: 190, a: 0xFF }
export const DARKPURPL: Color = { r:  112, g: 31,  b: 126, a: 0xFF }
export const BEIGE: Color     = { r:  211, g: 176, b: 131, a: 0xFF }
export const BROWN: Color     = { r:  127, g: 106, b: 79 , a: 0xFF }
export const DARKBROWN: Color = { r:  76,  g: 63,  b: 47 , a: 0xFF }
export const WHITE: Color     = { r:  255, g: 255, b: 255, a: 0xFF }
export const BLACK: Color     = { r:  0,   g: 0,   b: 0  , a: 0xFF }
export const BLANK: Color     = { r:  0,   g: 0,   b: 0  , a: 0    }
export const MAGENTA: Color   = { r:  255, g: 0,   b: 255, a: 0xFF }
export const RAYWHITE: Color  = { r:  245, g: 245, b: 245, a: 0xFF }

// IMPORTS

// Load image from file into CPU memory (RAM)
@external("env", "null0_LoadImage")
export declare function LoadImage(): u32


// Load image sequence from file (frames appended to image.data)
@external("env", "null0_LoadImageAnim")
export declare function LoadImageAnim(frames: i32): u32


// Load image from screen buffer and (screenshot)
@external("env", "null0_LoadImageFromScreen")
export declare function LoadImageFromScreen(): u32


// Unload image from CPU memory (RAM)
@external("env", "null0_UnloadImage")
export declare function UnloadImage(): void


// Export image data to file, returns true on success
@external("env", "null0_ExportImage")
export declare function ExportImage(fileName: ArrayBuffer): bool


// Generate image: plain color
@external("env", "null0_GenImageColor")
export declare function GenImageColor(height: i32, color: Color): u32


// Generate image: vertical gradient
@external("env", "null0_GenImageGradientV")
export declare function GenImageGradientV(height: i32, top: Color, bottom: Color): u32


// Generate image: horizontal gradient
@external("env", "null0_GenImageGradientH")
export declare function GenImageGradientH(height: i32, left: Color, right: Color): u32


// Generate image: radial gradient
@external("env", "null0_GenImageGradientRadial")
export declare function GenImageGradientRadial(height: i32, density: float, inner: Color, outer: Color): u32


// Generate image: checked
@external("env", "null0_GenImageChecked")
export declare function GenImageChecked(height: i32, checksX: i32, checksY: i32, col1: Color, col2: Color): u32


// Generate image: white noise
@external("env", "null0_GenImageWhiteNoise")
export declare function GenImageWhiteNoise(height: i32, factor: float): u32


// Generate image: perlin noise
@external("env", "null0_GenImagePerlinNoise")
export declare function GenImagePerlinNoise(height: i32, offsetX: i32, offsetY: i32, scale: float): u32


// Generate image: cellular algorithm, bigger tileSize means bigger cells
@external("env", "null0_GenImageCellular")
export declare function GenImageCellular(height: i32, tileSize: i32): u32


// Create an image duplicate (useful for transformations)
@external("env", "null0_ImageCopy")
export declare function ImageCopy(): u32


// Create an image from another image piece
@external("env", "null0_ImageFromImage")
export declare function ImageFromImage(rec: Rectangle): u32


// Create an image from text (default font)
@external("env", "null0_ImageText")
export declare function ImageText(fontSize: i32, color: Color): u32


// Convert image data to desired format
@external("env", "null0_ImageFormat")
export declare function ImageFormat(newFormat: i32): void


// Convert image to POT (power-of-two)
@external("env", "null0_ImageToPOT")
export declare function ImageToPOT(fill: Color): void


// Crop an image to a defined rectangle
@external("env", "null0_ImageCrop")
export declare function ImageCrop(crop: Rectangle): void


// Crop image depending on alpha value
@external("env", "null0_ImageAlphaCrop")
export declare function ImageAlphaCrop(threshold: float): void


// Clear alpha channel to desired color
@external("env", "null0_ImageAlphaClear")
export declare function ImageAlphaClear(color: Color, threshold: float): void


// Apply alpha mask to image
@external("env", "null0_ImageAlphaMask")
export declare function ImageAlphaMask(alphaMask: u32): void


// Premultiply alpha channel
@external("env", "null0_ImageAlphaPremultiply")
export declare function ImageAlphaPremultiply(): void


// Resize image (Bicubic scaling algorithm)
@external("env", "null0_ImageResize")
export declare function ImageResize(newWidth: i32, newHeight: i32): void


// Resize image (Nearest-Neighbor scaling algorithm)
@external("env", "null0_ImageResizeNN")
export declare function ImageResizeNN(newWidth: i32, newHeight: i32): void


// Resize canvas and fill with color
@external("env", "null0_ImageResizeCanvas")
export declare function ImageResizeCanvas(newWidth: i32, newHeight: i32, offsetX: i32, offsetY: i32, fill: Color): void


// Compute all mipmap levels for a provided image
@external("env", "null0_ImageMipmaps")
export declare function ImageMipmaps(): void


// Dither image data to 16bpp or lower (Floyd-Steinberg dithering)
@external("env", "null0_ImageDither")
export declare function ImageDither(rBpp: i32, gBpp: i32, bBpp: i32, aBpp: i32): void


// Flip image vertically
@external("env", "null0_ImageFlipVertical")
export declare function ImageFlipVertical(): void


// Flip image horizontally
@external("env", "null0_ImageFlipHorizontal")
export declare function ImageFlipHorizontal(): void


// Rotate image clockwise 90deg
@external("env", "null0_ImageRotateCW")
export declare function ImageRotateCW(): void


// Rotate image counter-clockwise 90deg
@external("env", "null0_ImageRotateCCW")
export declare function ImageRotateCCW(): void


// Modify image color: tint
@external("env", "null0_ImageColorTint")
export declare function ImageColorTint(color: Color): void


// Modify image color: invert
@external("env", "null0_ImageColorInvert")
export declare function ImageColorInvert(): void


// Modify image color: grayscale
@external("env", "null0_ImageColorGrayscale")
export declare function ImageColorGrayscale(): void


// Modify image color: contrast (-100 to 100)
@external("env", "null0_ImageColorContrast")
export declare function ImageColorContrast(contrast: float): void


// Modify image color: brightness (-255 to 255)
@external("env", "null0_ImageColorBrightness")
export declare function ImageColorBrightness(brightness: i32): void


// Modify image color: replace color
@external("env", "null0_ImageColorReplace")
export declare function ImageColorReplace(color: Color, replace: Color): void


// Get image pixel color at (x, y) position
@external("env", "null0_GetImageColor")
export declare function GetImageColor(x: i32, y: i32): Color


// Clear image background with given color
@external("env", "null0_ImageClearBackground")
export declare function ImageClearBackground(color: Color): void


// Draw pixel within an image
@external("env", "null0_ImageDrawPixel")
export declare function ImageDrawPixel(posX: i32, posY: i32, color: Color): void


// Draw pixel within an image (Vector version)
@external("env", "null0_ImageDrawPixelV")
export declare function ImageDrawPixelV(position: Vector2, color: Color): void


// Draw line within an image
@external("env", "null0_ImageDrawLine")
export declare function ImageDrawLine(startPosX: i32, startPosY: i32, endPosX: i32, endPosY: i32, color: Color): void


// Draw line within an image (Vector version)
@external("env", "null0_ImageDrawLineV")
export declare function ImageDrawLineV(start: Vector2, end: Vector2, color: Color): void


// Draw circle within an image
@external("env", "null0_ImageDrawCircle")
export declare function ImageDrawCircle(centerX: i32, centerY: i32, radius: i32, color: Color): void


// Draw circle within an image (Vector version)
@external("env", "null0_ImageDrawCircleV")
export declare function ImageDrawCircleV(center: Vector2, radius: i32, color: Color): void


// Draw rectangle within an image
@external("env", "null0_ImageDrawRectangle")
export declare function ImageDrawRectangle(posX: i32, posY: i32, width: i32, height: i32, color: Color): void


// Draw rectangle within an image (Vector version)
@external("env", "null0_ImageDrawRectangleV")
export declare function ImageDrawRectangleV(position: Vector2, size: Vector2, color: Color): void


// Draw rectangle within an image
@external("env", "null0_ImageDrawRectangleRec")
export declare function ImageDrawRectangleRec(rec: Rectangle, color: Color): void


// Draw rectangle lines within an image
@external("env", "null0_ImageDrawRectangleLines")
export declare function ImageDrawRectangleLines(rec: Rectangle, thick: i32, color: Color): void


// Draw a source image within a destination image (tint applied to source)
@external("env", "null0_ImageDraw")
export declare function ImageDraw(src: u32, srcRec: Rectangle, dstRec: Rectangle, tint: Color): void


// Draw text (using default font) within an image (destination)
@external("env", "null0_ImageDrawText")
export declare function ImageDrawText(text: ArrayBuffer, posX: i32, posY: i32, fontSize: i32, color: Color): void


// Get color with alpha applied, alpha goes from 0.0f to 1.0f
@external("env", "null0_Fade")
export declare function Fade(alpha: float): Color


// Get hexadecimal value for a Color
@external("env", "null0_ColorToInt")
export declare function ColorToInt(): i32


// Get Color normalized as float [0..1]
@external("env", "null0_ColorNormalize")
export declare function ColorNormalize(): Vector4


// Get Color from normalized values [0..1]
@external("env", "null0_ColorFromNormalized")
export declare function ColorFromNormalized(): Color


// Get HSV values for a Color, hue [0..360], saturation/value [0..1]
@external("env", "null0_ColorToHSV")
export declare function ColorToHSV(): Vector3


// Get a Color from HSV values, hue [0..360], saturation/value [0..1]
@external("env", "null0_ColorFromHSV")
export declare function ColorFromHSV(saturation: float, value: float): Color


// Get color with alpha applied, alpha goes from 0.0f to 1.0f
@external("env", "null0_ColorAlpha")
export declare function ColorAlpha(alpha: float): Color


// Get src alpha-blended into dst color with tint
@external("env", "null0_ColorAlphaBlend")
export declare function ColorAlphaBlend(src: Color, tint: Color): Color


// Get Color structure from hexadecimal value
@external("env", "null0_GetColor")
export declare function GetColor(): Color


// Get pixel data size in bytes for certain format
@external("env", "null0_GetPixelDataSize")
export declare function GetPixelDataSize(height: i32, format: i32): i32


// IMPORT ALIASES

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
export declare function DrawText(text: ArrayBuffer, posX: i32, posY: i32, fontSize: i32, color: Color): void



