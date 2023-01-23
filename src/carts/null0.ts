// null0 assemblyscript header, generated 2023-01-23T06:18:57.232Z

@unmanaged
export class Color {
  r: u8
  g: u8
  b: u8
  a: u8
}

@unmanaged
export class Vector3 {
  x: f32
  y: f32
  z: f32
}

@unmanaged
export class Vector4 {
  x: f32
  y: f32
  z: f32
  w: f32
}

@unmanaged
export class Rectangle {
  x: f32
  y: f32
  height: f32
  width: f32
}

@unmanaged
export class GlyphInfo {
  value: i32
  offsetX: i32
  offsetY: i32
  advanceX: i32
  image: Image
}

@unmanaged
export class Image {
  data: ArrayBuffer
  width: i32 
  height: i32 
  mipmaps: i32 
  format: i32 
}

@unmanaged
export class Texture {
  id: u32
  width: i32
  height: i32
  mipmaps: i32
  format: i32
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
declare function _LoadImage(fileName: ArrayBuffer): Image
export function LoadImage(fileName: string): Image {
  return _LoadImage(String.UTF8.encode(fileName, true))
}

// Load image sequence from file (frames appended to image.data)
@external("env", "null0_LoadImageAnim")
declare function _LoadImageAnim(fileName: ArrayBuffer, frames: i32[]): Image
export function LoadImageAnim(fileName: string, frames: i32[]): Image {
  return _LoadImageAnim(String.UTF8.encode(fileName, true), frames)
}

// Unload image from CPU memory (RAM)
@external("env", "null0_UnloadImage")
export declare function UnloadImage(image: Image): void

// Export image data to file, returns true on success
@external("env", "null0_ExportImage")
declare function _ExportImage(image: Image, fileName: ArrayBuffer): bool
export function ExportImage(image: Image, fileName: string): bool {
  return _ExportImage(image, String.UTF8.encode(fileName, true))
}

// Generate image: plain color
@external("env", "null0_GenImageColor")
export declare function GenImageColor(width: i32, height: i32, color: Color): Image

// Generate image: vertical gradient
@external("env", "null0_GenImageGradientV")
export declare function GenImageGradientV(width: i32, height: i32, top: Color, bottom: Color): Image

// Generate image: horizontal gradient
@external("env", "null0_GenImageGradientH")
export declare function GenImageGradientH(width: i32, height: i32, left: Color, right: Color): Image

// Generate image: radial gradient
@external("env", "null0_GenImageGradientRadial")
export declare function GenImageGradientRadial(width: i32, height: i32, density: float, inner: Color, outer: Color): Image

// Generate image: checked
@external("env", "null0_GenImageChecked")
export declare function GenImageChecked(width: i32, height: i32, checksX: i32, checksY: i32, col1: Color, col2: Color): Image

// Generate image: white noise
@external("env", "null0_GenImageWhiteNoise")
export declare function GenImageWhiteNoise(width: i32, height: i32, factor: float): Image

// Generate image: perlin noise
@external("env", "null0_GenImagePerlinNoise")
export declare function GenImagePerlinNoise(width: i32, height: i32, offsetX: i32, offsetY: i32, scale: float): Image

// Generate image: cellular algorithm, bigger tileSize means bigger cells
@external("env", "null0_GenImageCellular")
export declare function GenImageCellular(width: i32, height: i32, tileSize: i32): Image

// Generate image: grayscale image from text data
@external("env", "null0_GenImageText")
declare function _GenImageText(width: i32, height: i32, text: ArrayBuffer): Image
export function GenImageText(width: i32, height: i32, text: string): Image {
  return _GenImageText(width, height, String.UTF8.encode(text, true))
}

// Create an image duplicate (useful for transformations)
@external("env", "null0_ImageCopy")
export declare function ImageCopy(image: Image): Image

// Create an image from another image piece
@external("env", "null0_ImageFromImage")
export declare function ImageFromImage(image: Image, rec: Rectangle): Image

// Create an image from text (default font)
@external("env", "null0_ImageText")
declare function _ImageText(text: ArrayBuffer, fontSize: i32, color: Color): Image
export function ImageText(text: string, fontSize: i32, color: Color): Image {
  return _ImageText(String.UTF8.encode(text, true), fontSize, color)
}

// Create an image from text (custom sprite font)
@external("env", "null0_ImageTextEx")
declare function _ImageTextEx(font: Font, text: ArrayBuffer, fontSize: float, spacing: float, tint: Color): Image
export function ImageTextEx(font: Font, text: string, fontSize: float, spacing: float, tint: Color): Image {
  return _ImageTextEx(font, String.UTF8.encode(text, true), fontSize, spacing, tint)
}

// Convert image data to desired format
@external("env", "null0_ImageFormat")
export declare function ImageFormat(image: Image, newFormat: i32): void

// Convert image to POT (power-of-two)
@external("env", "null0_ImageToPOT")
export declare function ImageToPOT(image: Image, fill: Color): void

// Crop an image to a defined rectangle
@external("env", "null0_ImageCrop")
export declare function ImageCrop(image: Image, crop: Rectangle): void

// Crop image depending on alpha value
@external("env", "null0_ImageAlphaCrop")
export declare function ImageAlphaCrop(image: Image, threshold: float): void

// Clear alpha channel to desired color
@external("env", "null0_ImageAlphaClear")
export declare function ImageAlphaClear(image: Image, color: Color, threshold: float): void

// Apply alpha mask to image
@external("env", "null0_ImageAlphaMask")
export declare function ImageAlphaMask(image: Image, alphaMask: Image): void

// Premultiply alpha channel
@external("env", "null0_ImageAlphaPremultiply")
export declare function ImageAlphaPremultiply(image: Image): void

// Apply Gaussian blur using a box blur approximation
@external("env", "null0_ImageBlurGaussian")
export declare function ImageBlurGaussian(image: Image, blurSize: i32): void

// Resize image (Bicubic scaling algorithm)
@external("env", "null0_ImageResize")
export declare function ImageResize(image: Image, newWidth: i32, newHeight: i32): void

// Resize image (Nearest-Neighbor scaling algorithm)
@external("env", "null0_ImageResizeNN")
export declare function ImageResizeNN(image: Image, newWidth: i32, newHeight: i32): void

// Resize canvas and fill with color
@external("env", "null0_ImageResizeCanvas")
export declare function ImageResizeCanvas(image: Image, newWidth: i32, newHeight: i32, offsetX: i32, offsetY: i32, fill: Color): void

// Compute all mipmap levels for a provided image
@external("env", "null0_ImageMipmaps")
export declare function ImageMipmaps(image: Image): void

// Dither image data to 16bpp or lower (Floyd-Steinberg dithering)
@external("env", "null0_ImageDither")
export declare function ImageDither(image: Image, rBpp: i32, gBpp: i32, bBpp: i32, aBpp: i32): void

// Flip image vertically
@external("env", "null0_ImageFlipVertical")
export declare function ImageFlipVertical(image: Image): void

// Flip image horizontally
@external("env", "null0_ImageFlipHorizontal")
export declare function ImageFlipHorizontal(image: Image): void

// Rotate image clockwise 90deg
@external("env", "null0_ImageRotateCW")
export declare function ImageRotateCW(image: Image): void

// Rotate image counter-clockwise 90deg
@external("env", "null0_ImageRotateCCW")
export declare function ImageRotateCCW(image: Image): void

// Modify image color: tint
@external("env", "null0_ImageColorTint")
export declare function ImageColorTint(image: Image, color: Color): void

// Modify image color: invert
@external("env", "null0_ImageColorInvert")
export declare function ImageColorInvert(image: Image): void

// Modify image color: grayscale
@external("env", "null0_ImageColorGrayscale")
export declare function ImageColorGrayscale(image: Image): void

// Modify image color: contrast (-100 to 100)
@external("env", "null0_ImageColorContrast")
export declare function ImageColorContrast(image: Image, contrast: float): void

// Modify image color: brightness (-255 to 255)
@external("env", "null0_ImageColorBrightness")
export declare function ImageColorBrightness(image: Image, brightness: i32): void

// Modify image color: replace color
@external("env", "null0_ImageColorReplace")
export declare function ImageColorReplace(image: Image, color: Color, replace: Color): void

// Get image pixel color at (x, y) position
@external("env", "null0_GetImageColor")
export declare function GetImageColor(image: Image, x: i32, y: i32): Color

// Clear image background with given color
@external("env", "null0_ImageClearBackground")
export declare function ImageClearBackground(dst: Image, color: Color): void

// Draw pixel within an image
@external("env", "null0_ImageDrawPixel")
export declare function ImageDrawPixel(dst: Image, posX: i32, posY: i32, color: Color): void

// Draw pixel within an image (Vector version)
@external("env", "null0_ImageDrawPixelV")
export declare function ImageDrawPixelV(dst: Image, position: Vector2, color: Color): void

// Draw line within an image
@external("env", "null0_ImageDrawLine")
export declare function ImageDrawLine(dst: Image, startPosX: i32, startPosY: i32, endPosX: i32, endPosY: i32, color: Color): void

// Draw line within an image (Vector version)
@external("env", "null0_ImageDrawLineV")
export declare function ImageDrawLineV(dst: Image, start: Vector2, end: Vector2, color: Color): void

// Draw a filled circle within an image
@external("env", "null0_ImageDrawCircle")
export declare function ImageDrawCircle(dst: Image, centerX: i32, centerY: i32, radius: i32, color: Color): void

// Draw a filled circle within an image (Vector version)
@external("env", "null0_ImageDrawCircleV")
export declare function ImageDrawCircleV(dst: Image, center: Vector2, radius: i32, color: Color): void

// Draw circle outline within an image
@external("env", "null0_ImageDrawCircleLines")
export declare function ImageDrawCircleLines(dst: Image, centerX: i32, centerY: i32, radius: i32, color: Color): void

// Draw circle outline within an image (Vector version)
@external("env", "null0_ImageDrawCircleLinesV")
export declare function ImageDrawCircleLinesV(dst: Image, center: Vector2, radius: i32, color: Color): void

// Draw rectangle within an image
@external("env", "null0_ImageDrawRectangle")
export declare function ImageDrawRectangle(dst: Image, posX: i32, posY: i32, width: i32, height: i32, color: Color): void

// Draw rectangle within an image (Vector version)
@external("env", "null0_ImageDrawRectangleV")
export declare function ImageDrawRectangleV(dst: Image, position: Vector2, size: Vector2, color: Color): void

// Draw rectangle within an image
@external("env", "null0_ImageDrawRectangleRec")
export declare function ImageDrawRectangleRec(dst: Image, rec: Rectangle, color: Color): void

// Draw rectangle lines within an image
@external("env", "null0_ImageDrawRectangleLines")
export declare function ImageDrawRectangleLines(dst: Image, rec: Rectangle, thick: i32, color: Color): void

// Draw a source image within a destination image (tint applied to source)
@external("env", "null0_ImageDraw")
export declare function ImageDraw(dst: Image, src: Image, srcRec: Rectangle, dstRec: Rectangle, tint: Color): void

// Draw text (using default font) within an image (destination)
@external("env", "null0_ImageDrawText")
declare function _ImageDrawText(dst: Image, font: Font, text: ArrayBuffer, posX: i32, posY: i32, fontSize: i32, color: Color): void
export function ImageDrawText(dst: Image, font: Font, text: string, posX: i32, posY: i32, fontSize: i32, color: Color): void {
  return _ImageDrawText(dst, font, String.UTF8.encode(text, true), posX, posY, fontSize, color)
}

// Draw text (custom sprite font) within an image (destination)
@external("env", "null0_ImageDrawTextEx")
declare function _ImageDrawTextEx(dst: Image, font: Font, text: ArrayBuffer, position: Vector2, fontSize: float, spacing: float, tint: Color): void
export function ImageDrawTextEx(dst: Image, font: Font, text: string, position: Vector2, fontSize: float, spacing: float, tint: Color): void {
  return _ImageDrawTextEx(dst, font, String.UTF8.encode(text, true), position, fontSize, spacing, tint)
}

// Unload texture from GPU memory (VRAM)
@external("env", "null0_UnloadTexture")
export declare function UnloadTexture(texture: Texture2D): void

// Unload render texture from GPU memory (VRAM)
@external("env", "null0_UnloadRenderTexture")
export declare function UnloadRenderTexture(target: RenderTexture2D): void

// Update GPU texture with new data
@external("env", "null0_UpdateTexture")
export declare function UpdateTexture(texture: Texture2D, pixels: externref): void

// Update GPU texture rectangle with new data
@external("env", "null0_UpdateTextureRec")
export declare function UpdateTextureRec(texture: Texture2D, rec: Rectangle, pixels: externref): void

// Generate GPU mipmaps for a texture
@external("env", "null0_GenTextureMipmaps")
export declare function GenTextureMipmaps(texture: Texture): void

// Set texture scaling filter mode
@external("env", "null0_SetTextureFilter")
export declare function SetTextureFilter(texture: Texture2D, filter: i32): void

// Set texture wrapping mode
@external("env", "null0_SetTextureWrap")
export declare function SetTextureWrap(texture: Texture2D, wrap: i32): void

// Draw a Texture2D
@external("env", "null0_DrawTexture")
export declare function DrawTexture(texture: Texture2D, posX: i32, posY: i32, tint: Color): void

// Draw a Texture2D with position defined as Vector2
@external("env", "null0_DrawTextureV")
export declare function DrawTextureV(texture: Texture2D, position: Vector2, tint: Color): void

// Draw a Texture2D with extended parameters
@external("env", "null0_DrawTextureEx")
export declare function DrawTextureEx(texture: Texture2D, position: Vector2, rotation: float, scale: float, tint: Color): void

// Draw a part of a texture defined by a rectangle
@external("env", "null0_DrawTextureRec")
export declare function DrawTextureRec(texture: Texture2D, source: Rectangle, position: Vector2, tint: Color): void

// Draw a part of a texture defined by a rectangle with 'pro' parameters
@external("env", "null0_DrawTexturePro")
export declare function DrawTexturePro(texture: Texture2D, source: Rectangle, dest: Rectangle, origin: Vector2, rotation: float, tint: Color): void

// Draws a texture (or part of it) that stretches or shrinks nicely
@external("env", "null0_DrawTextureNPatch")
export declare function DrawTextureNPatch(texture: Texture2D, nPatchInfo: NPatchInfo, dest: Rectangle, origin: Vector2, rotation: float, tint: Color): void

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

// Generate image font atlas using chars info
@external("env", "null0_GenImageFontAtlas")
export declare function GenImageFontAtlas(chars: GlyphInfo, recs: Rectangle[], glyphCount: i32, fontSize: i32, padding: i32, packMethod: i32): Image

// Unload font chars info data (RAM)
@external("env", "null0_UnloadFontData")
export declare function UnloadFontData(chars: GlyphInfo, glyphCount: i32): void

// Unload font from GPU memory (VRAM)
@external("env", "null0_UnloadFont")
export declare function UnloadFont(font: Font): void

// Draw current FPS
@external("env", "null0_DrawFPS")
export declare function DrawFPS(posX: i32, posY: i32): void

// Draw text (using default font)
@external("env", "null0_DrawText")
declare function _DrawText(text: ArrayBuffer, posX: i32, posY: i32, fontSize: i32, color: Color): void
export function DrawText(text: string, posX: i32, posY: i32, fontSize: i32, color: Color): void {
  return _DrawText(String.UTF8.encode(text, true), posX, posY, fontSize, color)
}

// Draw text using font and additional parameters
@external("env", "null0_DrawTextEx")
declare function _DrawTextEx(font: Font, text: ArrayBuffer, position: Vector2, fontSize: float, spacing: float, tint: Color): void
export function DrawTextEx(font: Font, text: string, position: Vector2, fontSize: float, spacing: float, tint: Color): void {
  return _DrawTextEx(font, String.UTF8.encode(text, true), position, fontSize, spacing, tint)
}

// Draw text using Font and pro parameters (rotation)
@external("env", "null0_DrawTextPro")
declare function _DrawTextPro(font: Font, text: ArrayBuffer, position: Vector2, origin: Vector2, rotation: float, fontSize: float, spacing: float, tint: Color): void
export function DrawTextPro(font: Font, text: string, position: Vector2, origin: Vector2, rotation: float, fontSize: float, spacing: float, tint: Color): void {
  return _DrawTextPro(font, String.UTF8.encode(text, true), position, origin, rotation, fontSize, spacing, tint)
}

// Draw one character (codepoint)
@external("env", "null0_DrawTextCodepoint")
export declare function DrawTextCodepoint(font: Font, codepoint: i32, position: Vector2, fontSize: float, tint: Color): void

// Draw multiple character (codepoint)
@external("env", "null0_DrawTextCodepoints")
export declare function DrawTextCodepoints(font: Font, codepoints: i32[], count: i32, position: Vector2, fontSize: float, spacing: float, tint: Color): void

// Get glyph index position in font for a codepoint (unicode character), fallback to '?' if not found
@external("env", "null0_GetGlyphIndex")
export declare function GetGlyphIndex(font: Font, codepoint: i32): i32

// Load all codepoints from a UTF-8 text string, codepoints count returned by parameter
@external("env", "null0_LoadCodepoints")
declare function _LoadCodepoints(text: ArrayBuffer, count: i32[]): i32
export function LoadCodepoints(text: string, count: i32[]): i32 {
  return _LoadCodepoints(String.UTF8.encode(text, true), count)
}

// Unload codepoints data from memory
@external("env", "null0_UnloadCodepoints")
export declare function UnloadCodepoints(codepoints: i32[]): void

// Get total number of codepoints in a UTF-8 encoded string
@external("env", "null0_GetCodepointCount")
declare function _GetCodepointCount(text: ArrayBuffer): i32
export function GetCodepointCount(text: string): i32 {
  return _GetCodepointCount(String.UTF8.encode(text, true))
}

// Get next codepoint in a UTF-8 encoded string, 0x3f('?') is returned on failure
@external("env", "null0_GetCodepoint")
declare function _GetCodepoint(text: ArrayBuffer, codepointSize: i32[]): i32
export function GetCodepoint(text: string, codepointSize: i32[]): i32 {
  return _GetCodepoint(String.UTF8.encode(text, true), codepointSize)
}

// Get next codepoint in a UTF-8 encoded string, 0x3f('?') is returned on failure
@external("env", "null0_GetCodepointNext")
declare function _GetCodepointNext(text: ArrayBuffer, codepointSize: i32[]): i32
export function GetCodepointNext(text: string, codepointSize: i32[]): i32 {
  return _GetCodepointNext(String.UTF8.encode(text, true), codepointSize)
}

// Get previous codepoint in a UTF-8 encoded string, 0x3f('?') is returned on failure
@external("env", "null0_GetCodepointPrevious")
declare function _GetCodepointPrevious(text: ArrayBuffer, codepointSize: i32[]): i32
export function GetCodepointPrevious(text: string, codepointSize: i32[]): i32 {
  return _GetCodepointPrevious(String.UTF8.encode(text, true), codepointSize)
}

// Copy one string to another, returns bytes copied
@external("env", "null0_TextCopy")
declare function _TextCopy(dst: ArrayBuffer, src: ArrayBuffer): i32
export function TextCopy(dst: ArrayBuffer, src: string): i32 {
  return _TextCopy(dst, String.UTF8.encode(src, true))
}

// Check if two text string are equal
@external("env", "null0_TextIsEqual")
declare function _TextIsEqual(text1: ArrayBuffer, text2: ArrayBuffer): bool
export function TextIsEqual(text1: string, text2: string): bool {
  return _TextIsEqual(String.UTF8.encode(text1, true), String.UTF8.encode(text2, true))
}

// Append text at specific position and move cursor!
@external("env", "null0_TextAppend")
declare function _TextAppend(text: ArrayBuffer, append: ArrayBuffer, position: i32[]): void
export function TextAppend(text: ArrayBuffer, append: string, position: i32[]): void {
  return _TextAppend(text, String.UTF8.encode(append, true), position)
}

// Find first text occurrence within a string
@external("env", "null0_TextFindIndex")
declare function _TextFindIndex(text: ArrayBuffer, find: ArrayBuffer): i32
export function TextFindIndex(text: string, find: string): i32 {
  return _TextFindIndex(String.UTF8.encode(text, true), String.UTF8.encode(find, true))
}

// Get integer value from text (negative values not supported)
@external("env", "null0_TextToInteger")
declare function _TextToInteger(text: ArrayBuffer): i32
export function TextToInteger(text: string): i32 {
  return _TextToInteger(String.UTF8.encode(text, true))
}

// Log a string
@external("env", "null0_log")
declare function _log(text: ArrayBuffer): void
export function log(text: string): void {
  return _log(String.UTF8.encode(text, true))
}

// Fatal error - call this from your code on a fatal runtime error, similar to assemblyscript's abort(), but it's utf8
@external("env", "null0_fatal")
declare function _fatal(message: ArrayBuffer, filename: ArrayBuffer, lineNumber: i32, columnNumber: i32): void
export function fatal(message: string, filename: string, lineNumber: i32, columnNumber: i32): void {
  return _fatal(String.UTF8.encode(message, true), String.UTF8.encode(filename, true), lineNumber, columnNumber)
}

// Read a text-file from virtual filesystem
@external("env", "null0_ReadText")
declare function _ReadText(fileName: ArrayBuffer): ArrayBuffer
export function ReadText(fileName: string): string {
  return String.UTF8.decode(_ReadText(String.UTF8.encode(fileName, true)), true)
}

// Check if the file exists
@external("env", "null0_FileExists")
declare function _FileExists(fileName: ArrayBuffer): bool
export function FileExists(fileName: string): bool {
  return _FileExists(String.UTF8.encode(fileName, true))
}

// Perform a GET request on URL
@external("env", "null0_GetUrl")
declare function _GetUrl(url: ArrayBuffer): ArrayBuffer
export function GetUrl(url: string): string {
  return String.UTF8.decode(_GetUrl(String.UTF8.encode(url, true)), true)
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

// Draw a filled circle on the screen
@external("env", "null0_DrawCircle")
export declare function DrawCircle(centerX: i32, centerY: i32, radius: i32, color: Color): void

// Draw a filled circle on the screen (Vector version)
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
export declare function Draw(src: Image, srcRec: Rectangle, dstRec: Rectangle, tint: Color): void


