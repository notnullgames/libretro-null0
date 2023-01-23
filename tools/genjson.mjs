import { readFile } from 'fs/promises'

const regex = /^RLAPI (void|Image|Color|int|Vector4|Vector3|bool) \*?([A-Za-z]+)\((.+)\); +\/\/(.+)/gm

const str = await readFile('build/_deps/rimage-src/rimage.h', 'utf8')

let m

const out = []
while ((m = regex.exec(str)) !== null) {
  // This is necessary to avoid infinite loops with zero-width matches
  if (m.index === regex.lastIndex) {
    regex.lastIndex++
  }

  if (!m) {
    continue
  }

  const rec = {
    returns: m[1].trim(),
    name: m[2].trim(),
    params: m[3].split(',').map(s => s.trim().replace(' **', '** ').replace(' *', '* ').split(' ')),
    comment: m[4].trim()
  }

  if (rec.params.length === 1 && rec.params[0] === 'void') {
    rec.params = []
  }

  for (const p in rec.params) {
    if (rec.params[p][0] === 'const') {
      rec.params[p] = ['const ' + rec.params[p][1], ...rec.params[p].slice(2)]
    }
    if (rec.params[p][0] === 'unsigned') {
      rec.params[p] = ['unsigned ' + rec.params[p][1], ...rec.params[p].slice(2)]
    }
    if (rec.params[p][0] === 'const unsigned') {
      rec.params[p] = ['const unsigned ' + rec.params[p][1], ...rec.params[p].slice(2)]
    }
  }

  out.push(rec)
}

out.push({
  name: 'log',
  returns: 'void',
  params: [
    ['const char*', 'text']
  ],
  comment: 'Log a string'
})

out.push({
  name: 'fatal',
  returns: 'void',
  params: [
    ['const char*', 'message'],
    ['const char*', 'filename'],
    ['int', 'lineNumber'],
    ['int', 'columnNumber']
  ],
  comment: 'Fatal error - call this from your code on a fatal runtime error, similar to assemblyscript\'s abort(), but it\'s utf8'
})

out.push({
  name: 'ReadText',
  returns: 'const char*',
  params: [
    ['const char*', 'fileName']
  ],
  comment: 'Read a text-file from virtual filesystem'
})

out.push({
  name: 'WriteText',
  returns: 'void',
  params: [
    ['const char*', 'fileName'],
    ['const char*', 'contents']
  ],
  comment: 'Write a text-file to the virtual filesystem'
})

out.push({
  name: 'FileExists',
  returns: 'bool',
  params: [
    ['const char*', 'fileName']
  ],
  comment: 'Check if the file exists'
})

out.push({
  name: 'GetUrl',
  returns: 'const char*',
  params: [
    ['const char*', 'url']
  ],
  comment: 'Perform a GET request on URL'
})

console.log(JSON.stringify(out, null, 2))
