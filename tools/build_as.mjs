import archiver from 'archiver'
import fs from 'fs'
import shell from 'shelljs'
import asc from 'assemblyscript/asc'

const { test, cp, mkdir } = shell

const [, , target] = process.argv
if (!target) {
  console.error('Usage: build <TARGET>')
  process.exit(1)
}

function zip (sourceDir, outPath) {
  const archive = archiver('zip', { zlib: { level: 9 } })
  const stream = fs.createWriteStream(outPath)

  return new Promise((resolve, reject) => {
    archive
      .directory(sourceDir, false)
      .on('error', err => reject(err))
      .pipe(stream)

    stream.on('close', () => resolve())
    archive.finalize()
  })
}

const camelCaseToTitleCase = (s) =>
  s.split('').reduce(
    (acc, letter, i) => (
      i === 0
        ? [...acc, letter.toUpperCase()]
        : letter === letter.toUpperCase()
          ? [...acc, ' ', letter]
          : [...acc, letter]
    ), []
  ).join('')

// compile wat/wasm
const options = [
  '--use', 'abort=fatal',
  '--lib', 'carts/null0.ts',
  '--runtime', 'stub',
  '--exportRuntime',
  '--optimize',
  '--stats',
  `carts/${target}/main.ts`,
  '-o', `build/${target}.wasm`,
  '-t', `build/${target}.wat`,
  '--bindings', 'esm'
]

const { stats, stdout, stderr, error } = await asc.main(options)

if (error) {
  console.log('Compilation failed: ' + error.message)
  console.log(stderr.toString())
  process.exit(1)
} else {
  console.log(stdout.toString())
  console.log(`${target} built:`)
  for (const o of Object.keys(stats)) {
    if (o.includes('Time') || o === 'total') {
      console.log(`  ${camelCaseToTitleCase(o)}: ${(stats[o] / 1000000).toFixed(3)} ms`)
    } else {
      console.log(`  ${camelCaseToTitleCase(o)}: ${stats[o]}`)
    }
  }
}

mkdir('-p', `build/${target}`)
cp(`build/${target}.wasm`, `build/${target}/main.wasm`)

if (test('-d', `carts/${target}/assets`)) {
  cp('-R', `carts/${target}/assets/*`, `build/${target}/`)
}

await zip(`build/${target}`, `build/${target}.null0`)
