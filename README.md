# crisp-game-lib-portable

Minimal C-lang library for creating classic arcade-like mini-games running on devices and browsers (Experimental).

To make the [crisp-game-lib](https://github.com/abagames/crisp-game-lib) work on small devices, the library is implemented in C.

The goal now is to get it running on the [M5StickC PLUS](https://shop.m5stack.com/products/m5stickc-plus-esp32-pico-mini-iot-development-kit).

<a href="./docs/screenshot.gif"><img src="./docs/screenshot.gif" width="25%"></a>

[Emscripten](https://emscripten.org/) also makes it possible to run [games written in C](https://github.com/abagames/crisp-game-lib-portable/blob/main/src/c/game.c) on the browser. ([DEMO](https://abagames.github.io/crisp-game-lib-portable/build/))

## Unimplemented features

- Procedurally generated musics and [sound effects](https://abagames.github.io/crisp-game-lib/ref_document/functions/play.html)
- [Difficulty](https://abagames.github.io/crisp-game-lib/ref_document/variables/difficulty.html)
- [Score](https://abagames.github.io/crisp-game-lib/ref_document/functions/addScore.html)
- [Game over](https://abagames.github.io/crisp-game-lib/ref_document/functions/end.html), title screen and replay
- [Particle](https://abagames.github.io/crisp-game-lib/ref_document/functions/particle.html)
- [Color](https://abagames.github.io/crisp-game-lib/ref_document/functions/color.html)
- [Bar](https://abagames.github.io/crisp-game-lib/ref_document/functions/bar.html) and [box](https://abagames.github.io/crisp-game-lib/ref_document/functions/box.html)
- [Letter options](https://abagames.github.io/crisp-game-lib/ref_document/types/LetterOptions.html)
- [Game options](https://abagames.github.io/crisp-game-lib/ref_document/types/Options.html)
- Precise collision detection for texts and characters
- Utility functions
- [Input position](https://abagames.github.io/crisp-game-lib/ref_document/variables/input.pos.html)
