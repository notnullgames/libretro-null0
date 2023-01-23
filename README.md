# libretro-null0
Libretro-core based runtime for null0


## development

```
# setup tools
npm i

# build it
npm run build:host

# build some carts to test
npm run build:draw
npm run build:errors
npm run build:files
npm run build:http
npm run build:justlog
npm run build:random

# test with a simple node-based host
npm test build/justlog.wasm

# run on mac
/Applications/RetroArch.app/Contents/MacOS/RetroArch -L build/null0-libretro.dylib build/justlog.null0
```