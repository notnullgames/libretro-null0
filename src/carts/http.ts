// Example cart that just logs an http request

// called when the cart is loaded
export function load(): void {
  const r = GetUrl("https://example.com")
  log(r)
}