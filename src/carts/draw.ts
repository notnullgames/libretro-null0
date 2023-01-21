// called when the cart is loaded
export function load(): void {
  log('Hello! It works.')
}

// called per-frame to update screen
export function update(dt: i32): void {
  ClearBackground(SKYBLUE)
  DrawCircle(200 , 150, 30.0 + (dt as f32/10000.0) % 20.0, RED)
  DrawRectangle(400, 200, 100, 140, GREEN)
  DrawLine(50, 300, 550, 380, ORANGE)
}
