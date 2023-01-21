// this will generate the C wasm3 host

import funcs from './rimage.json' assert { type: 'json' }

let out = `
// null0 host, generated ${(new Date()).toISOString()}
`

console.log(out)