// Example cart that just logs an http request

import { http, log } from "./null0"

// TODO: figure out how to automatically inject these

// malloc for ArrayBuffer
export function wmalloc(size: usize): usize {
  return __pin(__new(size, 1))
}

// free an ArrayBuffer
export function wfree(ptr: usize): void {
  __unpin(ptr)
}

// called when the cart is loaded
export function init(): void {
  const r:string = http.request("https://example.com")
  log(r)
}