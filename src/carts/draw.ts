// called when the cart is loaded
export function load(): void {
  log('Hello! It works.')
}

// called per-frame to update screen
export function update(): void {
  log('update')
  ClearBackground(SKYBLUE)
  DrawCircle(200, 150, 30, RED)
  DrawRectangle(400, 200, 100, 140, GREEN)
  DrawLine(50, 300, 550, 380, ORANGE)
}
