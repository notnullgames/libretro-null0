// This is the auto-imported header for null0 WASM games, written in typescript

@unmanaged
export class Color {
  b: u8
  g: u8
  r: u8
  a: u8
}

export const LIGHTGRAY :Color = { r: 200, g: 200, b: 200, a: 255 }
export const GRAY      :Color = { r: 130, g: 130, b: 130, a: 255 }
export const DARKGRAY  :Color = { r: 80,  g:  80, b:  80, a: 255 }
export const YELLOW    :Color = { r: 253, g: 249, b: 0,   a: 255 }
export const GOLD      :Color = { r: 255, g: 203, b: 0,   a: 255 }
export const ORANGE    :Color = { r: 255, g: 161, b: 0,   a: 255 }
export const PINK      :Color = { r: 255, g: 109, b: 194, a: 255 }
export const RED       :Color = { r: 230, g:  41, b:  55, a: 255 }
export const MAROON    :Color = { r: 190, g:  33, b:  55, a: 255 }
export const GREEN     :Color = { r: 0,   g: 228, b: 48,  a: 255 }
export const LIME      :Color = { r: 0,   g: 158, b: 47,  a: 255 }
export const DARKGREEN :Color = { r: 0,   g: 117, b: 44,  a: 255 }
export const SKYBLUE   :Color = { r: 102, g: 191, b: 255, a: 255 }
export const BLUE      :Color = { r: 0,   g: 121, b: 241, a: 255 }
export const DARKBLUE  :Color = { r: 0,   g:  82, b: 172, a: 255 }
export const PURPLE    :Color = { r: 200, g: 122, b: 255, a: 255 }
export const VIOLET    :Color = { r: 135, g:  60, b: 190, a: 255 }
export const DARKPURPLE:Color = { r: 112, g:  31, b: 126, a: 255 }
export const BEIGE     :Color = { r: 211, g: 176, b: 131, a: 255 }
export const BROWN     :Color = { r: 127, g: 106, b:  79, a: 255 }
export const DARKBROWN :Color = { r: 76,  g:  63, b:  47, a: 255 }
export const WHITE     :Color = { r: 255, g: 255, b: 255, a: 255 }
export const BLACK     :Color = { r: 0,   g:   0, b:   0, a: 255 }
export const BLANK     :Color = { r: 0,   g:   0, b:   0, a: 0   }
export const MAGENTA   :Color = { r: 255, g:   0, b: 255, a: 255 }
export const RAYWHITE  :Color = { r: 245, g: 245, b: 245, a: 255 }

export enum SpeechType {
  SAW,
  TRIANGLE,
  SIN,
  SQUARE,
  PULSE,
  NOISE,
  WARBLE
}

export enum SfxPreset {
  COIN,
  LASER,
  EXPLOSION,
  POWERUP,
  HURT,
  JUMP,
  BLIP
}

// Log a string
@external("null0", "log")
declare function _log(text: ArrayBuffer): void
export function log(text: string): void {
  return _log(String.UTF8.encode(text, true))
}

// Fatal error - call this from your code on a fatal runtime error, similar to assemblyscript's abort(), but it's utf8
@external("null0", "fatal")
declare function _fatal(message: ArrayBuffer, filename: ArrayBuffer, lineNumber: i32, columnNumber: i32): void
export function fatal(message: string, filename: string, lineNumber: i32, columnNumber: i32): void {
  return _fatal(String.UTF8.encode(message, true), String.UTF8.encode(filename, true), lineNumber, columnNumber)
}

