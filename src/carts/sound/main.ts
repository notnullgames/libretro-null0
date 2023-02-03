// Example cart that plays some sounds

let casio:u8 
let amen:u8 

// called when the cart is loaded
export function load(): void {
  log('Hello, from sound load().')
  casio = load_sound('/Casio-MT-45-16-Beat.wav')
  amen = load_sound('/amen.wav')
  play_sound(casio, 1.0, 1)
  play_sound(amen, 1.0, 1)
}

// called to update screen
export function update(dt: f64): void {
}

// called when the cart is unloaded
export function unload(): void {
  log('Ok, bye.')
}