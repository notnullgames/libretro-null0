// Example cart that just logs an http request

// called when the cart is loaded
export function load(): void {
  const r:string = http_get("https://example.com")
  log(r)
}