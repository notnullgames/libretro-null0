// called when the cart is loaded
export function load(): void {
  log('Hello, from random load().')
}

// called per-frame to update screen
export function update(dt: i32): void {
  log(Math.random().toString())
}

// called when the cart is unloaded
export function unload(): void {
  log('Ok, bye.')
}