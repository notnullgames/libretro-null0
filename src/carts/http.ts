// Example cart that just logs an http request

import { http, log } from "./null0"

// called when the cart is loaded
export function init(): void {
  const r:string = http.request("https://example.com")
  log(r)
}