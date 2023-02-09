// Example cart that plays some sounds

let tts:u8

// called when the cart is loaded
export function load(): void {
  log('Hello, from sound load().')
  tts = create_speech("Hello from null zero.")
  sound_play(tts)
}

// called to update screen
export function update(dt: u64): void {
}

// called when the cart is unloaded
export function unload(): void {
  log('Ok, bye.')
}