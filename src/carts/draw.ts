// called when the cart is loaded
export function load(): void {
  log('Hello! It works.')
}

// called per-frame to update screen
export function update(dt: i32): void {
  ClearBackground(SKYBLUE)
  DrawCircle(200 , 150, 30, RED)
  DrawRectangle(400, 200, 100, 140, {r: GREEN.r, g: GREEN.g, b: GREEN.b, a: 0x55})
  DrawRectangle(500, 200, 100, 140, {r: BLUE.r, g: BLUE.g, b: BLUE.b, a: 0x55})
  DrawLine(50, 300, 550, 380, ORANGE)
}
