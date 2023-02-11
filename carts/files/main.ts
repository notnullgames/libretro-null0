// called when the cart is loaded
export function load(): void {
  log('Hello, from files load().')

  log("test.txt: " + file_read('test.txt'))

  if (file_exists("doesnotexist.txt")) {
    log('exists')
  }else{
    log('does not exist')
  }

  if (file_exists("counter.txt")) {
    const data = new DataView(file_read_binary("counter.txt"))
    const n = data.getInt32(0)
    data.setInt32(0, n + 1)
    file_write_binary("counter.txt", data.buffer)
  } else {
    const data = new DataView(new ArrayBuffer(4))
    data.setInt32(0, 0)
    file_write_binary("counter.txt", data.buffer)
  }
}

// called per-frame to update screen
export function update(dt: u64): void {
}

// called when the cart is unloaded
export function unload(): void {
  log("Ok, bye.");
}