// called when the cart is loaded
export function load(): void {
  log('Hello, from files load().')
  log("Text: " + ReadText('test.txt'))
}

// called per-frame to update screen
export function update(dt: i32): void {
  ClearBackground(BLACK)
}