// called when the cart is loaded
export function load(): void {
  log('Hello, from draw load().')
}

// called per-frame to update screen
export function update(dt: i32): void {
  ClearBackground(SKYBLUE)
  DrawCircle(200 , 150, 30, RED)
  DrawRectangle(10, 20, 100, 140, GREEN)
  DrawLine(0, 30, 200, 200, ORANGE)
}
