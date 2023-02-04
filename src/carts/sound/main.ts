// Example cart that plays some sounds

let blip:u8 
let amen:u8 
let exp:u8

// called when the cart is loaded
export function load(): void {
  log('Hello, from sound load().')
  blip = load_sfx(SfxPreset.BLIP, u8(Math.random()))
  exp = load_sfx(SfxPreset.EXPLOSION, u8(Math.random()))
  amen = load_sound('/amen.wav')
  play_sound(amen, 0.2, true)
}

// called to update screen
export function update(dt: u64): void {
  const beat = (dt/100000) % 24

  if (dt < 10000000) {
    if (beat == 16) {
      say("this is null zero.", 0.5)
    }
  } else if (dt < 20000000) {
    if (beat == 0) {
      play_sound(blip, 0.5, false)
    }
    if (beat == 12) {
      play_sound(exp, 0.5, false)
    }
  } else if (dt < 30000000) {
    stop_sound(amen)
  }
}

// called when the cart is unloaded
export function unload(): void {
  log('Ok, bye.')
}