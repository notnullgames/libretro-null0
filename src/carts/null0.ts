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
export declare function ClearBackground(color: u32): void

@external("env", "null0_DrawCircle")
export declare function DrawCircle(centerX: u32, centerY: u32, radius: f32, color: u32): void

@external("env", "null0_DrawLine")
export declare function DrawLine(startPosX: u32, startPosY: u32, endPosX: u32, endPosY: u32, color: u32): void

@external("env", "null0_DrawRectangle")
export declare function DrawRectangle(posX: u32, posY: u32, width: u32, height: u32, color: u32): void

export function rgb(r: u8, g: u8, b: u8): u32 {
  return rgba(r, g, b, 0xFF)
}

export function rgba(r: u8, g: u8, b: u8, a: u8): u32 {
  return (r << 24) + (g << 16) + (b << 8) + (a)
}

export const LIGHTGRAY = rgb( 200, 200, 200 )
export const GRAY      = rgb( 130, 130, 130 )
export const DARKGRAY  = rgb( 80,  80,  80  )
export const LIGHTGREY = rgb( 200, 200, 200 )
export const GREY      = rgb( 130, 130, 130 )
export const DARKGREY  = rgb( 80,  80,  80  )
export const YELLOW    = rgb( 253, 249, 0   )
export const GOLD      = rgb( 255, 203, 0   )
export const ORANGE    = rgb( 255, 161, 0   )
export const PINK      = rgb( 255, 109, 194 )
export const RED       = rgb( 230, 41,  55  )
export const MAROON    = rgb( 190, 33,  55  )
export const GREEN     = rgb( 0,   228, 48  )
export const LIME      = rgb( 0,   158, 47  )
export const DARKGREEN = rgb( 0,   117, 44  )
export const SKYBLUE   = rgb( 102, 191, 255 )
export const BLUE      = rgb( 0,   121, 241 )
export const DARKBLUE  = rgb( 0,   82,  172 )
export const PURPLE    = rgb( 200, 122, 255 )
export const VIOLET    = rgb( 135, 60,  190 )
export const DARKPURPL = rgb( 112, 31,  126 )
export const BEIGE     = rgb( 211, 176, 131 )
export const BROWN     = rgb( 127, 106, 79  )
export const DARKBROWN = rgb( 76,  63,  47  )
export const WHITE     = rgb( 255, 255, 255 )
export const BLACK     = rgb( 0,   0,   0   )
export const BLANK     = rgba(  0, 0, 0, 0  )
export const MAGENTA   = rgb( 255, 0,   255 )
export const RAYWHITE  = rgb( 245, 245, 245 )
