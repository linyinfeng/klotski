# Klotski

A classic sliding-block puzzle game (also known as Hua Rong Dao / 华容道).
Drag pieces to free the 2x2 block and reach the exit with as few steps as possible.

## Build

### With Nix

```console
nix run
```

### Manually

Requires a current Qt (6.x) with Widgets support and Qt Linguist tools
(`lrelease`) on your PATH.

```console
mkdir build && cd build
qmake ../klotski.pro && make
cp -r ../share/{images,levels,solutions,help} .
./klotski
```

## Credits

Developed by Yinfeng Lin, Nianyi Wang, Zuanjie Ma, Yaodanjun Ren and Yutong
Deng. Character artwork is from Koei Tecmo Games' *Romance of the Three
Kingdoms 13*; the game should not be used for commercial purposes.

## License

MIT – see [LICENSE](LICENSE). Qt is used under the LGPL.
