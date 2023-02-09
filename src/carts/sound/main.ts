// Example cart that plays some sounds

let tts:u8
let mod:u8
let wav:u8

// called when the cart is loaded
export function load(): void {
  log('Hello, from sound load().')
  tts = create_speech("Hello from null zero.")
  mod = create_mod("dune_-_zzzbla.xm")
  wav = create_wav("amen.wav")
  sound_play(tts)
  sound_play(mod)
  sound_play(wav)
  sound_loop(wav, true)
}

// called to update screen
export function update(dt: u64): void {
}

// called when the cart is unloaded
export function unload(): void {
  log('Ok, bye.')
}