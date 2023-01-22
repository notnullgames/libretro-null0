// this will generate the assemblyscript header

import funcs from './rimage.json' assert { type: 'json' }

// use as filter to not output some functions
const noFilter = ({name}) => ![
  'GetPixelColor',
  'SetPixelColor',
  'LoadImageRaw',
  'LoadImageFromMemory',
  'ExportImageAsCode',
  'UnloadImageColors',
  'UnloadImagePalette',
  'LoadImageColors',
  'LoadImagePalette'
].includes(name)

// map params to as-types
function getParams(params = []) {
  return (params.length === 1 && params[1] === 'void') ? '' : params.map(p => `${p[1]}: ${getType(p[0])}`).join(', ')
}

// similar to getParams, but as-side
function getAsParams(params = []) {
  let found = false
  const out = (params.length === 1 && params[1] === 'void') ? '' : params.map(p => {
    if (p[0] === 'const char*') {
      found = true
    }
    return `${p[1]}: ${p[0] === 'const char*' ? 'string' : getType(p[0])}`
  }).join(', ')
  if (found) {
    return out
  }
}

// do conversion from as function to wasm (UTF8)
function getAsConversionParams(params = []) {
  return (params.length === 1 && params[1] === 'void') ? '' : params.map(p => {
    return p[0] === 'const char*' ? `String.UTF8.encode(${p[1]}, true)` : p[1]
  }).join(', ')
}

// C types mapped to accemblyscript
// TODO: look into fancier types: https://www.assemblyscript.org/types.html
const typeMap = {
  'Image': 'u32',
  'Image*': 'u32',
  'Color*': '[Color]',
  'int': 'i32',
  'int*': 'i32[]',
  'unsigned int': 'u32',
  'const char*': 'ArrayBuffer',
  'const unsigned char*': 'ArrayBuffer'
}

// map return to as-type
function getType(ret) {
  return typeMap[ret] || ret || 'void'
}

// these are aliases that wrap Image 0 (screen)
const screenAliases = [
  "ImageClearBackground",
  "ImageDrawPixel",
  "ImageDrawPixelV",
  "ImageDrawLine",
  "ImageDrawLineV",
  "ImageDrawCircle",
  "ImageDrawCircleV",
  "ImageDrawRectangle",
  "ImageDrawRectangleV",
  "ImageDrawRectangleRec",
  "ImageDrawRectangleLines",
  "ImageDraw",
  "ImageDrawText"
]

const screenAliasesLookup = {}

function outputFunction(func) {
  if (screenAliases.includes(func.name)) {
    screenAliasesLookup[ func.name ] = func
  }

  let additional = ''

  const asParams = getAsParams(func.params)
  const returns = getType(func.returns)
  let name = func.name

  if (asParams) {
    name = `_${name}`
    additional = `
export function ${func.name}(${asParams}): ${returns} {
  return ${name}(${getAsConversionParams(func.params)})
}`
  }

  return `// ${func.comment}
@external("env", "null0_${func.name}")
${ asParams ? '' : 'export '}declare function ${name}(${getParams(func.params)}): ${returns}${additional}`
}

// similar to outputFunction, but strip ^Image for screen-level functions
function outputAlias(func) {
  return outputFunction({
    ...func,
    name: func.name.replace(/^Image/, ''),
    comment: func.comment.replace('within an image', 'on the screen'),
    params: func.params.slice(1)
  })
}

let out = `// null0 assemblyscript header, generated ${(new Date()).toISOString()}

@unmanaged
export class Color {
  r: u8;
  g: u8;
  b: u8;
  a: u8;
}

@unmanaged
export class Vector3 {
  x: f32;
  y: f32;
  z: f32;
}

@unmanaged
export class Vector4 {
  x: f32;
  y: f32;
  z: f32;
  w: f32;
}

@unmanaged
export class Rectangle {
  x: f32;
  y: f32;
  height: f32;
  width: f32;
}

export const LIGHTGRAY: Color = { r:  200, g: 200, b: 200, a: 255 }
export const GRAY: Color      = { r:  130, g: 130, b: 130, a: 255 }
export const DARKGRAY: Color  = { r:  80,  g: 80,  b: 80 , a: 255 }
export const LIGHTGREY: Color = { r:  200, g: 200, b: 200, a: 255 }
export const GREY: Color      = { r:  130, g: 130, b: 130, a: 255 }
export const DARKGREY: Color  = { r:  80,  g: 80,  b: 80 , a: 255 }
export const YELLOW: Color    = { r:  253, g: 249, b: 0  , a: 255 }
export const GOLD: Color      = { r:  255, g: 203, b: 0  , a: 255 }
export const ORANGE: Color    = { r:  255, g: 161, b: 0  , a: 255 }
export const PINK: Color      = { r:  255, g: 109, b: 194, a: 255 }
export const RED: Color       = { r:  230, g: 41,  b: 55 , a: 255 }
export const MAROON: Color    = { r:  190, g: 33,  b: 55 , a: 255 }
export const GREEN: Color     = { r:  0,   g: 228, b: 48 , a: 255 }
export const LIME: Color      = { r:  0,   g: 158, b: 47 , a: 255 }
export const DARKGREEN: Color = { r:  0,   g: 117, b: 44 , a: 255 }
export const SKYBLUE: Color   = { r:  102, g: 191, b: 255, a: 255 }
export const BLUE: Color      = { r:  0,   g: 121, b: 241, a: 255 }
export const DARKBLUE: Color  = { r:  0,   g: 82,  b: 172, a: 255 }
export const PURPLE: Color    = { r:  200, g: 122, b: 255, a: 255 }
export const VIOLET: Color    = { r:  135, g: 60,  b: 190, a: 255 }
export const DARKPURPL: Color = { r:  112, g: 31,  b: 126, a: 255 }
export const BEIGE: Color     = { r:  211, g: 176, b: 131, a: 255 }
export const BROWN: Color     = { r:  127, g: 106, b: 79 , a: 255 }
export const DARKBROWN: Color = { r:  76,  g: 63,  b: 47 , a: 255 }
export const WHITE: Color     = { r:  255, g: 255, b: 255, a: 255 }
export const BLACK: Color     = { r:  0,   g: 0,   b: 0  , a: 255 }
export const BLANK: Color     = { r:  0,   g: 0,   b: 0  , a: 0   }
export const MAGENTA: Color   = { r:  255, g: 0,   b: 255, a: 255 }
export const RAYWHITE: Color  = { r:  245, g: 245, b: 245, a: 255 }

// IMPORTS

${funcs.filter(noFilter).map(outputFunction).join('\n\n')}

${screenAliases.map(f => outputAlias(screenAliasesLookup[f])).join('\n\n')}

`

console.log(out)