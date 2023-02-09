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

// Load an image
@external("env", "null0_load_image")
declare function _load_image(filename: ArrayBuffer): u8
export function load_image(filename: string): u8 {
  return _load_image(String.UTF8.encode(filename, true))
}

// Draw an rectangle on an image
@external("env", "null0_draw_rectangle")
export declare function draw_rectangle_on_image(destination: u8, x:i32, y:i32, width:i32, height:i32, color:Color): void
export function draw_rectangle(x:i32, y:i32, width:i32, height:i32, color:Color): void {
  return draw_rectangle_on_image(0, x, y, width, height, color)
}

// Draw a circle on an image
@external("env", "null0_draw_circle")
export declare function draw_circle_on_image(destination: u8, centerX:i32, centerY:i32, radius:i32, color:Color): void
export function draw_circle(centerX:i32, centerY:i32, radius:i32, color:Color): void {
  return draw_circle_on_image(0, centerX, centerY, radius, color)
}

// Draw a pixel on an image
@external("env", "null0_draw_pixel")
export declare function draw_pixel_on_image(destination: u8, x:i32, y:i32, color:Color): void
export function draw_pixel(x:i32, y:i32, color:Color): void {
  return draw_pixel_on_image(0, x, y, color)
}

// Draw an image on the screen
@external("env", "null0_draw_image")
export declare function draw_image_on_image(destination: u8, source: u8, x:i32, y:i32): void
export function draw_image(source: u8, x:i32, y:i32): void {
  return draw_image_on_image(0, source, x, y)
}

// Clear an image with a color
@external("env", "null0_clear_screen")
export declare function clear_screen_on_image(destination: u8, color:Color): void
export function clear_screen(color:Color): void {
  return clear_screen_on_image(0, color)
}

// Generate an image with a single color
@external("env", "null0_gen_image_color")
export declare function gen_image_color(width:u8, height: u8, color:Color): u8

// Read a file
@external("env", "null0_file_read")
declare function _file_read(filename: ArrayBuffer): ArrayBuffer
export function file_read(filename: string): string {
  return String.UTF8.decode(_file_read(String.UTF8.encode(filename, true)))
}
export function file_read_binary(filename: string): ArrayBuffer {
  return _file_read(String.UTF8.encode(filename, true))
}

// write a file
@external("env", "null0_file_write")
declare function _file_write(filename: ArrayBuffer, content: ArrayBuffer, length: u32): void
export function file_write(filename: string, content: string): void {
  return _file_write(String.UTF8.encode(filename, true), String.UTF8.encode(content, true), content.length)
}
export function file_write_binary(filename: string, content: ArrayBuffer): void {
  return _file_write(String.UTF8.encode(filename, true), content, content.byteLength)
}

// Read a file
@external("env", "null0_file_exists")
declare function _file_exists(filename: ArrayBuffer): i32
export function file_exists(filename: string): boolean {
  return _file_exists(String.UTF8.encode(filename, true)) === 1
}

@external("env", "null0_sound_loop")
declare function _sound_loop(sound:u8, looing:i32): void
export function sound_loop(sound: u8, looping: boolean): void {
  return _sound_loop(sound, looping ? 1 : 0)
}

// Create a TTS speaker
@external("env", "null0_create_speech")
declare function _create_speech(text: ArrayBuffer): u8
export function create_speech(text: string): u8 {
  return _create_speech(String.UTF8.encode(text, true))
}


// Create a mod/xm/midi/etc player
@external("env", "null0_create_mod")
declare function _create_mod(filename: ArrayBuffer): u8
export function create_mod(filename: string): u8 {
  return _create_mod(String.UTF8.encode(filename, true))
}

// Create a wav player
@external("env", "null0_create_wav")
declare function _create_wav(filename: ArrayBuffer): u8
export function create_wav(filename: string): u8 {
  return _create_wav(String.UTF8.encode(filename, true))
}


// Set the text of a TTS speaker
@external("env", "null0_speech_settext")
declare function _speech_text(id: u8, text: ArrayBuffer): void
export function speech_text(id:u8, text: string): void {
  _speech_text(id, String.UTF8.encode(text, true))
}

// Play a sound
@external("env", "null0_sound_play")
export declare function sound_play(id: u8): void

