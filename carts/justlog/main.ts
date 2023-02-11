// Example cart that just logs

// called when the cart is loaded
export function load(): void {
  log('Hello, from justlog load().')
}

// called to update screen
export function update(dt: u64): void {
}

// called when the cart is unloaded
export function unload(): void {
  log('Ok, bye.')
}