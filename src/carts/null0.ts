// This is the header for using null0 in assemblyscript

// allocate a buffer, return a pounter
export function malloc(size: u32): u32 {
  log(`malloc ${size}`)
  return 0
}

// log a string
@external("env", "null0_log")
declare function null0_log(text: ArrayBuffer): void
export function log(text: string): void {
  null0_log(String.UTF8.encode(text, true))
}

// make a HTTP/HTTPS GET request
@external("env", "null0_http_request_get")
declare function null0_http_get(url: ArrayBuffer): ArrayBuffer

// XXX: this is probly the wrong way to export this shape of object, but it works
class Http {
  request(url: string): string {
    const out:ArrayBuffer = null0_http_get(String.UTF8.encode(url, true))
    return String.UTF8.decode(out, true)
  }
}

export const http = new Http()