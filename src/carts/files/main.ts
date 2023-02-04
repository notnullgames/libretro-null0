// called when the cart is loaded
export function load(): void {
  log('Hello, from files load().')
  log("Text: " + file_read('test.txt'))
}

// called per-frame to update screen
export function update(dt: u64): void {
}

// called when the cart is unloaded
export function unload(): void {
  log("Ok, bye.");
}