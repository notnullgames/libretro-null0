# libretro-null0
Libretro-core based runtime for null0


## development

```
# build it
cmake -B build .
make -C build

# run on mac
/Applications/RetroArch.app/Contents/MacOS/RetroArch -L build/null0_libretro.dylib game.null0
```

I also added some demo-carts in [carts/](carts/):

```
# install tools
npm i

# get a list of build-targets
npm run

# build a simple cart that just says "Hello" in carts/build/justlog.null0
npm run build:justlog
```