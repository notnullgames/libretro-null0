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
npm run build:justlog
npm run build:random

# test with a simple node-based host
npm test build/justlog.wasm

# run on mac
/Applications/RetroArch.app/Contents/MacOS/RetroArch -L build/null0-libretro.dylib build/justlog.null0
```

## contributing

If you want to help out, we have [a mega-issue](https://github.com/notnullgames/libretro-null0/issues/1) tracking stuff for first release (and beyond.) PRs are extremely appreciated, and nothing is too small to add.


## thanks

I could not have made this without a great deal of help from others.

- [TIC80](https://tic80.com/) & [wasm4](https://wasm4.org/) and [lutro](https://www.libretro.com/index.php/lutro-easy-retro-game-creation-powered-by-libretro/) and [love2d](https://love2d.org/) are a constant source of inspiration. No code is used duirectly from these, but they helped me figure out what I was trying to do.
- [Raylib](https://www.raylib.com/) is so nice. Easy & fun to work with, fast, and has a ton of features, and Ray & the community around Raylib are incredibly helpful. I am not using it in the current engine, but they have been incredibly helpful in showing me stuff and just being generally nice people.
- [Node-raylib](https://github.com/RobLoach/node-raylib) is extremely helpful for prototyping & the other people that are working on it (@twuky and @RobLoach) have been really helpful discussing things, and hammering out ideas with, as well as just straight-up helping me figure things out that were really complicated. Also, lots of ideas from @RobLoach's [raylib-physfs](https://github.com/RobLoach/raylib-physfs).
- Gulrak wrote the [utf8 lib](https://gist.github.com/gulrak/2eda01eacebdb308787b639fa30958b3) that makes dealing with assemblyscript strings much easier, and was helpful with inciteful discussion. I ended up not using it, but their input was incredibly helpfuil.
- [physfs](https://icculus.org/physfs/) - this provides the fs-sandbox (locked into specific dir or cart zip file) in native runtime, and it works really well.
- [@dcodeIO](https://github.com/dcodeIO) for helping with types in assemblyscript, and wasm in general.
- retroarch discord - lots of nice peopel there willign to put up with dumb questions, and have inciteful answers to trobles I faced.
- [retroluxury](https://github.com/leiradel/retroluxury) really helped me figure out getting [soloud sound](https://github.com/jarikomppa/soloud) working in libretro. Lots of really great ideas in their engine(s).
