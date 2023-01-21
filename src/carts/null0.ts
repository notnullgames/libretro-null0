// This is the header for using null0 in assemblyscript

// log a string
@external("env", "null0_log")
declare function null0_log(text: ArrayBuffer): void
export function log(text: string): void {
  null0_log(String.UTF8.encode(text, true))
}

// make a HTTP/HTTPS GET request
@external("env", "null0_http_request_get")
declare function null0_http_request_get(url: ArrayBuffer): ArrayBuffer

export function http_get(url: string): string {
  const out:ArrayBuffer = null0_http_request_get(String.UTF8.encode(url, true))
  return String.UTF8.decode(out, true)
}

@external("env", "null0_ClearBackground")
export declare function ClearBackground(color: i32): void

@external("env", "null0_DrawCircle")
export declare function DrawCircle(centerX: i32, centerY: i32, radius: f32, color: i32): void

@external("env", "null0_DrawLine")
export declare function DrawLine(startPosX: i32, startPosY: i32, endPosX: i32, endPosY: i32, color: i32): void

@external("env", "null0_DrawRectangle")
export declare function DrawRectangle(posX: i32, posY: i32, width: i32, height: i32, color: i32): void

export const LIGHTGRAY = 0xC8C8C8FF
export const LIGHTGREY = LIGHTGRAY
export const GRAY      = 0x828282FF
export const GREY      = GRAY
export const DARKGRAY  = 0x505050FF
export const DARKGREY  = DARKGRAY
export const WHITE     = 0xFFFFFFFF
export const BLACK     = 0x000000FF
export const BLANK     = 0x00000000
export const BEIGE = 0xd3b083FF
export const BLUE = 0x0079f1FF
export const BROWN = 0x7f6a4fFF
export const DARKBLUE = 0x0052acFF
export const DARKBROWN = 0x4c3f2fFF
export const DARKGREEN = 0x00752cFF
export const GREEN     = 0x00E430FF
export const DARKPURPL = 0x701f7eFF
export const GOLD = 0xffcb00FF
export const LIME = 0x009e2fFF
export const MAGENTA = 0xff00ffFF
export const MAROON = 0xbe2137FF
export const ORANGE = 0xffa100FF
export const PINK = 0xff6dc2FF
export const PURPLE = 0xc87affFF
export const RAYWHITE = 0xf5f5f5FF
export const RED = 0xe62937FF
export const SKYBLUE = 0x66bfffFF
export const VIOLET = 0x873cbeFF
export const YELLOW = 0xfdf900FF
