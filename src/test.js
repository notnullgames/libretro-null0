// this is a simple JS-based WASM host to test thigns outside of C host

import assert from 'assert'
import { readFile } from 'fs/promises'
import child from 'child_process'

if (process.argv.length !== 3) {
  console.error('Usage: test WASMFILE.wasm')
  process.exit(1)
}

// ArrayBuffer -> String
export function ab2str (buf) {
  return String.fromCharCode.apply(null, new Uint8Array(buf).slice(0, -1))
}

// String -> ArrayBuffer
export function str2ab (str) {
  const buf = new ArrayBuffer(str.length + 1)
  buf[str.length] = 0
  const bufView = new Uint8Array(buf)
  for (let i = 0, strLen = str.length; i < strLen; i++) {
    bufView[i] = str.charCodeAt(i)
  }
  return buf
}

const wasmBuffer = await readFile(process.argv[2])
const wasmModule = await WebAssembly.instantiate(wasmBuffer, {
  env: {
    null0_log: m => {
      console.log(ab2str(__liftBuffer(m)))
    },

    // not dealing with WTF16 strings here, this is just a stub to stop AS from complaining
    abort: (message, fileName, lineNumber, columnNumber) => {
      console.log('abort', lineNumber, columnNumber)
    },

    // synchronous GET request
    null0_http_request_get: (u) => {
      const url = ab2str(__liftBuffer(u))
      const buffer = child.execSync(`curl -s "${url}"`)
      return __lowerBuffer(buffer)
    }
  }
})

// these came from the generated wrapper
// In C, these are mostly handled automatically by the macros, but I will probly need to work out __new

function __liftBuffer (pointer) {
  if (!pointer) return null
  return wasmModule.instance.exports.memory.buffer.slice(pointer, pointer + new Uint32Array(wasmModule.instance.exports.memory.buffer)[pointer - 4 >>> 2])
}

function __lowerBuffer (value) {
  if (value == null) return 0
  const pointer = wasmModule.instance.exports.__new(value.byteLength, 1) >>> 0
  new Uint8Array(wasmModule.instance.exports.memory.buffer).set(new Uint8Array(value), pointer)
  return pointer
}

// run entry-point
wasmModule.instance.exports.init()
